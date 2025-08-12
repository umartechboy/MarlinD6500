#include "SettingsStep.h"
#include "..\..\Hardware\MarlinSpecific.h"
#include "..\Images.h"
#include "Fonts\FreeMono12pt7b.h"
#include "Fonts\FreeMono9pt7b.h"
#include "..\..\..\..\..\module\printcounter.h"
#include <WiFi.h>
#include "..\MenuApp.h"

void OnSelectionUpdatedCallback(void* caller, ListItem* selectedItem, int selectedIndex){
    SettingsStep* This = (SettingsStep*)caller;
    if (selectedItem == This->setNetworkOption){
        This->RetroNextStep = &wifiListStep;
        SERIAL_IMPL.printf("Setting next step: %s\n", ((StringListItem*)selectedItem)->ItemText.c_str());
    }
    else if (selectedItem == This->updatesOption){
        This->RetroNextStep = &updateStep;
        updateStep.PreviousStep = This;
        SERIAL_IMPL.printf("Setting next step: %s\n", ((StringListItem*)selectedItem)->ItemText.c_str());
    }
    else {
        This->RetroNextStep = 0;
    }
}
SettingsStep::SettingsStep(MenuHost* host):MenuStep(host)
{
    TextColor = ST7735_WHITE;
    BackColor = DarkRed;
    Icon = &img_M3D;
    TickPeriod = 50;
    RetroNextStep = 0;
    Title = "Settings";
    options = new VerticalList(host);
    setNetworkOption = new StringListItem(host, 0, "Change Network", 0, 16);
    updatesOption = new StringListItem(host, 0, "Check for updates", 0, 16);
    sensorsOption = new StringListItem(host, 0, "Test Sensors", 0, 16);
    options->Add(setNetworkOption);
    options->Add(updatesOption);
    options->Add(sensorsOption);
    options->SetOnSelectionUpdated(this, OnSelectionUpdatedCallback);
    //options->InvokeSelectionChanged();
}
SettingsStep::~SettingsStep(){
    delete options;
}
void SettingsStep::Paint(BufferedDisplay* g){            
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    uint8_t opBkp = g->GetOpacity();

    options->Paint(g, 0, retroTitleSectionHeight, Host->appWidth(), Host->appHeight(), ST7735_WHITE);
    g->SetOpacity(opBkp);

}
void SettingsStep::Tick(){
    NeedsRedraw = true;
}

void SettingsStep::IncrementValue() {
    HandleKeyPress(Keys::KEYPAD_UP);
}
void SettingsStep::DecrementValue() {
    HandleKeyPress(Keys::KEYPAD_DOWN);
}
void SettingsStep::HandleKeyPress(Keys key) {
    if (key == Keys::KEYPAD_DOWN)
        options->scrollDown();
    else if (key == Keys::KEYPAD_UP)
        options->scrollUp();
}    