#ifndef SD_MENU_STEP
#define SD_MENU_STEP

#include "..\..\Components\M3DUI.h"

class SDMenuStep: public MenuStep {
public:
    bool hasSDCard = false;
    VerticalList* list;
    SDMenuStep(MenuHost* host);
    ~SDMenuStep();
    void LoadComplete() override;
    void Tick() override;
    void Paint(BufferedDisplay* g) override;
    void IncrementValue() override;
    void DecrementValue() override;
    void HandleKeyPress(Keys key) override;
};

#endif