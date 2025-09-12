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
    int wipeAreaWidth = 15;
    int wipeAreaHeight = 25;
    int wipeRungHeight = 5;
    int pxStart = 30 + PROBING_MARGIN + ((X_BED_SIZE - 2 * PROBING_MARGIN) * 3) / 4 - wipeAreaWidth / 2;
    int pyStart = PROBING_MARGIN + ((Y_BED_SIZE - 2 * PROBING_MARGIN) * 3) / 4 - wipeAreaHeight;
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