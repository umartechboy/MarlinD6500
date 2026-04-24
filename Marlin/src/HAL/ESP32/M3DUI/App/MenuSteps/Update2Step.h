#ifndef UPDATE2_MENU_STEP
#define UPDATE2_MENU_STEP

#include "..\..\Components\M3DUI.h"
#define UpdateFileName "FIRMWARE.BIN"
#define SdUpdateBufferSize (1024 * 16)

enum SdUpdateState : uint8_t{
    None = 0,
    MountingSd,
    UpdatingFromSd,
    RestartingAfterSd,
};

class Update2MenuStep: public MenuStep {
public:
    bool hasSDCard = false;
    VerticalList* list;
    Update2MenuStep(MenuHost* host);
    ~Update2MenuStep();
    void LoadComplete() override;
    void Tick() override;
    void Paint(BufferedDisplay* g) override;
    void IncrementValue() override;
    void DecrementValue() override;
    void HandleKeyPress(Keys key) override;
    SdUpdateState UpdateState = SdUpdateState::MountingSd;
    uint8_t* updateBuffer = 0;
    long updateFinishedAt = 0;
};

#endif