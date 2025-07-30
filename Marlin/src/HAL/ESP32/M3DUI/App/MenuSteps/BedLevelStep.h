#ifndef __BED_LEVEL_STEP__
#define __BED_LEVEL_STEP__

#include "..\..\Components\M3DUI.h"

class BedLevelStep:public MenuStep
{
private:
    float preHeatTemp = 200;
    bool G29Sent = false;
    bool donePreHeating = false;
    int cleaningWipeDueIndex = 0;
    int cleaningWipeSentIndex = -1;
    long levelingDoneSince = 0;
    void notifyLevelingDone();
    int wipeAreaWidth = 10;
    int wipeAreaHeight = 10;
    int wipeRungHeight = 2;
    int pxStart = ((X_BED_SIZE - 2 * PROBING_MARGIN) * 3) / 4 - wipeAreaWidth;
    int pyStart = ((Y_BED_SIZE - 2 * PROBING_MARGIN) * 3) / 4 - wipeAreaHeight;
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