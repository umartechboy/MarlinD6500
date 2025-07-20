#include "MenuHost.h"
#include "..\App\Images.h"

MenuHost::MenuHost(){
    keypad = new KeyPad();
}
MenuHost::~MenuHost(){
    delete keypad;
}
bool MenuHost::CanGotoNextStep() {
    return TargetStep == 0;
}
void MenuHost::GotoStepFromAny(MenuStep* step) {
    TargetStep = step;
    ResetAnimationProgress(500);
    menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
}
void MenuHost::GotoNextStep(){
    if (!CurrentStep)
        return;
    if (Retro){
        if (!CurrentStep->GetNextStep()){
            PushNotification("No options at the current stage");
            return;
        }
        SERIAL_IMPL.println("Going to retro options step");
            menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
        TargetStep = CurrentStep->RetroNextStep;
        menuTransitionDirection = TransitionDirection::Forward;
        CurrentStep->RetroNextStep->LoadBegin();
        CurrentStep->UnloadBegin();
        ResetAnimationProgress(250);
    }
    else 
    {
        if (CurrentStep->GetNextStep()) {
            SERIAL_IMPL.println("Going to next step");
            // begin Next menu transition
            if (stepAnimationStage == StepAnimationStage::MainStep){
                menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
                TargetStep = CurrentStep->GetNextStep();
            }
            else
            {
                stepAnimationStage = StepAnimationStage::GoingToStep;
                moveToMenuAfterStepAnim = CurrentStep->GetNextStep();
            }
            menuTransitionDirection = TransitionDirection::Forward;
            if (CurrentStep->GetNextStep())
                CurrentStep->GetNextStep()->LoadBegin();
            CurrentStep->UnloadBegin();
            ResetAnimationProgress(250);
        }
    }
}
void MenuHost::GotoPreviousStep(){
    if (CurrentStep->GetPreviousStep()){ 
        SERIAL_IMPL.println("Going to previous step");
        // begin Next menu transition
        if (stepAnimationStage == StepAnimationStage::MainStep) {
            menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
            TargetStep = CurrentStep->GetPreviousStep();
        }
        else {
            stepAnimationStage = StepAnimationStage::GoingToStep;
            moveToMenuAfterStepAnim = CurrentStep->GetPreviousStep();
        }
        menuTransitionDirection = TransitionDirection::Backward;
        if (CurrentStep->GetPreviousStep())
            CurrentStep->GetPreviousStep()->LoadBegin();
        CurrentStep->UnloadBegin();
        ResetAnimationProgress(250);
    }
}
bool MenuHost::NeedsMenuTransition() {
    return (CurrentStep != TargetStep) && TargetStep != 0;
}

void MenuHost::ResetAnimationProgress(long duration){
    animationStepProgress = 0.0f;
    animationStartTime = millis();
    animationEndTime = animationStartTime + duration;
}
void MenuHost::IncrementAnimationProgress(){
    long currentTime = millis();
    if (currentTime < animationEndTime) {
        animationStepProgress = (float)(currentTime - animationStartTime) / (animationEndTime - animationStartTime);
    } else {
        animationStepProgress = 1.0f;
    }
}
void MenuHost::DrawButton(int index, MenuStep* step, BufferedDisplay* g, float progress){
    // Its a triangle at the top left corner in the button color of the step.
    //Serial.printf("Draw button %d @ %f\n", index, progress);
    
    int bSizeW = index == 3 ? 60:100;
    float aspect = 0.8F;
    int bSizeH = bSizeW * aspect;
    int width = MixFloats(6, bSizeW, progress, MixType::EaseInEaseOut); // goes from from 6 to 100 
    int height = width * aspect; // height is fraction of width
    int opBkp = g->GetOpacity();
    int mw = g->width() - 1;
    int mh = g->height() - 1;
    
    // Draw a shadow if the progress is more than 0
    if (progress > 0){
        //g->SetOpacity(10.0F * progress);
        //g->fillRect(0,0, Host->appWidth(), Host->appHeight(), rgb(0, 0, 0)); // backdrop // this is too much load. Gotto skip this one.
        g->SetOpacity(10);
        for (int si = 0; si <= progress * 20; si += 5)
        //int si = 28;
        { // maximum 5 times            
            // we need a polygon offsetting from the inner edge of the triangle. Gotto use two triangles for that.
            switch (index)
            {
                case 1:
                g->fillTriangle(width - 1, 0, width + si, 0, 0, height - 1, rgb(0, 0, 0));
                g->fillTriangle(0, height + si, width + si, 0, 0, height - 1, rgb(0, 0, 0)); break;
                case 2:
                g->fillTriangle(mw - (width - 1), mh, mw - (width + si), mh, mw, mh - (height - 1), rgb(0, 0, 0));
                g->fillTriangle(mw, mh - (height - 1), mw - (width + si), mh, mw, mh - (height + si), rgb(0, 0, 0)); break;
                case 3: 
                g->fillTriangle(mw - (width - 1), 0, mw - (width + si), 0, mw, height - 1, rgb(0, 0, 0));
                g->fillTriangle(mw, height + si - 1, mw - (width + si), 0, mw, height - 1, rgb(0, 0, 0)); break;
                default:
                break;
            }
        }
    }
    g->SetOpacity(100);
    switch (index)
    {
        case 1: g->fillTriangle(0,0, width, 0, 0, height, step->ButtonColor); break;
        case 2: g->fillTriangle(mw - 0, mh - 0, mw - width, mh - 0, mw - 0, mh - height, step->ButtonColor); break;
        case 3: g->fillTriangle(mw - 0, 0, mw - width, 0, mw - 0, height, step->ButtonColor); break;
    
        default:
            break;
    }

    // Draw image
    switch (index)
    {
        case 1: g->fillTriangle(0,0, width, 0, 0, height, step->ButtonColor); break;
        case 2: g->fillTriangle(mw - 0, mh - 0, mw - width, mh - 0, mw - 0, mh - height, step->ButtonColor); break;
        case 3: g->fillTriangle(mw - 0, 0, mw - width, 0, mw - 0, height, step->ButtonColor); break;
    
        default:
            break;
    }
    
    if (index == 3 && progress > 0){
        int x = height * width / (height + width); // anim Bounding box width and height
        (&img_Back)->Draw(g, mw - x, x - (&img_Back)->height());
    }
    if(step->Icon && progress > 0){
        //int xOffset = MixFloats(width, 0, progress, MixType::EaseInEaseOut);
        //int yOffset = MixFloats(height, 0, progress, MixType::EaseInEaseOut);
        int x = height * width / (height + width); // anim Bounding box width and height
        switch (index)
        {
            case 1: step->Icon->Draw(g, x - step->Icon->width(), x - step->Icon->height()); break;
            case 2: step->Icon->Draw(g, mw - x, mh - x, true); break;
            //case 3: (&img_Back)->Draw(g, mw - x, mh - x); break;
        
            default:
                break;
        }
    }

    g->SetOpacity(opBkp);
}
int MenuHost::appHeight(){
    return _appHeight;
}
int MenuHost::appWidth(){
    return _appWidth;
}
// Top left
void MenuHost::DrawButton1(MenuStep* step, BufferedDisplay* g, float progress){
    DrawButton(1, step, g, progress);
}
// Bottom Right
void MenuHost::DrawButton2(MenuStep* step, BufferedDisplay* g, float progress){
    // Just like the Button 1, but in the bottom right corner.
    DrawButton(2, step, g, progress);
}   
// Top Right
void MenuHost::DrawButton3(MenuStep* step, BufferedDisplay* g, float progress){        
    // Just like the Button 1, but in the top right corner.
    DrawButton(3, step, g, progress);
}
void MenuHost::Paint(BufferedDisplay* bTft){
    if (millis() - lastPaint < 20) { // limit to 50 fps
        return;
    }
    if (Retro){
        _appWidth = bTft->width();
        _appHeight = bTft->height() - 12;
    }
    else{
        _appWidth = bTft->width();
        _appHeight = bTft->height();
    }
    if (menuTrasnsitionStage == MenuTransitionStage::InStep){      
        if (CurrentStep){
            if (stepAnimationStage == StepAnimationStage::MainStep || stepAnimationStage == StepAnimationStage::InOverlay) {
                if (CurrentStep->NeedsRedraw) { // once there, draw only if needed     
                    CurrentStep->NeedsRedraw = false;                   
                    //Serial.println("In step");
                    CurrentStep->Paint(bTft);
                    // Draw the overlay buttons and hints in normal mode
                    if(Retro){
                        // Retro shows the overlay options hint and screen
                        bTft->yOffset = 0;                        
                        bTft->xOffset = 0;
                        CurrentStep->PaintRetroTitle(bTft);
                        bTft->yOffset = bTft->height() - retroNavSectionHeight;
                        CurrentStep->PaintRetroOptionsBar(bTft);
                        bTft->yOffset = 0; // reset
                    }
                    else {
                        if (stepAnimationStage == StepAnimationStage::InOverlay)
                            DrawButton3(CurrentStep, bTft, 1);
                        if (CurrentStep->GetNextStep()){
                            DrawButton2(CurrentStep->GetNextStep(), bTft, stepAnimationStage == StepAnimationStage::MainStep ? 0 : 1);
                        }
                        if (CurrentStep->GetPreviousStep()){
                            DrawButton1(CurrentStep->GetPreviousStep(), bTft, stepAnimationStage == StepAnimationStage::MainStep ? 0 : 1);
                        }
                    }
                }
            }
            // Doesn't happen in Retro
            else if (stepAnimationStage == StepAnimationStage::GoingToOverLay || stepAnimationStage == StepAnimationStage::GoingToStep) {
                //Serial.printf("Going to: %s\n", stepAnimationStage == StepAnimationStage::GoingToOverLay ? "overlay" : "step");
                CurrentStep->Paint(bTft);
                
                // Draw the overlay buttons and hints in normal mode
                if(!Retro){
                    DrawButton3(CurrentStep, bTft, stepAnimationStage == StepAnimationStage::GoingToOverLay? animationStepProgress:(1 - animationStepProgress));
                    if (CurrentStep->GetNextStep()){
                        DrawButton2(CurrentStep->GetNextStep(), bTft, stepAnimationStage == StepAnimationStage::GoingToOverLay? animationStepProgress:(1 - animationStepProgress));
                    }
                    if (CurrentStep->GetPreviousStep()){
                        DrawButton1(CurrentStep->GetPreviousStep(), bTft, stepAnimationStage == StepAnimationStage::GoingToOverLay? animationStepProgress:(1 - animationStepProgress));
                    }
                }
                IncrementAnimationProgress();
                if (animationStepProgress >= 1){
                    if (stepAnimationStage == StepAnimationStage::GoingToOverLay) {
                        stepAnimationStage = StepAnimationStage::InOverlay;                        
                        if (CurrentStep)
                            CurrentStep->FocusChanged(StepAnimationStage::InOverlay);
                        CurrentStep->NeedsRedraw = true;     
                    }
                    else if (stepAnimationStage == StepAnimationStage::GoingToStep) {
                        stepAnimationStage = StepAnimationStage::MainStep;
                        if (CurrentStep)
                            CurrentStep->FocusChanged(StepAnimationStage::MainStep);
                        
                        if (moveToMenuAfterStepAnim) {// we are back to menu but need to move to the next or previous                                
                            ResetAnimationProgress(250);
                            menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
                            TargetStep = moveToMenuAfterStepAnim; // this should trigger the menu change animation
                            moveToMenuAfterStepAnim = 0;
                        }
                        CurrentStep->NeedsRedraw = true;     
                    }
                }
            }                
        }
    }
    else if (menuTrasnsitionStage == MenuTransitionStage::TransitionaingScreens){
        //Serial.printf("Transitioning screens: %f\n", animationStepProgress);
        // There are no buttons, just need to slide the screens
        
        bTft->xOffset = MixFloats(menuTransitionDirection==TransitionDirection::Forward?128:-128, 0, animationStepProgress, MixType::EaseInEaseOut);                
        if (TargetStep)
            TargetStep->Paint(bTft);
        bTft->xOffset = MixFloats(0, menuTransitionDirection==TransitionDirection::Forward?-128:128, animationStepProgress, MixType::EaseInEaseOut);
        if (CurrentStep)
            CurrentStep->Paint(bTft);
        
        bTft->xOffset = 0;
        // Draw the overlay buttons and hints in normal mode
        if(Retro){
            // Retro shows the overlay options hint and screen
            if (CurrentStep){
                bTft->yOffset = MixFloats(0, -retroTitleSectionHeight, animationStepProgress, MixType::EaseInEaseOut);
                CurrentStep->PaintRetroTitle(bTft);
                bTft->yOffset = MixFloats(bTft->height() - retroNavSectionHeight, bTft->height(), animationStepProgress, MixType::EaseInEaseOut);
                CurrentStep->PaintRetroOptionsBar(bTft);
            }
            if (TargetStep){
                bTft->yOffset = MixFloats(-retroTitleSectionHeight, 0, animationStepProgress, MixType::EaseInEaseOut);
                TargetStep->PaintRetroTitle(bTft);
                bTft->yOffset = MixFloats(bTft->height(), bTft->height() - retroNavSectionHeight, animationStepProgress, MixType::EaseInEaseOut);
                TargetStep->PaintRetroOptionsBar(bTft);
            }
            bTft->yOffset = 0; // reset
        }
        
            
        IncrementAnimationProgress();
        if (animationStepProgress >= 1.0f) { // this happens before the animation is complete. So need to manually draw the last frame
            ResetAnimationProgress(250);
            menuTrasnsitionStage = MenuTransitionStage::InStep;            
            if (CurrentStep)
                CurrentStep->UnloadComplete();
            CurrentStep = TargetStep;
            TargetStep = 0;
            CurrentStep->Paint(bTft); // Confirm load            
            if(Retro){
                // Retro shows the overlay options hint and screen
                bTft->yOffset = 0;                        
                bTft->xOffset = 0;
                CurrentStep->PaintRetroTitle(bTft);
                bTft->yOffset = bTft->height() - retroNavSectionHeight;
                CurrentStep->PaintRetroOptionsBar(bTft);
                bTft->yOffset = 0; // reset
            }
            CurrentStep->LoadComplete();            
        }
    }
    
    if (TargetStep != 0) { // we have a transition animation
        
    }
    if (CurrentNotification){
        CurrentNotification->Paint(bTft);
        if (CurrentNotification->lifeLeft <= 0){
            delete CurrentNotification;
            CurrentNotification = 0;
        }
    }

    bTft->update(true, true); 
    
}
void MenuHost::PushNotification(const char* str, int life){
    if (CurrentNotification)
        delete CurrentNotification;
    String str2 = String(str);
    CurrentNotification = new Notification(str2, life);
}
void MenuHost::Loop(BufferedDisplay* bTft){
    keypad->Loop(this);
    Paint(bTft);
    if (CurrentStep)
    CurrentStep->loop();
    if (TargetStep)
    TargetStep->loop();

}