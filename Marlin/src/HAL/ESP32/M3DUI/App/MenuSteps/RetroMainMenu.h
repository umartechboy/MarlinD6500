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
    void HandleKeyPress(Keys key) override;
    bool CanJumpToMainMenu() override;
    StringListItem* printFromSDOption;
    StringListItem* filamentSetupOption;
    StringListItem* bedLevelingOption;
    StringListItem* settingsOption;
    StringListItem* unmountSDOption;
    StringListItem* infoOption;
};

#endif