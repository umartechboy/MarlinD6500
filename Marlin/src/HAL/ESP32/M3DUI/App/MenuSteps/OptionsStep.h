#ifndef TOOLS_STEP
#define TOOLS_STEP

#include "..\..\Components\M3DUI.h"

class OptionsStep: public MenuStep {
    public:
        OptionsStep(MenuHost* host);
        void Paint(BufferedDisplay* g) override;
        void HandleKeyPress(Keys key) override;
};
#endif