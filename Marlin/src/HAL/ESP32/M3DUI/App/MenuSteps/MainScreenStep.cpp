#include "MainScreenStep.h"
#include "..\Images.h"
#include "../../../../../module/printcounter.h"
#include "../../../../../module/motion.h"
#include "../../../../../module/temperature.h"
#include "../../../../../sd/cardreader.h"
#include "../../../../../feature/powerloss.h"
#include "../../../../../feature/babystep.h"
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include "../../Hardware/MarlinSpecific.h"
// #include "../../../LoadCell/LoadCell.h"
#include "..\MenuApp.h"
#include <Preferences.h>

extern int swapTools;

void printComSent(void* sender){
    MainScreenStep* This = (MainScreenStep*)sender;
    This->RetroNextStep = &inPrintMenuStep;
    This->NextActionString = "Pause";
    This->printStarted = true;
    This->printStatus = PrintStatus::Printing;
    This->TickPeriod = 3000;
}
void recoverComSent(void* sender){
    MainScreenStep* This = (MainScreenStep*)sender;
    This->RetroNextStep = 0;
    This->printStarted = true;
    This->printStatus = PrintStatus::Printing;
    This->TickPeriod = 3000;
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

    Icon = &img_Home;
    TickPeriod = 50;
}
MainScreenStep::~MainScreenStep(){
}
void MainScreenStep::prepareThisForPrint(){    
    materialAtStart = print_job_timer.getStats().filamentUsed;
    RetroPreviousStep = 0;
    RetroNextStep = 0;
    NextActionString = "";
    printStarted = false;            
    preparingRecovery = false;
    TickPeriod = 5000;
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
    if (card.isPrinting() && preparingRecovery) {
        // just started print after recovery prep.
        printStatus = PrintStatus::Printing;        
        RetroNextStep = &inPrintMenuStep;
        NextActionString = "Pause";
        preparingRecovery = false;
    }
    if (printStatus == PrintStatus::Idle && card.isPrinting()) { // Print sent via g code
        printComSent(this);
    }
    if (printStatus == PrintStatus::FileToPrint){
        if (!printStarted)
            printJobTick();
    }
    if (printStatus == PrintStatus::Printing){
        TickPeriod = 5000;
        if (!card.isPrinting() && !preparingRecovery) { // The print must have ended
            TickPeriod = 50;
            printStatus = PrintStatus::Idle;
            RetroNextStep = &retroMainMenuStep;
            RetroPreviousStep = 0;
            swapTools = false;
        }
    }
    // ProbeEnable = true;
    // if (millis() - lastReset > 10000){
    //     removeLoadCellOffset();
    //     lastReset = millis();
    // }
}
extern float readProbeAnalog();
extern bool ProbeEnable;
void MainScreenStep::Paint(BufferedDisplay* g) {
    
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    if (printStatus == PrintStatus::PrintToRecover){
        g->setFont(&FreeSans9pt7b);
        int _w, _h;
        int txtP = 10, txtW = 30;
        drawMultilineCenteredText(g, "Do you want to recover the print?", Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2, Host->appWidth(), 16, &_w, &_h);
        int16_t yesWid = 0, noWid = 0;
        centerString(g, "Yes", (Host->appWidth() / 2) - txtP - txtW / 2, retroTitleSectionHeight + Host->appHeight() / 2 + _h / 2 + 10, &yesWid);
        centerString(g, "No",  (Host->appWidth() / 2) + txtP + txtW / 2, retroTitleSectionHeight + Host->appHeight() / 2 + _h / 2 + 10, &noWid);
        int tSz = 12, p = 10;
        g->fillTriangle(
            (Host->appWidth() / 2) - txtW / 2 - txtP - yesWid / 2 - p + tSz / 2, Host->appHeight() / 2 + 49 + tSz / 2, 
            (Host->appWidth() / 2) - txtW / 2 - txtP - yesWid / 2 - p + tSz / 2, Host->appHeight() / 2 + 49 - tSz / 2, 
            (Host->appWidth() / 2) - txtW / 2 - txtP - yesWid / 2 - p - tSz / 2, Host->appHeight() / 2 + 49, 
            TextColor);
        g->fillTriangle(
            (Host->appWidth() / 2) + txtW / 2 + txtP + noWid / 2 + p - tSz / 2, Host->appHeight() / 2 + 49 + tSz / 2, 
            (Host->appWidth() / 2) + txtW / 2 + txtP + noWid / 2 + p - tSz / 2, Host->appHeight() / 2 + 49 - tSz / 2, 
            (Host->appWidth() / 2) + txtW / 2 + txtP + noWid / 2 + p + tSz / 2, Host->appHeight() / 2 + 49, 
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
            
            if (card.isPrinting()) {
                if (thermalManager.degHotendNear(active_extruder, thermalManager.degTargetHotend(active_extruder))) {
                    centerLeftString(g, "Printing", 2, titleHeight);
                }
                else
                    centerLeftString(g, "Preheating", 2, titleHeight);
            }
            else if (card.isPaused() || preparingRecovery) {
                if (preparingRecovery){
                    centerLeftString(g, "Preheating", 2, titleHeight);
                }
                else
                    centerLeftString(g, "Paused", 2, titleHeight);
            }
            else {// Must have ended
                centerLeftString(g, "All Done!", 2, titleHeight);
            }

            if (thermalManager.degHotendNear(active_extruder, thermalManager.degTargetHotend(active_extruder)))
                centerRightString(g, (String((print_job_timer.getStats().filamentUsed - materialAtStart) / 1000, 3) + String("m")).c_str(), Host->appWidth() - 2, titleHeight);
            else
                centerRightString(g, (String((thermalManager.degHotend(active_extruder) / (float)thermalManager.degTargetHotend(active_extruder)) * 100.0F, 0) + String("%")).c_str(), Host->appWidth() - 2, titleHeight);
            
            
            int pbh = 8;
            g->SetOpacity(50);
            float pcCommplete = card.percentDone();
            //pcCommplete = 24.4;
            g->drawRoundRect(1, Host->appTop() + (Host->appHeight() + 16) / 2 - 5, Host->appWidth() - 2, pbh, pbh / 2, TextColor);
            g->SetOpacity(100);
            g->fillRoundRect(1, Host->appTop() + (Host->appHeight() + 16) / 2 - 5, ((Host->appWidth() - 2) * pcCommplete) / 100, pbh, pbh / 2, TextColor);    
            g->setFont();
            centerString(g, (String(pcCommplete, 1) + String("%")).c_str(), Host->appWidth() / 2, Host->appTop() + (Host->appHeight() + 16) / 2 - 14);
            g->setFont();
            centerString(g, fileName.c_str(), Host->appWidth() / 2, Host->appTop() + (Host->appHeight() + 16) / 2 + pbh + 5);

            if (babystep.get_total_mm(Z_AXIS) != 0){
                String zStr = String("Z: ");
                if (babystep.get_total_mm(Z_AXIS) > 0.0)
                    zStr += "+"; // Force a sign
                zStr += String(babystep.get_total_mm(Z_AXIS), 2);
                centerLeftString(g, zStr.c_str(), 2, Host->appTop() + Host->appHeight() - 14);
            }
            if (feedrate_percentage != 100){
                String fStr = String("F: ");
                fStr += String(feedrate_percentage);
                fStr += "%";
                centerRightString(g, fStr.c_str(), Host->appWidth() - 2, Host->appTop() + Host->appHeight() - 14);
            }
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
    if (ProbeEnable) {
        int pHeight = readProbeAnalog();
        g->fillRect(g->width() - 3, g->height() - pHeight, 3, pHeight, ST7735_WHITE);
    }
}
void MainScreenStep::HandleKeyPress(Keys key){
    NeedsRedraw = true;
    if (Host->Retro) {        
        if (printStatus == PrintStatus::PrintToRecover) { 
            if (key == KEYPAD_LEFT) {
                SERIAL_IMPL.println("Resume print");
                prepareThisForPrint();
                preparingRecovery = true;
                
                bool _swapTools = false;
                Preferences prefs;
                prefs.begin("material");
                _swapTools = prefs.getBool("swap", false);
                swapTools = _swapTools;
                prefs.end();

                prepareMarlinForRecover(recoverComSent, this);            
            }
            else if (key == KEYPAD_RIGHT){                
                SERIAL_IMPL.println("Don't resume print");
                recovery.purge();
                printStatus = PrintStatus::Idle;
                fileName = "";
            }
        } 
        else if (printStatus == PrintStatus::Printing){
            HandleKeyHold(key);
        }
    }
    else {
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
        else if (printStatus == PrintStatus::PrintToRecover) { 
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

extern int16_t feedrate_percentage;
void MainScreenStep::HandleKeyHold(Keys key){
    if (key == Keys::KEYPAD_UP){
        SERIAL_IMPL.printf("Baby Step Up from: %f\n", babystep.get_total_mm(Z_AXIS));
        if (babystep.get_total_mm(Z_AXIS) < 1.0) // Limit to +1mm
            babystep.add_mm(Z_AXIS, 0.05);
    }
    else if (key == Keys::KEYPAD_DOWN){
        SERIAL_IMPL.printf("Baby Step Down from: %f\n", babystep.get_total_mm(Z_AXIS));
        if (babystep.get_total_mm(Z_AXIS) > -1.0) // Limit to -1mm            
            babystep.add_mm(Z_AXIS, -0.05);
    }
    else if (key == Keys::KEYPAD_LEFT || key == Keys::KEYPAD_RIGHT){
        int newSpeed = feedrate_percentage + ((key == Keys::KEYPAD_LEFT) ? -10:10);
        if (newSpeed < 20) newSpeed = 20; else if (newSpeed > 300) newSpeed = 300;
        feedrate_percentage = newSpeed; // directly control it. as done in marlinui.cpp
        SERIAL_IMPL.printf("Speed change: %d\n", newSpeed);
    }
}
void MainScreenStep::LoadComplete(){
    if (!Host->Retro){
        // Get colors
        prefs.begin("material");
        e0Color = AvailableColors[prefs.getInt("e1_c", 0)];
        e1Color = AvailableColors[prefs.getInt("e2_c", 1)];
        prefs.end();
    }

    if (Host->Retro){
        if (printStatus == PrintStatus::FileToPrint) { // This must be set by the file selection menus.
            SERIAL_IMPL.printf("Home screen with print file: %s, %s\n", fileName.c_str(), DOSFileName.c_str());
            
            prepareThisForPrint();
            prepareMarlinForPrint(DOSFileName, true, true, printComSent, this);            
        }
        else if (printStatus == PrintStatus::Printing) {
            if (card.isPaused()) { // came back from the menu with back button
                card.startOrResumeFilePrinting();                
                TickPeriod = 5000;
            }
        }
        else if (printStatus == PrintStatus::PrintToRecover){
            // Pretty much the same as other states.
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
            prepareMarlinForPrint(DOSFileName, true, true, printComSent, this);
        } else if (printStatus == PrintStatus::ChangingFilament) { // Back from changing the filament
            materialsMenuStep.NextStep = &toolsMenuStep; // reset the route
            SERIAL_IMPL.println("Back to print (1)");
            card.startOrResumeFilePrinting();            
            TickPeriod = 5000;
        } else {
            SERIAL_IMPL.println("Idle home screen.");
            if(card.jobRecoverFileExists())
            {
                SERIAL_IMPL.println("Recovery file exists");
                printStatus = PrintStatus::PrintToRecover;
                TickPeriod = 50;
                // Remove the steps to restrict access to the print alone
                NextStep = 0;
                PreviousStep = 0;
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
            TickPeriod = 50;

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