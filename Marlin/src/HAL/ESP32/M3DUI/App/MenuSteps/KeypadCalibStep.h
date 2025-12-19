#ifndef __KEYPAD_CALIB_STEP__
#define __KEYPAD_CALIB_STEP__

#include "..\..\Components\M3DUI.h"

class KeypadCalibStep:public MenuStep
{
private:
public:
    KeypadCalibStep(MenuHost* host);
    void Paint(BufferedDisplay* g) override;
    void Tick() override;   
    void LoadComplete() override;
};

#endif