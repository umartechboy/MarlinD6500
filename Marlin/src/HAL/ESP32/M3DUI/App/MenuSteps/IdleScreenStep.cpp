#include "IdleScreenStep.h"
#include "..\Bitmaps.h"
#include "../../../../../module/printcounter.h"
#include "../../../../../sd/cardreader.h"
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include "../../Hardware/MarlinSpecific.h"
#include "..\MenuApp.h"

IdleScreenStep::IdleScreenStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkRed;
    BackColor = DarkRed;
    TextColor = ST7735_WHITE;            
    Icon = &bmp_Home;
    TickPeriod = 50;
}
void IdleScreenStep::Tick() {
    NeedsRedraw = true;
}
void IdleScreenStep::Paint(BufferedDisplay* g) {
    
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    if (printStatus == PrintStatus::PrintToResume){
        g->setFont(&FreeSans9pt7b);
        centerString(g, "Do you want", g->width() / 2, g->height() / 2 - 50);
        centerString(g, "to resume", g->width() / 2, g->height() / 2 - 30);
        centerString(g, "the print?", g->width() / 2, g->height() / 2 - 10);
        g->setFont(&FreeSans12pt7b);
        centerString(g, "Yes", g->width() / 4, g->height() / 2 + 22);
        centerString(g, "No", (g->width() * 3) / 4, g->height() / 2 + 22);
        int tSz = 12;
        g->fillTriangle(
            g->width() / 4 - tSz / 2, g->height() / 2 + 49, 
            g->width() / 4 + tSz / 2, g->height() / 2 + 49 - tSz / 2, 
            g->width() / 4 + tSz / 2, g->height() / 2 + 49 + tSz / 2, 
            TextColor);
        g->fillTriangle(
            (g->width() * 3) / 4 + tSz / 2, g->height() / 2 + 49, 
            (g->width() * 3) / 4 - tSz / 2, g->height() / 2 + 49 - tSz / 2, 
            (g->width() * 3) / 4 - tSz / 2, g->height() / 2 + 49 + tSz / 2, 
            TextColor);
    }
    else {
        if(printStatus == PrintStatus::Idle){
            g->setFont(&FreeSans12pt7b);
            centerString(g, "Ready!", g->width() / 2, g->height() / 2);
        }
        else{        
            if (card.isPrinting())
                centerLeftString(g, "Printing", 2, 10);
            else if (card.isPaused())
                centerLeftString(g, "Paused", 2, 10);
            else {// Must have ended
                centerLeftString(g, "All Done!", 2, 10);
            }
            centerRightString(g, (String((print_job_timer.getStats().filamentUsed - materialAtStart) / 1000, 3) + String("m")).c_str(), g->width() - 2, 10);
            
            int pbh = 8;
            g->SetOpacity(50);
            float pcCommplete = card.percentDone();
            //pcCommplete = 24.4;
            g->drawRoundRect(1, g->height() / 2 - 5, g->width() - 2, pbh, pbh / 2, TextColor);
            g->SetOpacity(100);
            g->fillRoundRect(1, g->height() / 2 - 5, ((g->width() - 2) * pcCommplete) / 100, pbh, pbh / 2, TextColor);    
            g->setFont();
            centerString(g, (String(pcCommplete, 1) + String("%")).c_str(), g->width() / 2, g->height() / 2 - 14);
            g->setFont();
            centerString(g, fileName.substring(1, fileName.length() - 6).c_str(), g->width() / 2, g->height() / 2 + pbh + 5);
        }
        uint8_t opBkp = g->GetOpacity();
        uint8_t lineMargin = 10;
        uint8_t tempSectionHeight = 20;
        g->SetOpacity(10);
        for (int i =0; i < 5; i++)
            g->drawLine(lineMargin + i * 2, g->height() - tempSectionHeight, g->width() - lineMargin - i * 2, g->height() - tempSectionHeight, TextColor);
        g->SetOpacity(opBkp);
        g->drawLine(g->width() / 2, g->height() - tempSectionHeight, g->width() / 2, g->height(), g->readPixel(g->width() / 2, g->height() - tempSectionHeight));
        
        g->setFont();
        String temp1 = String(readTemp1(), 1) + " C";
        String temp2 = String(readTemp2(), 1) + " C";
        int16_t tw, th;
        centerString(g, temp1.c_str(), g->width() / 4, g->height() + 1 - tempSectionHeight / 2, &tw, &th);
        g->drawCircle(g->width() / 4 + tw / 2 - 9, g->height() + 1 - tempSectionHeight + 5, 1, TextColor);
        centerString(g, temp2.c_str(), (3 * g->width()) / 4, g->height() + 1 - tempSectionHeight / 2, &tw, &th);
        g->drawCircle((3 * g->width()) / 4 + tw / 2 - 9, g->height() + 1 - tempSectionHeight + 5, 1, TextColor);
        g->fillRect(0, g->height() - 2, g->width() / 2, 2, e0Color);
        g->fillRect(g->width() / 2 + 1, g->height() - 2, g->width() / 2, 2, e1Color);
    }
}

void IdleScreenStep::LoadComplete(){
    // Get colors
    prefs.begin("material");
    e0Color = AvailableColors[prefs.getInt("e1_c", 0)];
    e1Color = AvailableColors[prefs.getInt("e2_c", 1)];
    prefs.end();

    // Get the status and set the mode
    if (printStatus == PrintStatus::FileToPrint) { // This must be set by the file selection menus.
        SERIAL_IMPL.printf("Home screen with print file: %s\n", fileName.c_str());
        //card.openAndPrintFile(fileName.c_str());
        materialAtStart = print_job_timer.getStats().filamentUsed;
        // Remove the steps to restrict access to the print alone
        NextStep = 0;
        PreviousStep = 0;
    } else if (printStatus == PrintStatus::ChangingFilament) { // Back from changing the filament
        materialsMenuStep.NextStep = &toolsMenuStep; // reset the route
        SERIAL_IMPL.println("Back to print");
        //card.startOrResumeFilePrinting();
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
        }
        else {
            NextStep = &sdMenuStep;
            PreviousStep = &toolsMenuStep;
            printStatus = PrintStatus::Idle;
        }
    } 
}

void IdleScreenStep::HandleKeyUp(Keys key){
    if (printStatus == PrintStatus::Idle){
        if (key == KEYPAD_LEFT){
            SERIAL_IMPL.println("Go to previous from Idle");
            Host->GotoPreviousStep();
        }
        else if (key == KEYPAD_RIGHT){
            SERIAL_IMPL.println("Go to next from Idle");
            Host->GotoNextStep();
        }
    }
    else if (printStatus == PrintStatus::PrintToResume) { 
        if (key == KEYPAD_LEFT) {
            SERIAL_IMPL.println("Resume print");
            printStatus = PrintStatus::FileToPrint;
            // Trigger the resume too
            //card.startOrResumeFilePrinting();
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
void IdleScreenStep::FocusChanged(StepAnimationStage stage){
    if (printStatus == PrintStatus::FileToPrint) {
        if (card.isPrinting() || card.isPaused())
         {
            if (stage == StepAnimationStage::GoingToOverLay){
                // Pause the print
                //card.pauseSDPrint();
                SERIAL_IMPL.println("Pause the print");
                // Show the in-print utilities
                PreviousStep = &materialsMenuStep;
                materialsMenuStep.NextStep = this; 
                // this will disturbe the logic in idle mode. Make the options screen reset the next step for material
                printStatus = PrintStatus::ChangingFilament;
                Host->GotoPreviousStep();
            }
            else if (stage == StepAnimationStage::MainStep){
                // resume the print
                //card.startOrResumeFilePrinting();
                SERIAL_IMPL.println("Resume the print");
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