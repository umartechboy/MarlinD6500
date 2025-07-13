#ifndef SD_MENU_STEP
#define SD_MENU_STEP

#include "..\..\Components\M3DUI.h"

class SDMenuStep: public MenuStep {
    public:
    bool hasSDCard = false;
    VerticalList list = VerticalList(14, 128, "No SD Card");
    SDMenuStep(MenuHost* host);
    void Tick() override;
    void Paint(BufferedDisplay* g) override;
    void BeginSD();
    void IncrementValue() override;
    void DecrementValue() override;
    void HandleKeyUp(Keys key) override;
    void updateSelection();
    private:
    int lastSelected = -1;
};

#endif