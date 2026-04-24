#ifndef UPDATE2_MENU_STEP
#define UPDATE2_MENU_STEP

#include "..\..\Components\M3DUI.h"
#define UpdateFileName "FIRMWARE.BIN"
#define SdUpdateBufferSize (1024 * 16)

enum SdUpdateState : uint8_t{
    None = 0,
    BeginSd,
    UpdatingFromSd,
    RestartingAfterSd,
    BeginNetwork,
};

class Update2MenuStep: public MenuStep {
public:
    Update2MenuStep(MenuHost* host);
    ~Update2MenuStep();
    void LoadComplete() override;
    void Tick() override;
    void Paint(BufferedDisplay* g) override;
    SdUpdateState UpdateState = SdUpdateState::None;
    uint8_t* updateBuffer = 0;
    long updateFinishedAt = 0;
    long totalBytesReadForSd = 0;
    float Progress = 0;
    String StatusMessage = "";
};

#endif