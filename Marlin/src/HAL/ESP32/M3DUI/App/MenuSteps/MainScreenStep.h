#ifndef IDLE_SCREEN_STEP
#define IDLE_SCREEN_STEP
#include "..\..\Components\M3DUI.h"
#include <Preferences.h>

class MainScreenStep: public MenuStep {
private:    
    Preferences prefs;
    uint16_t e0Color = 0;
    uint16_t e1Color = 0;
    float materialAtStart = 0;
    DummyMenuStep* cancelResumeDummyStep;
    DummyMenuStep* beginResumeDummyStep;
public:
    String fileName;
    String DOSFileName;
    PrintStatus printStatus = PrintStatus::Idle;
    int minX = -1, maxX = -1, minY = -1, maxY = -1, maxZ = -1;
    int xOffset = 0;
    int yOffset = 0;
    MainScreenStep(MenuHost* host);
    ~MainScreenStep();
    void Tick() override;
    void Paint(BufferedDisplay* g) override;
    void HandleKeyUp(Keys key) override;
    void LoadComplete() override;
    void UnloadBegin() override;
    void UnloadComplete() override;
    void FocusChanged(StepAnimationStage stage) override;
    MenuStep* GetPreviousStep(bool returnEvenIfDummy = false) override;
    bool CanJumpToMainMenu() override;
    bool printStarted = false;
};
#endif