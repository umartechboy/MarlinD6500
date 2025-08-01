#ifndef __TEXT_ENTRY_STEP__
#define __TEXT_ENTRY_STEP__

#include "..\..\Components\M3DUI.h"

class TextEntryStep:public MenuStep
{
private:
    DummyMenuStep* confirmStep;
public:
    TextEntryStep(MenuHost* host);
    ~TextEntryStep();
    void Paint(BufferedDisplay* g) override;
    void HandleKeyPress(Keys key) override;  
    void Tick() override;   
    void UnloadBegin() override;
    void LoadComplete() override;
    bool CanJumpToMainMenu() override { return false; }
    TextEntryField* TextEntry;
    String Message;
    void (*textEditCallback)(String& str, MenuStep* _owner);
    MenuStep *textEditCallbackOwner;
    MenuStep* textEditOwnerNextStepBackup;    
    void RequestTextEntry(String Label, String message, String Text, void (*callback)(String& edited, MenuStep* _owner), MenuStep* owner);
};

#endif