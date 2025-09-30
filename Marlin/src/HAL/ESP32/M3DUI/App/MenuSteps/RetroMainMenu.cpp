#include "RetroMainMenu.h"
#include "..\MenuApp.h"
#include "..\Images.h"
#include "..\..\..\..\..\sd\cardreader.h"

DummyMenuStep* unmountSDStep;

static void OnUnmountSDSelected(void* caller){
    RetroMainMenuStep* This = (RetroMainMenuStep*)caller;
    if (card.isMounted()){
        card.release();
    }
    This->Host->PushNotification("You may remove the SD Card", 3000);
}
static void selectionUpdated(void* caller, ListItem* selectedItem, int selectedIndex){
    RetroMainMenuStep* This = (RetroMainMenuStep*)caller;

    This->RetroNextStep = 0;
    if (selectedItem == This->printFromSDOption) This->RetroNextStep = &sdMenuStep;
    if (selectedItem == This->filamentSetupOption) This->RetroNextStep = &materialsMenuStep;
    if (selectedItem == This->bedLevelingOption) This->RetroNextStep = &bedLevelStep;
    if (selectedItem == This->settingsOption) This->RetroNextStep = &settingsStep;
    if (selectedItem == This->unmountSDOption) This->RetroNextStep = unmountSDStep;
    if (selectedItem == This->infoOption) This->RetroNextStep = &printerInfoStep;
    if (selectedItem == This->helpOption) This->RetroNextStep = &helpStep;

    if (This->RetroNextStep)
        This->RetroNextStep->RetroPreviousStep = This;
}

RetroMainMenuStep::RetroMainMenuStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkPaleYellow;
    BackColor = DarkPaleYellow;
    TextColor = ST7735_BLACK;
    Title = "Menu";
    NextActionString = "Select";
    RetroIcon = &img_RetroOptions;
    RetroPreviousStep = &mainScreenStep;
    Icon = &img_SD;
    TickPeriod = 50;
    list = new VerticalList(Host, "");
    list->SetOnSelectionUpdated(this, selectionUpdated);
    printFromSDOption = new StringListItem(Host, &img_RetroSD, "Print from SD", 0, 18);
    filamentSetupOption = new StringListItem(Host, &img_RetroChangeFilament, "Filament Setup", 0, 18);
    bedLevelingOption = new StringListItem(Host, &img_RetroBedLevel, "Bed Leveling", 0, 18);
    settingsOption = new StringListItem(Host, &img_RetroOptions, "Settings", 0, 18);
    unmountSDOption = new StringListItem(Host, &img_RetroSD, "Unmount Card", 0, 18);
    infoOption = new StringListItem(Host, &img_RetroRedM3D, "Info", 0, 18);
    helpOption = new StringListItem(Host, &img_Help, "Help", 0, 18);
    list->Add(printFromSDOption);
    list->Add(filamentSetupOption);
    list->Add(bedLevelingOption);
    list->Add(settingsOption);
    list->Add(unmountSDOption);
    list->Add(infoOption);
    list->Add(helpOption);
    unmountSDStep = new DummyMenuStep(this->Host);
    unmountSDStep->SetLoadCallBack(OnUnmountSDSelected, this);
}
RetroMainMenuStep::~RetroMainMenuStep(){
    delete list;
}
void RetroMainMenuStep::Tick() {
    NeedsRedraw = true;
}
void RetroMainMenuStep::Paint(BufferedDisplay* g) {
    //Serial.printf("SD Menu Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    list->Paint(g, 0, 0, g->width(), g->height(), TextColor);
}
void RetroMainMenuStep::IncrementValue() {
    HandleKeyPress(Keys::KEYPAD_UP);
}
void RetroMainMenuStep::DecrementValue() {
    HandleKeyPress(Keys::KEYPAD_DOWN);
}
void RetroMainMenuStep::HandleKeyPress(Keys key) {
    if (key == Keys::KEYPAD_DOWN)
        list->scrollDown();
    else if (key == Keys::KEYPAD_UP)
        list->scrollUp();
    else if (key == Keys::KEYPAD_RIGHT)
        Host->GotoNextStep();
    else if (key == Keys::KEYPAD_LEFT)
        Host->GotoPreviousStep();
}    

void RetroMainMenuStep::LoadComplete(){    
    materialsMenuStep.RetroPreviousStep = this;
}

bool RetroMainMenuStep::CanJumpToMainMenu(){
    return false;
}