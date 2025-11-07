#ifndef __SENSOR_INFO_STEP__
#define __SENSOR_INFO_STEP__

#include "..\..\Components\M3DUI.h"

class SensorInfoStep:public MenuStep
{
private:
    long autoBackTimeoutStartAt = -1;
    int keysToBack = 5;
    long timeLeft = -1;
public:
    SensorInfoStep(MenuHost* host);
    void Paint(BufferedDisplay* g) override;
    void HandleKeyPress(Keys key) override;  
    bool CanJumpToMainMenu() override;
    void LoadBegin() override;
    void UnloadBegin() override;
    void Tick() override;
};

#endif