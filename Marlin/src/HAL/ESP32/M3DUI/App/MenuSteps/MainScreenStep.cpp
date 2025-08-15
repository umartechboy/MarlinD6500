#include "MainScreenStep.h"
#include "..\Images.h"
#include "../../../../../module/printcounter.h"
#include "../../../../../sd/cardreader.h"
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include "../../Hardware/MarlinSpecific.h"
// #include "../../../LoadCell/LoadCell.h"
#include "..\MenuApp.h"


void printComSent(void* sender){
    MainScreenStep* This = (MainScreenStep*)sender;
    This->RetroNextStep = &inPrintMenuStep;
    This->NextActionString = "Pause";
    This->printStarted = true;
    This->printStatus = PrintStatus::Printing;
}
void cancelResumeCalled(void* sender){
    MainScreenStep* This = (MainScreenStep*)sender;
    card.removeJobRecoveryFile();
    This->printStatus = PrintStatus::Idle;
    This->RetroNextStep = &retroMainMenuStep;
    This->RetroPreviousStep = 0;
}
void beginResumeCalled(void* sender){
    MainScreenStep* This = (MainScreenStep*)sender;
    SERIAL_IMPL.println("Resume print");
    This->printStatus = PrintStatus::FileToPrint;
    // Trigger the resume too
    card.startOrResumeFilePrinting();
    This->RetroNextStep = &inPrintMenuStep;
    This->RetroPreviousStep = 0;
}
MainScreenStep::MainScreenStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkRed;
    BackColor = DarkRed;
    TextColor = ST7735_WHITE;            
    Title = "M3D Enabler";
    RetroIcon = &img_RetroM3D;  
    NextStep = &sdMenuStep;    
    PreviousStep = &toolsMenuStep;
    RetroNextStep = &retroMainMenuStep;

    inPrintMenuStep.RetroPreviousStep = this;
    inPrintMenuStep.RetroIcon = &img_RetroHome;
    inPrintMenuStep.Title = "Paused";

    cancelResumeDummyStep = new DummyMenuStep(Host);
    beginResumeDummyStep = new DummyMenuStep(Host);

    cancelResumeDummyStep->SetLoadCallBack(cancelResumeCalled, this);
    beginResumeDummyStep->SetLoadCallBack(beginResumeCalled, this);
    Icon = &img_Home;
    TickPeriod = 50;
}
MainScreenStep::~MainScreenStep(){
    delete cancelResumeDummyStep;
    delete beginResumeDummyStep;
}
MenuStep* MainScreenStep::GetPreviousStep(bool returnEvenIfDummy){
    if (Host->Retro)
        return 0;
    else return MenuStep::GetPreviousStep(returnEvenIfDummy);
}
// long lastReset = 0;
// extern bool ProbeEnable; 
// extern float lastAnalogReturn;
// extern void removeLoadCellOffset();
void MainScreenStep::Tick() {
    NeedsRedraw = true;
    if (printStatus == PrintStatus::FileToPrint){
        if (!printStarted)
            printJobTick();
    }
    // ProbeEnable = true;
    // if (millis() - lastReset > 10000){
    //     removeLoadCellOffset();
    //     lastReset = millis();
    // }
}
void MainScreenStep::Paint(BufferedDisplay* g) {
    
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    if (printStatus == PrintStatus::PrintToResume){
        g->setFont(&FreeSans9pt7b);
        int _w, _h;
        drawMultilineCenteredText(g, "Do you want to resume the print?", Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2, Host->appWidth(), 16, &_w, &_h);
        g->setFont(&FreeSans12pt7b);
        centerString(g, "Yes", (Host->appWidth() * 1) / 4, retroTitleSectionHeight + Host->appHeight() / 2 + _h / 2 + 10);
        centerString(g, "No",  (Host->appWidth() * 3) / 4, retroTitleSectionHeight + Host->appHeight() / 2 + _h / 2 + 10);
        int tSz = 12;
        g->fillTriangle(
            Host->appWidth() / 4 - tSz / 2, Host->appHeight() / 2 + 49, 
            Host->appWidth() / 4 + tSz / 2, Host->appHeight() / 2 + 49 - tSz / 2, 
            Host->appWidth() / 4 + tSz / 2, Host->appHeight() / 2 + 49 + tSz / 2, 
            TextColor);
        g->fillTriangle(
            (Host->appWidth() * 3) / 4 + tSz / 2, Host->appHeight() / 2 + 49, 
            (Host->appWidth() * 3) / 4 - tSz / 2, Host->appHeight() / 2 + 49 - tSz / 2, 
            (Host->appWidth() * 3) / 4 - tSz / 2, Host->appHeight() / 2 + 49 + tSz / 2, 
            TextColor);
    }
    else {
        if(printStatus == PrintStatus::Idle){
            g->setFont(&FreeSans12pt7b);
            centerString(g, "Ready!", Host->appWidth() / 2, Host->appTop() + Host->appHeight() / 2);
            // String lStr;
            // lStr = String(lastAnalogReturn);
            // if (millis() - lastReset > 9000){
            //     lStr = "...";
            // }
            // centerString(g, lStr.c_str(), Host->appWidth() / 2, Host->appTop() + Host->appHeight() / 2);
        }
        else{        
            int titleHeight = 10;
            if (Host->Retro)
                titleHeight = retroTitleSectionHeight + 14;
            
            if (card.isPrinting())
                centerLeftString(g, "Printing", 2, titleHeight);
            else if (card.isPaused())
                centerLeftString(g, "Paused", 2, titleHeight);
            else {// Must have ended
                centerLeftString(g, "All Done!", 2, titleHeight);
            }

            centerRightString(g, (String((print_job_timer.getStats().filamentUsed - materialAtStart) / 1000, 3) + String("m")).c_str(), Host->appWidth() - 2, titleHeight);
            
            int pbh = 8;
            g->SetOpacity(50);
            float pcCommplete = card.percentDone();
            //pcCommplete = 24.4;
            g->drawRoundRect(1, Host->appTop() + Host->appHeight() / 2 - 5, Host->appWidth() - 2, pbh, pbh / 2, TextColor);
            g->SetOpacity(100);
            g->fillRoundRect(1, Host->appTop() + Host->appHeight() / 2 - 5, ((Host->appWidth() - 2) * pcCommplete) / 100, pbh, pbh / 2, TextColor);    
            g->setFont();
            centerString(g, (String(pcCommplete, 1) + String("%")).c_str(), Host->appWidth() / 2, Host->appTop() + Host->appHeight() / 2 - 14);
            g->setFont();
            centerString(g, fileName.c_str(), Host->appWidth() / 2, Host->appTop() + Host->appHeight() / 2 + pbh + 5);
        }
        if (Host->Retro){
        }
        else{            
            uint8_t opBkp = g->GetOpacity();
            uint8_t lineMargin = 10;
            uint8_t tempSectionHeight = 20;
            g->SetOpacity(10);
            for (int i =0; i < 5; i++)
                g->drawLine(lineMargin + i * 2, Host->appHeight() - tempSectionHeight, Host->appWidth() - lineMargin - i * 2, Host->appHeight() - tempSectionHeight, TextColor);
            g->SetOpacity(opBkp);
            g->drawLine(Host->appWidth() / 2, Host->appHeight() - tempSectionHeight, Host->appWidth() / 2, Host->appHeight(), g->readPixel(Host->appWidth() / 2, Host->appHeight() - tempSectionHeight));            
            
            g->setFont();
            String temp1 = String(readTemp1(), 1) + " C";
            String temp2 = String(readTemp2(), 1) + " C";
            int16_t tw, th;
            centerString(g, temp1.c_str(), Host->appWidth() / 4, Host->appHeight() + 1 - tempSectionHeight / 2, &tw, &th);
            g->drawCircle(Host->appWidth() / 4 + tw / 2 - 9, Host->appHeight() + 1 - tempSectionHeight + 5, 1, TextColor);
            centerString(g, temp2.c_str(), (3 * Host->appWidth()) / 4, Host->appHeight() + 1 - tempSectionHeight / 2, &tw, &th);
            g->drawCircle((3 * Host->appWidth()) / 4 + tw / 2 - 9, Host->appHeight() + 1 - tempSectionHeight + 5, 1, TextColor);
            g->fillRect(0, Host->appHeight() - 2, Host->appWidth() / 2, 2, e0Color);
            g->fillRect(Host->appWidth() / 2 + 1, Host->appHeight() - 2, Host->appWidth() / 2, 2, e1Color);
        }
    }
}

void MainScreenStep::HandleKeyPress(Keys key){
    if (!Host->Retro) {
        if (printStatus == PrintStatus::Idle){
            if (key == KEYPAD_LEFT && !Host->Retro){
                SERIAL_IMPL.println("Go to previous from Idle");
                Host->GotoPreviousStep();
            }
            else if (key == KEYPAD_RIGHT && !Host->Retro){
                SERIAL_IMPL.println("Go to next from Idle");
                Host->GotoNextStep();
            }
        }
        else if (printStatus == PrintStatus::PrintToResume) { 
            if (key == KEYPAD_LEFT) {
                SERIAL_IMPL.println("Resume print");
                printStatus = PrintStatus::FileToPrint;
                // Trigger the resume too
                card.startOrResumeFilePrinting();
            }
            else if (key == KEYPAD_RIGHT){                
                SERIAL_IMPL.println("Don't resume print");
                card.removeJobRecoveryFile();
                PreviousStep = &toolsMenuStep;
                NextStep = &sdMenuStep;
                printStatus = PrintStatus::Idle;
                fileName = "";
            }
        } else {        
            if (!card.isPrinting() & !card.isPaused()){            
                SERIAL_IMPL.println("Turning to idle menu (1)");
                // No print. The job must have finished. A click should bring the printer to the idle state.
                NextStep = &sdMenuStep;
                PreviousStep = &toolsMenuStep;
                printStatus = PrintStatus::Idle;
                fileName = "";
            }
        }
    }
}

void MainScreenStep::LoadComplete(){
    // Get colors
    prefs.begin("material");
    e0Color = AvailableColors[prefs.getInt("e1_c", 0)];
    e1Color = AvailableColors[prefs.getInt("e2_c", 1)];
    prefs.end();

    if (Host->Retro){
        if (printStatus == PrintStatus::FileToPrint) { // This must be set by the file selection menus.
            SERIAL_IMPL.printf("Home screen with print file: %s, %s\n", fileName.c_str(), DOSFileName.c_str());
            materialAtStart = print_job_timer.getStats().filamentUsed;
            RetroPreviousStep = 0;
            RetroNextStep = 0;
            NextActionString = "";
            printStarted = false;
            prepareForPrint(DOSFileName, true, true, printComSent, this);            
        }
        else if (printStatus == PrintStatus::Printing) {
            if (card.isPaused()) { // came back from the menu with back button
                card.startOrResumeFilePrinting();
            }
        }
    }
    else {
        // Get the status and set the mode
        if (printStatus == PrintStatus::FileToPrint) { // This must be set by the file selection menus.
            SERIAL_IMPL.printf("Home screen with print file: %s, %s\n", fileName.c_str(), DOSFileName.c_str());
            materialAtStart = print_job_timer.getStats().filamentUsed;
            // Remove the steps to restrict access to the print alone
            NextStep = 0;
            PreviousStep = 0;
            prepareForPrint(DOSFileName, true, true, printComSent, this);
        } else if (printStatus == PrintStatus::ChangingFilament) { // Back from changing the filament
            materialsMenuStep.NextStep = &toolsMenuStep; // reset the route
            SERIAL_IMPL.println("Back to print (1)");
            card.startOrResumeFilePrinting();
        }
        else {
            SERIAL_IMPL.println("Idle home screen.");
            if(card.jobRecoverFileExists())
            {
                SERIAL_IMPL.println("Recovery file exists");
                printStatus = PrintStatus::PrintToResume;
                // Remove the steps to restrict access to the print alone
                NextStep = 0;
                PreviousStep = 0;
                RetroPreviousStep = cancelResumeDummyStep;
                RetroNextStep = beginResumeDummyStep;
            }
            else {
                NextStep = &sdMenuStep;
                PreviousStep = &toolsMenuStep;
                printStatus = PrintStatus::Idle;
            }
        } 
    }
}
void MainScreenStep::UnloadBegin(){
}
void MainScreenStep::UnloadComplete(){
    if (printStatus == PrintStatus::FileToPrint) {
        if (card.isPaused()) {// going to in-print utilities            
            printStatus = PrintStatus::ChangingFilament;

            if (!Host->Retro)
                Host->GotoPreviousStep();
        }
    }
}
void MainScreenStep::FocusChanged(StepAnimationStage stage){
    if (printStatus == PrintStatus::FileToPrint) {
        if (card.isPrinting() || card.isPaused())
         {
            if (stage == StepAnimationStage::GoingToOverLay){
                // Pause the print                
                enqueueComs({"M25", "G91", "G1 Z5 F1000", "G90", "G1 Y150 F3000"});
                SERIAL_IMPL.println("Pause the print");
                // Show the in-print utilities
                PreviousStep = &materialsMenuStep;
                materialsMenuStep.NextStep = this; 
            }
            else if (stage == StepAnimationStage::MainStep){
                // resume the print
                SERIAL_IMPL.println("Back to print (2)");
                enqueueComs({"M24"});
            }
        }
        else { // Ended. Return to idle
            NextStep = &sdMenuStep;
            PreviousStep = &toolsMenuStep;
            printStatus = PrintStatus::Idle;
            fileName = "";   
            SERIAL_IMPL.println("Turning to idle menu (2)");
        }
    }
}
bool MainScreenStep::CanJumpToMainMenu(){
    return printStatus == PrintStatus::Idle; // Can jump ony when idle
}