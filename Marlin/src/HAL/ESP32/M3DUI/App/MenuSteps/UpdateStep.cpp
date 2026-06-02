#include "UpdateStep.h"
#include "..\MenuApp.h"
#include "..\Images.h"
#include "..\..\..\..\..\sd\cardreader.h"
#include <Update.h>
#include "..\..\..\OTA\OTA.h"

UpdateStep::UpdateStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkRed;
    BackColor = DarkRed;
    TextColor = ST7735_WHITE;            
    Title = "Firmware Update";
    RetroIcon = &img_RetroM3D;  
    PreviousStep = 0;
    RetroNextStep = 0;
    NoActivityTimeout = 0;
    JumpToStepOnNoActivity = &settingsStep;

    Icon = &img_Home;
}
UpdateStep::~UpdateStep(){
    if (updateBuffer)
        delete updateBuffer;
}

void UpdateStep::Tick() {
    NeedsRedraw = true;
    if (UpdateState == SdUpdateState::BeginSd){
        UpdateState = SdUpdateState::BeginNetwork; // skip to network in case this doesn't yield good
        StatusMessage = "Starting SD card";
        SERIAL_IMPL.println("Starting SD");
        if (!card.isMounted()){
            card.mount();
        }
        bool hasSDCard = card.isMounted();

        if (!hasSDCard){            
            StatusMessage = "SD card not found";
            SERIAL_IMPL.println("SD card not found");
            return;
        }

        StatusMessage = "SD update failed";
        SERIAL_IMPL.println("SD card FOUND!");
        if (card.fileExists(UpdateFileName)){
            SERIAL_IMPL.println("Firmware file found!");   
            card.openFileRead(UpdateFileName);
            if (card.isFileOpen()){                
                if(Update.begin(card.getFileSize())) {
                    totalBytesReadForSd = 0; 
                    TickPeriod = 100;        
                    StatusMessage = "Updating from SD";
                    beginSdWaitStaertedAt = millis();
                    UpdateState = SdUpdateState::WatingToBeginSd;
                    SERIAL_IMPL.println("File open for update.");                
                    updateBuffer = new uint8_t[SdUpdateBufferSize];                
                }
                else {
                    SERIAL_IMPL.println("Update could not begin");
                }
            }
            else{
                SERIAL_IMPL.println("File could not be opened.");
            }
        }
        else{
            SERIAL_IMPL.println("No firmware file found");
        }
    } 
    else if (UpdateState == SdUpdateState::WatingToBeginSd){        
        if (millis() - beginSdWaitStaertedAt < 5000){
            StatusMessage = String("Updating in ") + String((5000 - (millis() - beginSdWaitStaertedAt)) / 1000) + String("s");
        }
        else {
            StatusMessage = "Updating from SD Card";
            TickPeriod = 1;
            UpdateState = SdUpdateState::UpdatingFromSd;
        }
    }
    else if (UpdateState == SdUpdateState::UpdatingFromSd){
        int tRead = card.read(updateBuffer, SdUpdateBufferSize);
        if (tRead > 0) {
            totalBytesReadForSd += tRead;
            // write update data
            Update.write(updateBuffer, tRead);
            Progress = (float)totalBytesReadForSd / (float)card.getFileSize() * 100.0F;
        }
        SERIAL_IMPL.printf("Read %d/%d\n", totalBytesReadForSd, card.getFileSize());
        if (tRead == 0){
            delete updateBuffer;
            updateBuffer = 0;
            SERIAL_IMPL.println("EOF for Update file");

            if(Update.end()){
                SERIAL_IMPL.println("Update Finished!");
                Progress = 100;
                StatusMessage = "Complete";
            }
            card.closefile();
            UpdateState = SdUpdateState::RestartingAfterSd;
            updateFinishedAt = millis();
            TickPeriod = 1000;
        }
    }
    else if (UpdateState == SdUpdateState::RestartingAfterSd){
        if (millis() - updateFinishedAt < 5000){
            SERIAL_IMPL.printf("Restarting in %d\n", (5000 - (millis() - updateFinishedAt)) / 1000);
            StatusMessage = String("Restarting in ") + String((5000 - (millis() - updateFinishedAt)) / 1000) + String("s");
        }
        else{
            SERIAL_IMPL.println("Restarting");
            ESP.restart();
            UpdateState = SdUpdateState::None;            
        }
    }
    else if (UpdateState == SdUpdateState::BeginNetwork){
        UpdateState = SdUpdateState::None;
        SERIAL_IMPL.println("Switching to Network Update.");
        TryOTAUpdate();
    }
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
    TickPeriod = 1000;
    UpdateState = SdUpdateState::BeginSd;
    StatusMessage = "Checking SD card";
    NeedsRedraw = true;   
    NoActivityTimeout = 0;
}

void UpdateStep::NotifyOTAProgressChange(String str){
    StatusMessage = str; NeedsRedraw = true;
}
void UpdateStep::NotifyOTAProgressChange(float progress){
    Progress = progress;
    NeedsRedraw = true;
}
void UpdateStep::NotifyOTAComplete(){
    Progress = 100;
    NeedsRedraw = "";
}
void UpdateStep::NotifyOTAFailed(){
    //retroPreviousStepBkp = retroPreviousStepBkp;
}
void UpdateStep::HandleKeyPress(Keys key){
    if (UpdateState == SdUpdateState::WatingToBeginSd){
        UpdateState =  SdUpdateState::None;
        StatusMessage = "Cancelled";
        card.closefile();
        delete updateBuffer;
        updateBuffer = 0;
        Update.abort();
        TickPeriod = 100;
        NeedsRedraw = true;        
        NoActivityTimeout = 2000;
    }
}