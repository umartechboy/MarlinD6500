#ifndef __BED_LEVEL_STEP__
#define __BED_LEVEL_STEP__

#include "..\..\Components\M3DUI.h"

class BedLevelStep:public MenuStep
{
private:
    float preHeatTemp = 180;
    bool G29Sent = false;
    bool donePreHeating = false;
    int cleaningWipeDueIndex = 0;
    int cleaningWipeSentIndex = -1;
    long levelingDoneSince = 0;
    void notifyLevelingDone();
    int wipeAreaWidth = 15;
    int wipeAreaHeight = 2;
    int wipeRungHeight = 2;
    int pxStart = PROBING_MARGIN + 2 + 18;
    int pyStart = PROBING_MARGIN + 2;
public:
    BedLevelStep(MenuHost* host);
    ~BedLevelStep();
    void LoadComplete() override;
    void UnloadBegin() override;
    void Paint(BufferedDisplay* g) override;
    void HandleKeyPress(Keys key) override;         
    void Tick() override;
    void FocusChanged(StepAnimationStage currentStage) override;
    MenuStep* GetPreviousStep(bool returnEvenIfDummy = false) override;
};

#endif