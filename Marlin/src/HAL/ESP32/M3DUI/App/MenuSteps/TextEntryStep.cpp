#include "TextEntryStep.h"
#include "..\..\Hardware\MarlinSpecific.h"
#include "..\Images.h"
#include "Fonts\FreeMono12pt7b.h"
#include "Fonts\FreeMono9pt7b.h"
#include "..\..\..\..\..\module\printcounter.h"
#include <WiFi.h>
#include "..\MenuApp.h"

void OnConfirmed(void* owner){
    TextEntryStep* This = (TextEntryStep*)owner;
    
    if (This->textEditCallback){
        (*This->textEditCallback)(This->TextEntry->Text, This->textEditCallbackOwner);
        This->Host->GotoPreviousStep();
    }
}

TextEntryStep::TextEntryStep(MenuHost* host):MenuStep(host)
{
    TextColor = ST7735_WHITE;
    BackColor = DarkRed;
    TickPeriod = 50;
    TextEntry = new TextEntryField();
    confirmStep = new DummyMenuStep(host);
    confirmStep->SetLoadCallBack(OnConfirmed, this);
    RetroNextStep = confirmStep;
    NextActionString = "OK";
}
TextEntryStep::~TextEntryStep()
{
    delete TextEntry;
}
void TextEntryStep::Paint(BufferedDisplay* g){            
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    centerString(g, Message.c_str(), Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2 - 20);
    TextEntry->Paint(g, 3, retroTitleSectionHeight + Host->appHeight() / 2 - 8, Host->appWidth() - 6, 25, &FreeMono9pt7b);
}
void TextEntryStep::Tick(){
    NeedsRedraw = true;
}

void TextEntryStep::RequestTextEntry(String Label, String message, String Text, void (*callback)(String& edited, MenuStep* _owner), MenuStep* owner){
    // Save context
    textEditCallback = callback;
    textEditCallbackOwner = owner;
    textEditOwnerNextStepBackup = owner->GetNextStep();
    // Prepare the text field.
    Title = Label;
    Message = message;
    TextEntry->Text = Text;
    // Prepare exit
    RetroPreviousStep = owner;
    // Go!
    textEditCallbackOwner->RetroNextStep = this;
    Host->GotoNextStep();
}
void TextEntryStep::LoadComplete(){
    Host->RequestTextEntry(TextEntry);
}
void TextEntryStep::HandleKeyPress(Keys key) {
    if (key == Keys::KEYPAD_RIGHT){
        Host->GotoNextStep(); // back to options menu
    }
}
void TextEntryStep::UnloadBegin(){
    Host->ReleaseTextEntry();
    // We already have sent the text change callback in case of confirmation
    // Restrore context before going    
    textEditCallbackOwner->NextStep = textEditOwnerNextStepBackup;
    textEditCallbackOwner->RetroNextStep = textEditOwnerNextStepBackup;
}
