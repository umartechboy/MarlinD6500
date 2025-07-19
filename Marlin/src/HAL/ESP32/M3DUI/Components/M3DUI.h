#ifndef __M3DUI_H__
#define __M3DUI_H__
#include "..\Hardware\BufferedDisplay.h"
#include <Adafruit_ST7735.h>
#include <vector>
#include "Bitmap.h"
#include "Utils.h"
#include "..\Hardware\Keypad.h"
#include "..\..\..\..\MarlinCore.h"

static uint16_t AvailableColors[] = {ST7735_BLACK, ST7735_WHITE, ST7735_RED, ST7735_GREEN, ST7735_BLUE, ST7735_CYAN, ST7735_MAGENTA, ST7735_YELLOW, ST7735_ORANGE};

enum TransitionDirection{
    Forward = 0,
    Backward,
};

class ListItem{
    public:
        virtual void Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int y, bool selected);; // Must be overriden
        bool IsDummyItem(){
            return isDummyItem;
        }
    protected:
        bool isDummyItem = false;
};
class ListSeparatorItem: public ListItem{
    public:
    ListSeparatorItem(){
        isDummyItem = true;
    }
    virtual void Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int y, bool selected);; // Must be overriden
};
class StringListItem: public ListItem{
public:
    StringListItem();
    StringListItem(String str, int _endTrimLength);
    String ItemText;
    void Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int y, bool selected) override;
private:
    int endTrimLength;
};
class FileNameListItem: public StringListItem{
public:
    String DOSName;
    FileNameListItem(String str, String dosName, int _endTrimLength);
};

class ColorSelectorListItem: public ListItem{
public:
    ColorSelectorListItem();
    ColorSelectorListItem(String str, int colorIndex);
    String ItemText;
    String Label;
    int selectedColorIndex = 0;
    void incrementColor();
    void decrementColor();
    void Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int y, bool selected) override;
};
class VerticalList{
    private:
    std::vector<ListItem*> items;
        int scrollOffset = 0;
        int targetScrollOffset = 0;
        int lineHeight = 0;
        int displayHeight = 0;
    public:
        int selected = -1;
        String EmptyString;
        VerticalList(int lineHeight, int _displayHeight, String emptyString = "");
        ~VerticalList();
        int getSelectedIndex();
        ListItem* getSelected();
        // Overload operator[] for non-const access
        ListItem* operator[](int index);
        void Clear();
        void Add(ListItem* item);
        int Count();
        void scrollDown();
        void scrollUp();
        void Paint(BufferedDisplay* g, Color TextColor);
};

enum StepAnimationStage{
    MainStep = 0,
    GoingToOverLay,
    InOverlay,
    GoingToStep,
};

enum MenuTransitionStage{
    TransitionaingScreens,
    InStep,
};
enum PrintStatus{
    Idle,
    FileToPrint,
    PrintToResume,
    ChangingFilament
};
class MenuHost;
class MenuStep {
   public:
        bool NeedsRedraw = true;
        MenuHost* Host;
        MenuStep* NextStep = 0;
        MenuStep* PreviousStep = 0;
        Color ButtonColor;
        Color BackColor;
        Color TextColor;
        Bitmap* Icon = 0;
        MenuStep(MenuHost* host);
        virtual void Paint(BufferedDisplay* g) {}
        virtual void HandleKeyUp(Keys key) {}
        virtual void IncrementValue() {}
        virtual void DecrementValue() {}
        virtual void LoadBegin() {}
        virtual void LoadComplete() {}
        virtual void UnloadBegin() {}
        virtual void UnloadComplete() {}
        virtual void FocusChanged(StepAnimationStage currentStage) {}
        void loop();
    protected: 
        long lastTick = 0;
        long TickPeriod = 0;
    protected: 
        virtual void Tick() {}
};

class MenuHost{
    public:
    MenuStep* CurrentStep = 0;
    MenuStep* TargetStep = 0;
    MenuTransitionStage menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
    TransitionDirection menuTransitionDirection = TransitionDirection::Forward;
    StepAnimationStage stepAnimationStage = StepAnimationStage::MainStep;
    MenuStep* moveToMenuAfterStepAnim = 0;
    long lastPaint = 0;
    MenuHost();
    ~MenuHost();
    bool CanGotoNextStep();
    void GotoStepFromNull(MenuStep* step);
    void GotoNextStep();
    void GotoPreviousStep();
    bool NeedsMenuTransition();
    void ResetAnimationProgress(long duration = 200);
    void IncrementAnimationProgress();
    void DrawButton(int index, MenuStep* step, BufferedDisplay* g, float progress);
    int appHeight();
    int appWidth();
    // Top left
    void DrawButton1(MenuStep* step, BufferedDisplay* g, float progress);
    // Bottom Right
    void DrawButton2(MenuStep* step, BufferedDisplay* g, float progress);
    // Top Right
    void DrawButton3(MenuStep* step, BufferedDisplay* g, float progress);
    void Paint(BufferedDisplay* bTft);
    void Loop(BufferedDisplay* bTft);
    private:
        float animationStepProgress = 0.0f;
        long animationStartTime = 0;
        long animationEndTime = 0;
        KeyPad* keypad;
};


#endif // M3DUI