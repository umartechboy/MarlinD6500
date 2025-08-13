#ifndef __UPDATE_STEP__
#define __UPDATE_STEP__
#include "..\..\Components\M3DUI.h"
#include <Preferences.h>

class UpdateStep: public MenuStep {
private:    
    MenuStep* retroPreviousStepBkp;
    String StatusMessage;
    int Progress = 0;
public:
    UpdateStep(MenuHost* host);
    ~UpdateStep();
    void Tick() override;
    void Paint(BufferedDisplay* g) override;
    void LoadComplete() override;
    void NotifyOTAProgressChange(String str);
    void NotifyOTAProgressChange(float progress);
    void NotifyOTAComplete();
    void NotifyOTAFailed();
};
#endif