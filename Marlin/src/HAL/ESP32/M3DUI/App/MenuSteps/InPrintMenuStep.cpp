#include "InPrintMenuStep.h"
#include "..\MenuApp.h"
#include "..\Images.h"
#include "..\..\Hardware\MarlinSpecific.h"

static void backToPrintSelected(void* caller){
    // resume the active print
    resumePrint();
    InPrintMenuStep* This = (InPrintMenuStep*)caller;
    This->RetroNextStep = &mainScreenStep;
    This->Host->GotoNextStep();

}
static void filamentSetupSelected(void* caller){
    InPrintMenuStep* This = (InPrintMenuStep*)caller;
    This->RetroNextStep = &materialsMenuStep;
    materialsMenuStep.RetroPreviousStep = This;
    This->Host->GotoNextStep();
    This->sentForMaterialChange = true;
}
static void abortPrintSelected(void* caller){
    abortPrint();    
    InPrintMenuStep* This = (InPrintMenuStep*)caller;
    This->RetroNextStep = &mainScreenStep;
    mainScreenStep.fileName = "";
    mainScreenStep.DOSFileName = "";
    mainScreenStep.printStatus = PrintStatus::Idle;
    This->Host->GotoNextStep();
    This->RetroPreviousStep = 0; // we are now in the main menu
}

static void selectionUpdated(void* caller, ListItem* selectedItem, int selectedIndex){
    InPrintMenuStep* This = (InPrintMenuStep*)caller;

    This->RetroNextStep = 0;
    if (selectedItem == This->backToPrintOption) This->RetroNextStep = This->backToPrintStep;
    if (selectedItem == This->filamentSetupOption) This->RetroNextStep = This->filamentSetupStep;
    if (selectedItem == This->abortPrintOption) This->RetroNextStep = This->abortPrintStep;

    if (This->RetroNextStep)
        This->RetroNextStep->RetroPreviousStep = This;
}

InPrintMenuStep::InPrintMenuStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkPaleYellow;
    BackColor = DarkPaleYellow;
    TextColor = ST7735_BLACK;
    Title = "Print Paused";
    NextActionString = "Select";
    RetroIcon = &img_RetroOptions;
    RetroPreviousStep = &mainScreenStep;
    TickPeriod = 50;
    list = new VerticalList(Host, "");
    list->SetOnSelectionUpdated(this, selectionUpdated);
    backToPrintOption = new StringListItem(Host, &img_RetroPlay, "Back to print", 0, 18);
    filamentSetupOption = new StringListItem(Host, &img_RetroChangeFilament, "Filament Setup", 0, 18);
    abortPrintOption = new StringListItem(Host, &img_RetroPause, "Abort Print", 0, 18);
    list->Add(backToPrintOption);
    list->Add(filamentSetupOption);
    list->Add(abortPrintOption);
    backToPrintStep = new DummyMenuStep(Host);
    filamentSetupStep = new DummyMenuStep(Host);
    abortPrintStep = new DummyMenuStep(Host);
    backToPrintStep->SetLoadCallBack(backToPrintSelected, this);
    filamentSetupStep->SetLoadCallBack(filamentSetupSelected, this);
    abortPrintStep->SetLoadCallBack(abortPrintSelected, this);
}
InPrintMenuStep::~InPrintMenuStep(){
    delete list;
    delete backToPrintStep;
    delete filamentSetupStep;
    delete abortPrintStep;
}
void InPrintMenuStep::Tick() {
    NeedsRedraw = true;
}
void InPrintMenuStep::Paint(BufferedDisplay* g) {
    //Serial.printf("SD Menu Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    list->Paint(g, 0, 0, g->width(), g->height(), TextColor);
}
void InPrintMenuStep::IncrementValue() {
    HandleKeyPress(Keys::KEYPAD_UP);
}
void InPrintMenuStep::DecrementValue() {
    HandleKeyPress(Keys::KEYPAD_DOWN);
}
void InPrintMenuStep::HandleKeyPress(Keys key) {
    if (key == Keys::KEYPAD_DOWN)
        list->scrollDown();
    else if (key == Keys::KEYPAD_UP)
        list->scrollUp();
    else if (key == Keys::KEYPAD_RIGHT)
        Host->GotoNextStep();
    else if (key == Keys::KEYPAD_LEFT)
        Host->GotoPreviousStep();
}    

void InPrintMenuStep::LoadBegin(){
    SERIAL_IMPL.println("InPrint menu Loaded");
    if (!sentForMaterialChange)
        pausePrint();
}
void InPrintMenuStep::LoadComplete(){        
    sentForMaterialChange = false;
}

void InPrintMenuStep::UnloadComplete(){    
}
bool InPrintMenuStep::CanJumpToMainMenu(){
    return false;
}