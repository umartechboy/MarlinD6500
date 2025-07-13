#ifndef __FILAMENT_CHANGE__
#define __FILAMENT_CHANGE__

#include "..\..\Components\M3DUI.h"

class FilamentChangeStep:public MenuStep
{
private:
    int filamentIndex = 0;
    float preHeatTemp = 200;
public:
    FilamentChangeStep(MenuHost* host, int index);
    ~FilamentChangeStep();
    void LoadComplete() override;
    void UnloadBegin() override;
    void Paint(BufferedDisplay* g) override;
    void HandleKeyUp(Keys key) override;         
    void Tick() override;
};

#endif