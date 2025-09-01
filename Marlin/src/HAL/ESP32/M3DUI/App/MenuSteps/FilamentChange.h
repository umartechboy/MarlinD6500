#ifndef __FILAMENT_CHANGE__
#define __FILAMENT_CHANGE__

#include "..\..\Components\M3DUI.h"

enum FilamentChangeStage: uint8_t {
    Preheat = 0,
    ProcessSelection,
    Warning,
    Demo,
    Action,
    Wait
};
enum FilamentChangeActionType{
    Undecided = 0,
    Load,
    Unload,
};
class FilamentChangeStep:public MenuStep
{
private:
    FilamentChangeStage stage = FilamentChangeStage::Preheat;
    int filamentIndex = 0;
    float preHeatTemp = 200;
    FilamentChangeActionType action;
    uint16_t eColors[2];
public:
    FilamentChangeStep(MenuHost* host, int index);
    ~FilamentChangeStep();
    void LoadComplete() override;
    void UnloadBegin() override;
    void Paint(BufferedDisplay* g) override;
    void HandleKeyPress(Keys key) override;         
    void Tick() override;
    bool CanJumpToMainMenu() override;
};

#endif