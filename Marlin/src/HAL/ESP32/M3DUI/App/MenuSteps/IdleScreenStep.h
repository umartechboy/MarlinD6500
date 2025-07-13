#ifndef IDLE_SCREEN_STEP
#define IDLE_SCREEN_STEP
#include "..\..\Components\M3DUI.h"

class IdleScreenStep: public MenuStep {
    public:
        PrintStatus CurrentPrintStatus = PrintStatus::Idle;
        IdleScreenStep(MenuHost* host);
        void Tick() override;
        void Paint(BufferedDisplay* g) override;
        void HandleKeyUp(Keys key) override;
};
#endif