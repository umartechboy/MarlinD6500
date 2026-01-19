#include "BedLevelStep.h"
#include "..\..\Hardware\MarlinSpecific.h"
#include "..\Images.h"
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include "..\MenuApp.h"

BedLevelStep::BedLevelStep(MenuHost* host):MenuStep(host)
{
    TextColor = ST7735_WHITE;
    Icon = &img_Tools;
    NextStep = &toolsMenuStep;
    Title = "Auto bed leveling";
}

BedLevelStep::~BedLevelStep()
{
}

void BedLevelStep::Tick()
{
    NeedsRedraw = true;    
    if ((readTemp(0) >= preHeatTemp && readTemp(1) >= preHeatTemp)){
        if (!donePreHeating){
            // just done
            // Send the cleaning codes
            // find an empty slot
            String probeCom = String("G30 X") + String(pxStart) + String(" Y") + String(pyStart);
            enqueueComs({"G91", "M106", "T0", "G1 Z3 X20", probeCom}); // We are gonna need the fan too
        }
        donePreHeating = true;
    }
    if (donePreHeating && levelingDoneSince == 0){ // We are either wiping or leveling
        if(cleaningWipeDueIndex < wipeAreaHeight / wipeRungHeight){
            if (cleaningWipeSentIndex != cleaningWipeDueIndex) {
                // Send now                
                String wipeCom0 = String("G1 Z0.2");
                String wipeCom1 = String("G1 X") + String(wipeAreaWidth) + String(" E-9 F300");
                String wipeCom2 = String("G1 E6 Z2 F2000"); // force ooze out too
                String wipeCom3 = String("G1 X") + String(-wipeAreaWidth) + String(" Y") + String(wipeRungHeight);// Force ooze out
                String wipeCom4 = String("G30"); // Find new Z
                if (cleaningWipeDueIndex + 1 >= wipeAreaHeight / wipeRungHeight) // next iteration won't run
                    wipeCom4 = "";
                enqueueComs({wipeCom0, wipeCom1, wipeCom2, wipeCom3, wipeCom4});
                cleaningWipeSentIndex = cleaningWipeDueIndex;
            }
            else {
                // See if we can send more
                if (!hasComsQueued())
                    cleaningWipeDueIndex++;
            }
            // check if cleaning coms are done
        }
        else {
            if (!G29Sent) {
                // Send the G29
                PreviousStep = 0;
                G29Sent = true;
                enqueueComs({"M107", "M206 Z0", "M420 S0"});
                StartABL();
                writeTemp(0, 0);
                writeTemp(1, 0);
            }
            // else{
            // This keeps on triggering as long as levelingDoneSince is not set
            // }

        }        
    }
    else if (millis() - levelingDoneSince > 5000 && levelingDoneSince != 0 && !checkABLFailed()){ // auto procede in case of success
        SERIAL_IMPL.println("Wait on completion is up. Go back to main menu");
        TickPeriod = 0; // remove the tick
        this->RetroPreviousStep = &retroMainMenuStep;
        this->PreviousStep = &bedLevelStep;
        Host->GotoPreviousStep();
    }
}

void BedLevelStep::LoadComplete(){
    SERIAL_IMPL.printf("Begin Bed Level\n");
    // Lift and heat up
    writeTemp(0, preHeatTemp);
    writeTemp(1, preHeatTemp);
    enqueueComs({"G28"});
    enqueueComs({"G91", "G1 Z5", "G90"});
    G29Sent = false;
    donePreHeating = false;
    cleaningWipeDueIndex = 0;
    cleaningWipeSentIndex = -1;
    levelingDoneSince = 0;
    TickPeriod = 50;
}

void BedLevelStep::UnloadBegin(){
    SERIAL_IMPL.println("End ABL");
    AbortABL();
    if (donePreHeating && !checkABLComplete()) {
        // Nozzle wipe
        enqueueComs("G1 Z-3"); // lift the head in case of nozzle wipe
    }
    enqueueComs({"G90"});

    writeTemp(0, 0);
    writeTemp(1, 0);
}

void BedLevelStep::notifyLevelingDone(){
    if (levelingDoneSince == 0){
        SERIAL_IMPL.println("Leveling done");
        TickPeriod = 100;
        levelingDoneSince = millis();
        writeTemp(0, 0);
        writeTemp(1, 0);
        enqueueComs({"M106 S0", "G1 Y160 X100 Z5 F2000", "M420 S1"}); // Move out of the way
    }
}
extern float readProbeAnalog();
void BedLevelStep::Paint(BufferedDisplay* g){            
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    Color red = Color(DarkRed);
    int a = (readTemp(0) / preHeatTemp) * 255.0F;
    if (a > 255) a = 255;
    else if (a < 0) a = 0;
    red.a(a);
    red.BlendOn(DarkOceanBlue);
    BackColor = red;
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    if (!G29Sent){        
        g->setFont(&FreeSans9pt7b);
        int _h = 0, _w;
        drawMultilineCenteredText(g, !donePreHeating?"Heating up...":"Cleaning Nozzle...", Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2 - 10, Host->appWidth(), 20, &_w, &_h);
        g->setFont();
        String tempStatus = String("(") + String(readTemp(0) * 0.5F + readTemp(1) * 0.5F, 0) + "/" + String(preHeatTemp, 0) + String(")");
        centerString(g, tempStatus.c_str(), Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2 + _h / 2 + 10);
    }
    else {
        if (checkABLComplete() && !checkABLFailed()){
            g->setFont(&FreeSans9pt7b);
            centerString(g, "Leveling", Host->appWidth() / 2,  retroTitleSectionHeight + Host->appHeight() / 2 - 10);
            centerString(g, "Successful", Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2 + 10);
            notifyLevelingDone();
        }
        else { // Going on or done with failure
            // Draw the bed
            g->fillScreen(BackColor);
            g->setFont();
            g->setTextColor(TextColor);
            g->drawRect(PrintPositionStep::transformX(0), PrintPositionStep::transformY(PrintPositionStep::bedHeight), PrintPositionStep::scaleX(PrintPositionStep::bedWidth), PrintPositionStep::scaleY(PrintPositionStep::bedHeight), ST7735_WHITE);

            // Draw the probes
            int thisProbeInd = 0;
            int sideMargin = 20;
            int xStep = (PrintPositionStep::bedWidth - sideMargin * 2) / (GRID_MAX_POINTS_X - 1);
            int yStep = (PrintPositionStep::bedHeight - sideMargin * 2) / (GRID_MAX_POINTS_Y - 1);
            for (int yi = 0; yi < GRID_MAX_POINTS_Y; yi++){
                for (int xi = yi % 2 /* Even/Odd */ == 0 ? 0 : (GRID_MAX_POINTS_X - 1); yi % 2 == 0 ? (xi < GRID_MAX_POINTS_X) : (xi >= 0); xi += yi % 2 == 0 ? 1 : -1) {
                    int x = PrintPositionStep::transformX(sideMargin + xi * xStep);
                    int y = PrintPositionStep::transformY(sideMargin + yi * yStep);
                    if (thisProbeInd < getABLIndex()) {// This probe must have succeeded
                        g->fillCircle(x, y, 3, ST77XX_GREEN);
                    }
                    else if (thisProbeInd > getABLIndex()) {// This probe is undefined for sure
                        g->fillCircle(x, y, 1, ST77XX_YELLOW);
                    }
                    else { // we now need to see if this probe is being done or has failed
                        if (!checkABLFailed()) { // Going on
                            int r = millis() % 2000;
                            if (r < 300){ // 300 ms
                                float p =(float)r / 300.0F;
                                g->SetOpacity(p * 100);
                                g->fillCircle(x, y, MixFloats(30, 0, p, MixType::EaseOut), ST77XX_YELLOW);
                                g->SetOpacity(100);
                            }
                        }
                        else
                        {
                            g->fillCircle(x, y, 3, ST77XX_RED);
                        }
                    }
                    thisProbeInd++;
                }
            }
            if (checkABLFailed()){
                g->setFont(&FreeSans9pt7b);
                centerString(g, "Leveling", Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2 - 10);
                centerString(g, "Failed", Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2 + 10);
                notifyLevelingDone();
            }
            else{           
                g->setFont();     
                centerString(g, "Leveling build plate", Host->appWidth() / 2, Host->appHeight() / 2 - 8);
                centerString(g, "Please wait...", Host->appWidth() / 2, Host->appHeight() / 2 + 8);
            }
        }
    }
    int pHeight = readProbeAnalog();
    g->fillRect(g->width() - 3, g->height() - pHeight, 3, pHeight, ST7735_WHITE);
    g->setFont();
}

MenuStep* BedLevelStep::GetPreviousStep(bool returnEvenIfDummy){
    if (!donePreHeating){
        // No need to M104 because it will be called in step unload.
        return MenuStep::GetPreviousStep(returnEvenIfDummy);
    }
    else{
        if (levelingDoneSince != 0){
            return &retroMainMenuStep;
        }
        return 0;
    }
}

void BedLevelStep::HandleKeyPress(Keys key) {
    //Host->PushNotification("Bed leveling in process. Please wait...");
}
void BedLevelStep::FocusChanged(StepAnimationStage stage){
    if (stage == StepAnimationStage::InOverlay){
        // pause
        pauseABL();
    }
    else if (stage == StepAnimationStage::MainStep){
        // pause
        if (checkABLPaused()){
            resumeABL();
        }
    }
}