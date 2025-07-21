#ifndef __BED_LEVEL_STEP__
#define __BED_LEVEL_STEP__

#include "..\..\Components\M3DUI.h"

class BedLevelStep:public MenuStep
{
private:
    float preHeatTemp = 50;
    bool G29Sent = false;
    bool donePreHeating = false;
    long levelingDoneSince = 0;
    void notifyLevelingDone();
public:
    BedLevelStep(MenuHost* host);
    ~BedLevelStep();
    void LoadComplete() override;
    void UnloadBegin() override;
    void Paint(BufferedDisplay* g) override;
    void HandleKeyUp(Keys key) override;         
    void Tick() override;
    void FocusChanged(StepAnimationStage currentStage) override;
    MenuStep* GetPreviousStep() override;
};

#endif