#ifndef _IN_PRINT_MENU_STEP
#define _IN_PRINT_MENU_STEP

#include "..\..\Components\M3DUI.h"

class InPrintMenuStep: public MenuStep {
public:
    VerticalList* list;
    InPrintMenuStep(MenuHost* host);
    ~InPrintMenuStep();
    void LoadBegin() override;
    void LoadComplete() override;
    void UnloadComplete() override;
    void Tick() override;
    void Paint(BufferedDisplay* g) override;
    void IncrementValue() override;
    void DecrementValue() override;
    void HandleKeyPress(Keys key) override;
    bool CanJumpToMainMenu() override;
    bool sentForMaterialChange = false;
    StringListItem* backToPrintOption;
    StringListItem* filamentSetupOption;
    StringListItem* abortPrintOption;
    DummyMenuStep* backToPrintStep;
    DummyMenuStep* filamentSetupStep;
    DummyMenuStep* abortPrintStep;
};

#endif