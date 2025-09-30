#ifndef __HELP_STEP__
#define __HELP_STEP__

#include "..\..\Components\M3DUI.h"

class HelpStep:public MenuStep
{
private:
    bool displayingQR = true;
public:
    HelpStep(MenuHost* host);
    void Paint(BufferedDisplay* g) override;
    void HandleKeyPress(Keys key) override;  
};

#endif