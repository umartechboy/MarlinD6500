#ifndef __M3DUI_H__
#define __M3DUI_H__
#include "..\Hardware\BufferedDisplay.h"
#include <Adafruit_ST7735.h>
#include <vector>
#include "Bitmap.h"
#include "Utils.h"
#include "..\Hardware\Keypad.h"
#include "..\..\..\..\MarlinCore.h"
#include "UIEnums.h"

static uint16_t AvailableColors[] = {ST7735_BLACK, ST7735_WHITE, ST7735_RED, ST7735_GREEN, ST7735_BLUE, ST7735_CYAN, ST7735_MAGENTA, ST7735_YELLOW, ST7735_ORANGE};

class ListItem{
public:
    ListItem(MenuHost* host, int _height);
    MenuHost* Host = 0;
    virtual void Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int x, int y, int width, int height, bool selected){} // Must be overriden
    int getHeight(){
        return itemHeight;
    }
    bool IsDummyItem(){
        return isDummyItem;
    }
protected:
    bool isDummyItem = false;
    int itemHeight = 0;
};
class ListSeparatorItem: public ListItem{
    public:
    ListSeparatorItem(MenuHost* host):ListItem(host, 8){
        isDummyItem = true;
    }
    virtual void Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int y, bool selected);; // Must be overriden
};
class StringListItem: public ListItem{
public:
    StringListItem(MenuHost* host, Image* icon, String str, int _endTrimLength, int height);
    String ItemText;
    void Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int x, int y, int width, int height, bool selected) override;
private:
    int endTrimLength;
    Image* Icon;
};

class FileNameListItem: public StringListItem{
public:
    String DOSName;
    FileNameListItem(MenuHost* host, String str, String dosName, int _endTrimLength, int height);
};

class ColorSelectorListItem: public ListItem{
public:
    ColorSelectorListItem(MenuHost* host, String str, int colorIndex, int height);
    String ItemText;
    String Label;
    int selectedColorIndex = 0;
    void incrementColor();
    void decrementColor();
    void Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int x, int y, int width, int height, bool selected) override;
};

typedef void (*SelectionUpdatedCallback)(void* caller, ListItem* selectedItem, int selectedIndex);

class VerticalList{
    private:
    std::vector<ListItem*> items;
        int scrollOffset = 0;
        int targetScrollOffset = 0;
        MenuHost* Host = 0;
        int lastSelected = -2;
        SelectionUpdatedCallback OnSelectionUpdated = 0;
        void *Owner = 0;
    public:
        int selected = -1;
        String EmptyString;
        VerticalList(MenuHost* host, String emptyString = "");
        ~VerticalList();
        int getSelectedIndex();
        ListItem* getSelected();
        void SetOnSelectionUpdated(void* owner, SelectionUpdatedCallback _OnSelectionUpdated);
        void InvokeSelectionChanged();
        // Overload operator[] for non-const access
        ListItem* operator[](int index);
        void Clear();
        ListItem* At(int index);
        void Add(ListItem* item);
        int Count();
        void scrollDown();
        void scrollUp();
        void Paint(BufferedDisplay* g, int x, int y, int w, int h, Color TextColor);
        void MarkSelectionChangeSent();
};

class Notification
{
private:
    int fadeInProgress = 0;
    int fadeOutProgress = 0;
public:
    int lifeLeft = 0;
    long lastDrawing = 0;
    String Text;
    Notification(String& text, int life);
    void Paint(BufferedDisplay* g);
    void HandleKeyPress(Keys key);
    ~Notification();
};

class MenuHost;
class MenuStep {
   public:
        bool NeedsRedraw = true;
        MenuHost* Host;
        MenuStep* NextStep = 0;
        MenuStep* PreviousStep = 0;
        MenuStep* RetroNextStep = 0;
        MenuStep* RetroPreviousStep = 0;
        Color ButtonColor;
        Color BackColor;
        Color TextColor;
        Image* Icon = 0;
        Image* RetroIcon = 0;
        String Title;
        String NextActionString;
        MenuStep(MenuHost* host);
        virtual void Paint(BufferedDisplay* g) {}
        virtual void PaintRetroTitle(BufferedDisplay* g);
        virtual void PaintRetroOptionsBar(BufferedDisplay* g);
        virtual void HandleKeyPress(Keys key) {}
        virtual void IncrementValue() {}
        virtual void DecrementValue() {}
        virtual void LoadBegin() {}
        virtual void LoadComplete() {}
        virtual void UnloadBegin() {}
        virtual void UnloadComplete() {}
        virtual void FocusChanged(StepAnimationStage currentStage) {}
        virtual MenuStep* GetPreviousStep(bool returnEvenIfDummy = false);
        virtual MenuStep* GetNextStep();
        virtual bool CanJumpToMainMenu();
        void loop();
        bool IsDummyStep();
        long TickPeriod = 0;
    protected: 
        long lastTick = 0;
        bool isDummy = false;
        virtual void Tick() {}
};

class DummyMenuStep:public MenuStep{
public:
    DummyMenuStep(MenuHost* host):MenuStep(host){ isDummy = true; }
    void SetLoadCallBack(void (*_callback)(void*), void* _sender) { callback = _callback; sender = _sender; }
    void NotifySelected(){ if (callback) (*callback)(sender); }
private:
    void (*callback)(void* sender) = 0;
    void* sender = 0;
};

class TextEntryField {
public:
    String Text;
    void Paint(BufferedDisplay* g, int x, int y, int width, int height, const GFXfont* font);
    int cursor = -1;
};

class TextEntrySource {
public:
    VerticalList* chars;
    TextEntrySource(MenuHost* host);
    void setTarget(TextEntryField* target);
    ~TextEntrySource();
    void Loop();
    void HandleKeyPress(Keys key);
    void Paint(BufferedDisplay* g, int x, int y, int width, int height);
    MenuHost* Host;
    bool isCaps = false;
private:
    TextEntryField* Target;
};

class EntryCharListItem: public ListItem{
public:
    TextEntrySource* Owner;
    EntryCharListItem(TextEntrySource* owner, char chr, int height);
    void Paint(BufferedDisplay* g, uint8_t op, uint16_t TextColor, int x, int y, int width, int height, bool selected) override;
    char GetChar();
private:
    char ItemChar;
};
#endif // M3DUI