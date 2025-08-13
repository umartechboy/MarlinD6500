#include "UpdateStep.h"
#include "..\Images.h"
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
// #include "../../../LoadCell/LoadCell.h"
#include "..\MenuApp.h"
#include "..\..\..\OTA\OTA.h"

UpdateStep::UpdateStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkRed;
    BackColor = DarkRed;
    TextColor = ST7735_WHITE;            
    Title = "Firmware Update";
    RetroIcon = &img_RetroM3D;  
    PreviousStep = 0;
    RetroNextStep = 0;

    Icon = &img_Home;
    TickPeriod = 50;
}
UpdateStep::~UpdateStep(){
}
// long lastReset = 0;
// extern bool ProbeEnable; 
// extern float lastAnalogReturn;
// extern void removeLoadCellOffset();
void UpdateStep::Tick() {
    
}
void UpdateStep::Paint(BufferedDisplay* g) {
    
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    int titleHeight = 10;
    if (Host->Retro)
        titleHeight = retroTitleSectionHeight + 14;
    
    int pbh = 8;
    g->SetOpacity(50);
    float pcCommplete = Progress;
    //pcCommplete = 24.4;
    g->drawRoundRect(1, Host->appTop() + Host->appHeight() / 2 - 5, Host->appWidth() - 2, pbh, pbh / 2, TextColor);
    g->SetOpacity(100);
    g->fillRoundRect(1, Host->appTop() + Host->appHeight() / 2 - 5, ((Host->appWidth() - 2) * pcCommplete) / 100, pbh, pbh / 2, TextColor);    
    g->setFont();
    centerString(g, (String(pcCommplete, 1) + String("%")).c_str(), Host->appWidth() / 2, Host->appTop() + Host->appHeight() / 2 - 14);
    g->setFont();
    centerString(g, StatusMessage.c_str(), Host->appWidth() / 2, Host->appTop() + Host->appHeight() / 2 + pbh + 5);

}

void UpdateStep::LoadComplete(){
    TryOTAUpdate();
    retroPreviousStepBkp = RetroPreviousStep;
    RetroPreviousStep = 0;
}

void UpdateStep::NotifyOTAProgressChange(String str){
    StatusMessage = str; NeedsRedraw = true;
}
void UpdateStep::NotifyOTAProgressChange(int progress){
    Progress = progress;
    NeedsRedraw = true;
}
void UpdateStep::NotifyOTAComplete(){
    Progress = 100;
    NeedsRedraw = "";
}
void UpdateStep::NotifyOTAFailed(){
    retroPreviousStepBkp = retroPreviousStepBkp;
}