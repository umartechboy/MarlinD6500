#ifndef UPDATE_MENU_STEP
#define UPDATE_MENU_STEP

#include "..\..\Components\M3DUI.h"
#define UpdateFileName "FIRMWARE.BIN"
#define SdUpdateBufferSize (1024 * 16)

enum SdUpdateState : uint8_t{
    None = 0,
    BeginSd,
    WatingToBeginSd,
    UpdatingFromSd,
    RestartingAfterSd,
    BeginNetwork,
};

class UpdateStep: public MenuStep {
public:
    UpdateStep(MenuHost* host);
    ~UpdateStep();
    void LoadComplete() override;
    void Tick() override;
    void Paint(BufferedDisplay* g) override;
    SdUpdateState UpdateState = SdUpdateState::None;
    uint8_t* updateBuffer = 0;
    long updateFinishedAt = 0;
    long beginSdWaitStaertedAt = 0;
    long totalBytesReadForSd = 0;
    float Progress = 0;
    String StatusMessage = "Waiting...";
    void NotifyOTAProgressChange(String str);
    void NotifyOTAProgressChange(float progress);
    void NotifyOTAComplete();
    void NotifyOTAFailed();
    void HandleKeyPress(Keys key) override;    
};

#endif