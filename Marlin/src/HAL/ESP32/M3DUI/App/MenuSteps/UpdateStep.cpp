#include "UpdateStep.h"
#include "..\Images.h"
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
// #include "../../../LoadCell/LoadCell.h"
#include "..\MenuApp.h"
#include "..\..\..\OTA\OTA.h"
#include "..\..\..\..\..\sd\cardreader.h"
#include <Update.h>

bool inSDUpdate = false;
bool requestSDOtaUpdate = false;
bool sdOtaLoopRunning = false;
TaskHandle_t sdOtaLoopHandle;
void SDCardOTALoop(void*);
void _TrySDOTAUpdate_();

// void TrySDCardUpdate() {
//     if (!sdOtaLoopRunning){ 
//         xTaskCreate(SDCardOTALoop, "sd_ota", 4096, 0, 1, &sdOtaLoopHandle);
//     }
//     if (inSDUpdate){ // dont put a double request
//         return;
//     }
//     requestSDOtaUpdate = true; // put the request
// }

// void SDCardOTALoop(void*){    
//     sdOtaLoopRunning = true;       
//     while(1){ // Can't exit a Task
//         while(!requestSDOtaUpdate) // wait for request
//             delay(100);
//         requestSDOtaUpdate = false; // remove the flag for request
//         inSDUpdate = true;
//         _TrySDOTAUpdate_();
//         inSDUpdate = false;
//     }
// }

// void _TrySDOTAUpdate_() {
//     SERIAL_IMPL.println("TrySDOTAUpdate()");

//     if (menuHost.CurrentStep && menuHost.CurrentStep != &updateStep){
//         menuHost.CurrentStep->NextStep = &updateStep;
//         menuHost.GotoNextStep(); 
//     }

//     updateStep.NotifyOTAProgressChange(0);
//     updateStep.NotifyOTAProgressChange("Checking SD Card...");

//     SdVolume sdVolume;
//     SdFile rootDir;
//     SdFile firmwareFile;
    
//     if (!sdVolume.init(card.diskIODriver())) {
//         updateStep.NotifyOTAProgressChange("SD Vol Init Failed");
//         return;
//     }
//     if (!rootDir.openRoot(&sdVolume)) {
//         updateStep.NotifyOTAProgressChange("SD Root Init Failed");
//         return;
//     }

//     if (!firmwareFile.open(&rootDir, "firmware.bin", O_READ) && !firmwareFile.open(&rootDir, "FIRMWARE.BIN", O_READ)) {
//         updateStep.NotifyOTAProgressChange("Error opening file");
//         return;
//     }

//     long contentLength = firmwareFile.fileSize();
//     SERIAL_IMPL.printf("SD Card firmware.bin size: %li\n", contentLength);

//     if (contentLength > 0) {
//         bool canBegin = Update.begin(contentLength);
//         if (canBegin) {
//             updateStep.NotifyOTAProgressChange("Updating from SD...");
            
//             long totalToWrite = contentLength;
//             long perPacket = 8192;
//             long written = 0;

//             SERIAL_IMPL.printf("Update From SD: Total Size: %d\n", totalToWrite);
//             uint8_t* buffer = new uint8_t[perPacket];
//             while (written < totalToWrite) {                
//                 int thisPacket = perPacket;
//                 if ((totalToWrite - written) < perPacket)
//                     thisPacket = (totalToWrite - written);
                
//                 int bytesRead = firmwareFile.read(buffer, thisPacket);
//                 if (bytesRead <= 0) {
//                     SERIAL_IMPL.printf("SD Read Error bytesRead=%d\n", bytesRead);
//                     delay(10);
//                     continue; // EOF or error
//                 }
//                 Update.write(buffer, bytesRead);   
//                 updateStep.NotifyOTAProgressChange((float)(written * 99) / (float)totalToWrite + 1);
//                 written += bytesRead;
//                 SERIAL_IMPL.printf("Writting %d/%d\n", written, totalToWrite);
//                 delay(10);
//             }
//             delete[] buffer;

//             SERIAL_IMPL.printf("All Done. Closing file\n");
//             firmwareFile.close(); // Close before renaming

//             if (written == contentLength) {
//                 SERIAL_IMPL.printf("SD Written : %li successfully\n", written);
//             } else {
//                 updateStep.NotifyOTAProgressChange("SD Update failed [1]");
//                 return;
//             }

//             if (Update.end()) {
//                 updateStep.NotifyOTAProgressChange(100);
//                 if (Update.isFinished()) {
//                     SERIAL_IMPL.println("SD Update successfully completed.");
//                     updateStep.NotifyOTAProgressChange("Moving backup file");
//                     SdBaseFile::remove(&rootDir, "firmware.bkp");
//                     SdFile fileToRename;
//                     if (fileToRename.open(&rootDir, "firmware.bin", O_READ | O_WRITE) || fileToRename.open(&rootDir, "FIRMWARE.BIN", O_READ | O_WRITE)) {
//                         fileToRename.rename(&rootDir, "firmware.bkp");
//                         fileToRename.close();
//                     }

//                     for (int i = 5; i > 0; i--) {
//                         SERIAL_IMPL.printf("Rebooting in %d\n", i);
//                         delay(1000);                        
//                         updateStep.NotifyOTAProgressChange(String("Rebooting in (") + String(i) + String(")"));
//                     }
//                     delay(10);
//                     SERIAL_IMPL.println("Rebooting.");
//                     delay(10);
//                     ESP.restart();
//                 } else {
//                     SERIAL_IMPL.printf("SD Update failed [2]\n");
//                     updateStep.NotifyOTAProgressChange("SD Update failed [2]");
//                 }
//             } else {
//                 SERIAL_IMPL.printf("SD Update failed [3]\n");
//                 updateStep.NotifyOTAProgressChange("SD Update failed [3]");
//                 SERIAL_IMPL.printf("SD Error Occurred. Error #: %d\n", Update.getError());
//             }
//         } else {
//             SERIAL_IMPL.printf("SD Update failed [4]\n");
//             updateStep.NotifyOTAProgressChange("SD Update failed [4]");
//             firmwareFile.close();
//         }
//     } else {
//         SERIAL_IMPL.printf("SD Update failed [5]\n");
//         updateStep.NotifyOTAProgressChange("SD Update failed [5]");
//         firmwareFile.close();
//     }
// }

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
    // bool sdOtaTried = false;
    // if (!card.isMounted()) {
    //     card.mount();
    // }
    
    // if (card.isMounted()) {
    //     SdFile root = card.getroot();
    //     SdFile testFile;
    //     // Firmware file uppercase since FAT uses 8.3 internally (or might be DOS format)
    //     // fileExists can act directly, but open test is solid.
    //     if (testFile.open(&root, "firmware.bin", O_READ)) {
    //         sdOtaTried = true;
    //         testFile.close();
    //     } else if (testFile.open(&root, "FIRMWARE.BIN", O_READ)) {
    //         sdOtaTried = true;
    //         testFile.close();
    //     }
    // }

    // if (sdOtaTried) {
    //     TrySDCardUpdate();
    // } else {
        TryOTAUpdate();
    // }

    retroPreviousStepBkp = RetroPreviousStep;
    RetroPreviousStep = 0;
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
    retroPreviousStepBkp = retroPreviousStepBkp;
}