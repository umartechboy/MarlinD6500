#ifndef __SETTINGS_STEP__
#define __SETTINGS_STEP__

#include "..\..\Components\M3DUI.h"

class SettingsStep:public MenuStep
{
private:
public:
    SettingsStep(MenuHost* host);
    ~SettingsStep();
    void Paint(BufferedDisplay* g) override;
    void HandleKeyPress(Keys key) override;  
    void IncrementValue();
    void DecrementValue();
    void Tick() override;   
    VerticalList* options;
    StringListItem* setNetworkOption;
    StringListItem* updatesOption;
    StringListItem* sensorsOption;
    StringListItem* motorMovementOption;
};

#endif