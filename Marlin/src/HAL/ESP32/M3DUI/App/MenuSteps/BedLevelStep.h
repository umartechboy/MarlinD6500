#ifndef __BED_LEVEL_STEP__
#define __BED_LEVEL_STEP__

#include "..\..\Components\M3DUI.h"

class BedLevelStep:public MenuStep
{
private:
    float preHeatTemp = 30;
    bool G29Sent = false;
    bool donePreHeating = false;
public:
    BedLevelStep(MenuHost* host);
    ~BedLevelStep();
    void LoadComplete() override;
    void UnloadBegin() override;
    void Paint(BufferedDisplay* g) override;
    void HandleKeyUp(Keys key) override;         
    void Tick() override;
};

#endif