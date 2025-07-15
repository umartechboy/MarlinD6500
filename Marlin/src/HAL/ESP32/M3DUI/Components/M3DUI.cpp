#include "M3DUI.h"
#include "..\Hardware\BufferedDisplay.h"
#include "..\Hardware\Keypad.h"
#include "Utils.h"
#include "..\App\Bitmaps.h"

StringListItem::StringListItem(){
}
StringListItem::StringListItem(String str, int _endTrimLength){
    ItemText = str;
    endTrimLength = _endTrimLength;
}
void StringListItem::Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int y, bool selected) {
    
    uint8_t opBkp = g->GetOpacity();
    g->SetOpacity(op);
    g->setTextColor(TextColor);
    // }
    // else
    //     g->setTextColor(TextColor);
    //Serial.printf("Printing SD File %d: %s @ %d, %d\n", i, files[i].c_str(), g->width() / 2, FileLineCenter(i));
    int16_t w = 0;
    centerString(g, ItemText.substring(0, ItemText.length() - endTrimLength).c_str(), g->width() / 2, y, &w);
    if (selected){
        w = g->width() - w - 8;
        g->drawLine(0, y, w / 2, y, TextColor);
        g->drawLine(g->width(), y, g->width() - w / 2, y, TextColor);
    }
    g->SetOpacity(opBkp);
}

FileNameListItem::FileNameListItem(String str, String dosName, int _endTrimLength):StringListItem(str, _endTrimLength) {
    DOSName = dosName;
}
void ListSeparatorItem::Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int y, bool selected) {
    
    uint8_t opBkp = g->GetOpacity();
    uint8_t lineMargin = 10;
    uint8_t tempSectionHeight = 20;
    g->SetOpacity(10);
    for (int i =0; i < 5; i++)
        g->drawLine(lineMargin + i * 2, y - 5, g->width() - lineMargin - i * 2, y - 5, TextColor);
    g->SetOpacity(opBkp);
}

ColorSelectorListItem::ColorSelectorListItem(){
}
ColorSelectorListItem::ColorSelectorListItem(String str, int colorIndex){
    ItemText = str;
    selectedColorIndex = colorIndex;
}
void ColorSelectorListItem::incrementColor(){
    selectedColorIndex ++;
    if (selectedColorIndex >= sizeof(AvailableColors) / sizeof(AvailableColors[0]))
        selectedColorIndex = 0;
}
void ColorSelectorListItem::decrementColor(){
    selectedColorIndex --;
    if (selectedColorIndex < 0)
        selectedColorIndex = sizeof(AvailableColors) / sizeof(AvailableColors[0]) - 1;
}
void ColorSelectorListItem::Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int y, bool selected) {
    
    uint8_t opBkp = g->GetOpacity();
    g->SetOpacity(op);
    g->setTextColor(TextColor);
    // }
    // else
    //     g->setTextColor(TextColor);
    //Serial.printf("Printing SD File %d: %s @ %d, %d\n", i, files[i].c_str(), g->width() / 2, FileLineCenter(i));
    int16_t w = 0;
    int px = 8;
    int pxe = 2;
    int rSz = 18;
    int arrowSpace = 12;
    int aSz = 8;
    centerLeftString(g, ItemText.c_str(), px, y, &w);
    g->SetOpacity(100); // force full bright rect.
    g->fillRoundRect(g->width() - pxe - arrowSpace - rSz, y - rSz/2, rSz, rSz, 3, AvailableColors[selectedColorIndex]);
    g->SetOpacity(op); // revert to list opacity
    g->drawRoundRect(g->width() - pxe - arrowSpace - rSz, y - rSz/2, rSz, rSz, 3, TextColor);
    // Draw the label
    if (Label.length() > 0){
        centerString(g, Label.c_str(), g->width() - pxe - arrowSpace - rSz / 2, y);
    }
    if (selected){
        // Show arrows with the color
        g->fillTriangle(
            g->width() - pxe - arrowSpace - rSz - arrowSpace + aSz, y - aSz / 2, 
            g->width() - pxe - arrowSpace - rSz - arrowSpace + aSz, y + aSz / 2, 
            g->width() - pxe - arrowSpace - rSz - arrowSpace, y, 
            TextColor);
        g->fillTriangle(
            g->width() - pxe - aSz, y - aSz / 2, 
            g->width() - pxe - aSz, y + aSz / 2, 
            g->width() - pxe, y, 
            TextColor);
    }
    g->SetOpacity(opBkp);
}
VerticalList::VerticalList(int lineHeight, int _displayHeight, String emptyString){
    for (int i =0; i < items.size(); i++)
        items[i] = 0;
    this->lineHeight = lineHeight;
    EmptyString = emptyString;
    displayHeight = _displayHeight;
}
VerticalList::~VerticalList(){
    for (int i = 0; i < items.size(); i++){
        if (items[i]) delete items[i];
    }
}
int VerticalList::getSelectedIndex(){
    return selected;
}
ListItem* VerticalList::getSelected(){
    return items[selected];
}
// Overload operator[] for non-const access
ListItem* VerticalList::operator[](int index) {
    if (index < 0 || index >= items.size()) {
        return 0;
    }
    return items[index];
}
void VerticalList::Clear(){
    items.clear();
}
void VerticalList::Add(ListItem* item){
    items.push_back(item);
}
int VerticalList::Count(){
    return items.size();
}
void VerticalList::scrollDown(){ // List goes up, selection goes down
    targetScrollOffset -= lineHeight;
    if (selected + 1 < Count()){
        if (items[selected + 1]->IsDummyItem()) // skip over
            targetScrollOffset -= lineHeight;
    }
}
void VerticalList::scrollUp(){ // List goes down, selection goes up
    targetScrollOffset += lineHeight;
    if (selected - 1 >= 0){
        if (items[selected - 1]->IsDummyItem()) // skip over, double scroll
            targetScrollOffset += lineHeight;
    }
}
void VerticalList::Paint(BufferedDisplay* g, Color TextColor){
    g->setTextColor(TextColor);
    if (targetScrollOffset != scrollOffset){
        scrollOffset = (scrollOffset + targetScrollOffset) / 2;
    }
    if (items.size() == 0)
        centerString(g, emptyString.c_str(), g->width() / 2, displayHeight / 2);
    else{

        // Adjust underflow from the top
        if (scrollOffset > 0) {
            targetScrollOffset = 0;
            SERIAL_IMPL.println("Adjust underflow 1");
        }
        else if (scrollOffset < -((int)items.size() - 1) * lineHeight) {
            targetScrollOffset = -((int)items.size() - 1) * lineHeight;
            SERIAL_IMPL.printf("Adjust underflow 2 %d < %d\n", scrollOffset, -((int)items.size() - 1) * lineHeight);
        }
        for (int i = 0; i < items.size(); i++){
            g->setFont();
            //if (i * lineHeight + scrollOffset > - lineHeight / 2 && i * lineHeight + scrollOffset < lineHeight / 2){
            
            int16_t op = (abs(i * lineHeight + scrollOffset) * 100) / displayHeight;
            if (op < 0) op = 0;
            if (op > 100) op = 100;
            op = 100 - op;
            // 100, 90, 80, 70, 60
            if (op < 95){ 
                op -= 50; 
                if (op < 0) op = 0;
            }

            //Serial.printf("Printing SD File %d: %s @ %d\n", i, files[i].c_str(), op);
            
            // if (op > 100)
            //     op = 100;
            // op = 100 - op;
            // if (op < 95){
            //     op -= 50;
            //     op *= 2;
            // }

            int lY = displayHeight / 2 + i * lineHeight + scrollOffset;
            if (op > 95)
                selected = i;
            items[i]->Paint(g, op, TextColor, lY, i == selected);
        }
    }
}

MenuStep::MenuStep(MenuHost* host){
    Host = host;
}
void MenuStep::loop(){
    if (TickPeriod == 0) return; 
    if (millis() - lastTick > TickPeriod){
        lastTick = millis();
        Tick();
    }
}
MenuHost::MenuHost(){
    keypad = new KeyPad();
}
MenuHost::~MenuHost(){
    delete keypad;
}
bool MenuHost::CanGotoNextStep() {
    return TargetStep == 0;
}
void MenuHost::GotoStepFromNull(MenuStep* step) {
    TargetStep = step;
    ResetAnimationProgress(500);
    menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
}
void MenuHost::GotoNextStep(){
    if (CurrentStep->NextStep) {
        SERIAL_IMPL.println("Going to next step");
        // begin Next menu transition
        if (stepAnimationStage == StepAnimationStage::MainStep){
            menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
            TargetStep = CurrentStep->NextStep;
        }
        else
        {
            stepAnimationStage = StepAnimationStage::GoingToStep;
            moveToMenuAfterStepAnim = CurrentStep->NextStep;
        }
        menuTransitionDirection = TransitionDirection::Forward;
        if (CurrentStep->NextStep)
            CurrentStep->NextStep->LoadBegin();
        CurrentStep->UnloadBegin();
        ResetAnimationProgress(250);
    }
}
void MenuHost::GotoPreviousStep(){
    if (CurrentStep->PreviousStep){ 
        SERIAL_IMPL.println("Going to previous step");
        // begin Next menu transition
        if (stepAnimationStage == StepAnimationStage::MainStep) {
            menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
            TargetStep = CurrentStep->PreviousStep;
        }
        else {
            stepAnimationStage = StepAnimationStage::GoingToStep;
            moveToMenuAfterStepAnim = CurrentStep->PreviousStep;
        }
        menuTransitionDirection = TransitionDirection::Backward;
        if (CurrentStep->PreviousStep)
            CurrentStep->PreviousStep->LoadBegin();
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
        //g->fillRect(0,0, g->width(), g->height(), rgb(0, 0, 0)); // backdrop // this is too much load. Gotto skip this one.
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
        (&bmp_Back)->Draw(g, mw - x, x - (&bmp_Back)->height());
    }
    if(step->Icon && progress > 0){
        //int xOffset = MixFloats(width, 0, progress, MixType::EaseInEaseOut);
        //int yOffset = MixFloats(height, 0, progress, MixType::EaseInEaseOut);
        int x = height * width / (height + width); // anim Bounding box width and height
        switch (index)
        {
            case 1: step->Icon->Draw(g, x - step->Icon->width(), x - step->Icon->height()); break;
            case 2: step->Icon->Draw(g, mw - x, mh - x, true); break;
            //case 3: (&bmp_Back)->Draw(g, mw - x, mh - x); break;
        
            default:
                break;
        }
    }

    g->SetOpacity(opBkp);
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
    if (menuTrasnsitionStage == MenuTransitionStage::InStep){      
        if (CurrentStep){
            if (stepAnimationStage == StepAnimationStage::MainStep || stepAnimationStage == StepAnimationStage::InOverlay) {
                if (CurrentStep->NeedsRedraw) { // once there, draw only if needed     
                    CurrentStep->NeedsRedraw = false;                   
                    //Serial.println("In step");
                    CurrentStep->Paint(bTft);
                    if (stepAnimationStage == StepAnimationStage::InOverlay)
                        DrawButton3(CurrentStep, bTft, 1);
                    if (CurrentStep->NextStep){
                        DrawButton2(CurrentStep->NextStep, bTft, stepAnimationStage == StepAnimationStage::MainStep ? 0 : 1);
                    }
                    if (CurrentStep->PreviousStep){
                        DrawButton1(CurrentStep->PreviousStep, bTft, stepAnimationStage == StepAnimationStage::MainStep ? 0 : 1);
                    }
                }
            }
            else if (stepAnimationStage == StepAnimationStage::GoingToOverLay || stepAnimationStage == StepAnimationStage::GoingToStep) {
                //Serial.printf("Going to: %s\n", stepAnimationStage == StepAnimationStage::GoingToOverLay ? "overlay" : "step");
                CurrentStep->Paint(bTft);
                
                DrawButton3(CurrentStep, bTft, stepAnimationStage == StepAnimationStage::GoingToOverLay? animationStepProgress:(1 - animationStepProgress));
                if (CurrentStep->NextStep){
                    DrawButton2(CurrentStep->NextStep, bTft, stepAnimationStage == StepAnimationStage::GoingToOverLay? animationStepProgress:(1 - animationStepProgress));
                }
                if (CurrentStep->PreviousStep){
                    DrawButton1(CurrentStep->PreviousStep, bTft, stepAnimationStage == StepAnimationStage::GoingToOverLay? animationStepProgress:(1 - animationStepProgress));
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
            
        IncrementAnimationProgress();
        if (animationStepProgress >= 1.0f) {
            ResetAnimationProgress(250);
            menuTrasnsitionStage = MenuTransitionStage::InStep;            
            if (CurrentStep)
                CurrentStep->UnloadComplete();
            CurrentStep = TargetStep;
            TargetStep = 0;
            CurrentStep->Paint(bTft); // Confirm load
            CurrentStep->LoadComplete();            
        }
    }
    
    if (TargetStep != 0) { // we have a transition animation
        
    }
    bTft->update(true, true); 
    
}
void MenuHost::Loop(BufferedDisplay* bTft){
    keypad->Loop(this);
    Paint(bTft);
    if (CurrentStep)
    CurrentStep->loop();
    if (TargetStep)
    TargetStep->loop();

}