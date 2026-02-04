#ifndef __M3D_HOST__
#define __M3D_HOST__
#include "UIComponents.h"
#include "..\Hardware\Keypad.h"
#include "UIEnums.h"

class MenuStep;
class Notification;
class TextEntryField;
class TextEntrySource;

class MenuHost{
public:
    MenuStep* CurrentStep = 0;
    MenuStep* TargetStep = 0;
    MenuTransitionStage menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
    TransitionDirection menuTransitionDirection = TransitionDirection::Forward;
    StepAnimationStage stepAnimationStage = StepAnimationStage::MainStep;
    MenuStep* moveToMenuAfterStepAnim = 0;
    Notification* CurrentNotification = 0;
    long lastPaint = 0;
    bool Retro = true;
    MenuHost();
    ~MenuHost();
    bool CanGotoNextStep();
    void GotoStepFromAny(MenuStep* step);
    void GotoNextStep();
    void GotoPreviousStep();
    bool NeedsMenuTransition();
    void ResetAnimationProgress(long duration = 200);
    void IncrementAnimationProgress();
    void DrawButton(int index, MenuStep* step, BufferedDisplay* g, float progress);
    int appHeight();
    int appWidth();
    int appTop();
    int appLeft();
    void HandleKeyPress(Keys key);
    void HandleKeyHold(Keys key);
    void HandleDialIncrement();
    void HandleDialDecrement();
    void NotifyNoKey();
    // Top left
    void DrawButton1(MenuStep* step, BufferedDisplay* g, float progress);
    // Bottom Right
    void DrawButton2(MenuStep* step, BufferedDisplay* g, float progress);
    // Top Right
    void DrawButton3(MenuStep* step, BufferedDisplay* g, float progress);
    void Paint(BufferedDisplay* bTft);
    void PushNotification(const char* str, int life = -1);
    void Loop(BufferedDisplay* bTft);
    void RequestTextEntry(TextEntryField* textField);
    bool HasTextEntry();
    void ReleaseTextEntry();
private:
    float animationStepProgress = 0.0f;
    long animationStartTime = 0;
    long animationEndTime = 0;
    KeyPad* keypad;
    int _appHeight = 0;
    int _appWidth = 0;
    TextEntryField* textEntryTarget = 0;
    TextEntrySource* textEntrySource;
};


#endif