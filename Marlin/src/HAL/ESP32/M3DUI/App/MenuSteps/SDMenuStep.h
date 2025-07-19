#ifndef SD_MENU_STEP
#define SD_MENU_STEP

#include "..\..\Components\M3DUI.h"

class SDMenuStep: public MenuStep {
    public:
    bool hasSDCard = false;
    VerticalList list;
    SDMenuStep(MenuHost* host);
    void LoadComplete() override;
    void Tick() override;
    void Paint(BufferedDisplay* g) override;
    void IncrementValue() override;
    void DecrementValue() override;
    void HandleKeyUp(Keys key) override;
    void updateSelection();
    private:
    int lastSelected = -1;
};

#endif