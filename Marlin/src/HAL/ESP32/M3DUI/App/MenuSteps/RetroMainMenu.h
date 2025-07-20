#ifndef RETRO_OPTIONS_MENU_STEP
#define RETRO_OPTIONS_MENU_STEP

#include "..\..\Components\M3DUI.h"

class RetroMainMenuStep: public MenuStep {
    public:
    bool hasSDCard = false;
    VerticalList* list;
    RetroMainMenuStep(MenuHost* host);
    ~RetroMainMenuStep();
    void LoadComplete() override;
    void Tick() override;
    void Paint(BufferedDisplay* g) override;
    void IncrementValue() override;
    void DecrementValue() override;
    void HandleKeyUp(Keys key) override;
    bool CanJumpToMainMenu() override;
};

#endif