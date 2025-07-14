#include "BedLevelStep.h"
#include "..\..\Hardware\MarlinSpecific.h"
#include "..\Bitmaps.h"
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include "..\MenuApp.h"

BedLevelStep::BedLevelStep(MenuHost* host):MenuStep(host)
{
    TextColor = ST7735_WHITE;
    Icon = &bmp_Tools;
    TickPeriod = 50;
}

BedLevelStep::~BedLevelStep()
{
}

void BedLevelStep::Tick()
{
    NeedsRedraw = true;
    if (millis() - levelingDoneSince > 3000 && levelingDoneSince != 0){
        TickPeriod = 0; // remove the tick
        Host->GotoNextStep();
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
    levelingDoneSince = 0;
    TickPeriod = 50;
}

void BedLevelStep::UnloadBegin(){
    SERIAL_IMPL.println("End ABL");
    AbortABL();

    writeTemp(0, 0);
    writeTemp(1, 0);
}

void BedLevelStep::notifyLevelingDone(){
    if (levelingDoneSince == 0){
        TickPeriod = 100;
        levelingDoneSince = millis();
        writeTemp(0, 0);
        writeTemp(1, 0);
    }
}
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
    if ((readTemp(0) < preHeatTemp || readTemp(1) < preHeatTemp) && !donePreHeating){        
        g->setFont(&FreeSans9pt7b);
        centerString(g, "Heating up...", g->width() / 2, g->height() / 2 - 10);
        g->setFont();
        String tempStatus = String("(") + String(readTemp(0) * 0.5F + readTemp(1) * 0.5F, 0) + "/" + String(preHeatTemp, 0) + String(")");
        centerString(g, tempStatus.c_str(), g->width() / 2, g->height() / 2 + 10);
    }
    else {
        donePreHeating = true; // latch preheat check
        if (!G29Sent){
            G29Sent = true; 
            StartABL(); // resets the flags
        }
        if (checkABLComplete() && !checkABLFailed()){
            g->setFont(&FreeSans9pt7b);
            centerString(g, "Leveling", g->width() / 2, g->height() / 2 - 10);
            centerString(g, "Successful", g->width() / 2, g->height() / 2 + 10);
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
            int xStep = (PrintPositionStep::bedWidth - sideMargin * 2) / 2;
            int yStep = (PrintPositionStep::bedHeight - sideMargin * 2) / 2;
            for (int yi = 0; yi < 3; yi++){
                for (int xi = yi % 2 == 0 ? 0 : 2; yi % 2 == 0 ? (xi < 3) : (xi >= 0); xi += yi % 2 == 0 ? 1 : -1) {
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
                centerString(g, "Leveling", g->width() / 2, g->height() / 2 - 10);
                centerString(g, "Failed", g->width() / 2, g->height() / 2 + 10);
                notifyLevelingDone();
            }
            else{           
                g->setFont();     
                centerString(g, "Leveling build plate", g->width() / 2, g->height() / 2 - 8);
                centerString(g, "Please wait...", g->width() / 2, g->height() / 2 + 8);
            }
        }
    }
    g->setFont();
}

void BedLevelStep::HandleKeyUp(Keys key) {
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