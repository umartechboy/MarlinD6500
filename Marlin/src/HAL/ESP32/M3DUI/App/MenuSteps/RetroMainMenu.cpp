#include "RetroMainMenu.h"
#include "..\MenuApp.h"
#include "..\Images.h"

StringListItem* printFromSDOption;
StringListItem* filamentSetupOption;
StringListItem* bedLevelingOption;
StringListItem* settingsOption;
StringListItem* infoOption;

static void selectionUpdated(void* caller, ListItem* selectedItem, int selectedIndex){
    RetroMainMenuStep* menu = (RetroMainMenuStep*)caller;

    menu->RetroNextStep = 0;
    if (selectedItem == printFromSDOption) menu->RetroNextStep = &sdMenuStep;
    if (selectedItem == filamentSetupOption) menu->RetroNextStep = &materialsMenuStep;
    if (selectedItem == bedLevelingOption) menu->RetroNextStep = &bedLevelStep;
    if (selectedItem == settingsOption) menu->RetroNextStep = 0;
    if (selectedItem == infoOption) menu->RetroNextStep = &printerInfoStep;

    if (menu->RetroNextStep)
        menu->RetroNextStep->RetroPreviousStep = menu;
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
    list = new VerticalList(Host, 128, "");
    list->SetOnSelectionUpdated(this, selectionUpdated);
    printFromSDOption = new StringListItem(Host, &img_RetroSD, "Print from SD", 0, 18);
    filamentSetupOption = new StringListItem(Host, &img_RetroChangeFilament, "Filament Setup", 0, 18);
    bedLevelingOption = new StringListItem(Host, &img_RetroBedLevel, "Bed Leveling", 0, 18);
    settingsOption = new StringListItem(Host, &img_RetroOptions, "Settings", 0, 18);
    infoOption = new StringListItem(Host, &img_RetroRedM3D, "Info", 0, 18);
    list->Add(printFromSDOption);
    list->Add(filamentSetupOption);
    list->Add(bedLevelingOption);
    list->Add(settingsOption);
    list->Add(infoOption);
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
    list->Paint(g, TextColor);
}
void RetroMainMenuStep::IncrementValue() {
    HandleKeyUp(Keys::KEYPAD_UP);
}
void RetroMainMenuStep::DecrementValue() {
    HandleKeyUp(Keys::KEYPAD_DOWN);
}
void RetroMainMenuStep::HandleKeyUp(Keys key) {
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
}

bool RetroMainMenuStep::CanJumpToMainMenu(){
    return false;
}