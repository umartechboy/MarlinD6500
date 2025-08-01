#include "WifiListStep.h"
#include <SD.h>
#include <WiFi.h>
#include "esp_wifi.h"
#include "..\MenuApp.h"
#include "..\Images.h"

void gotPassword(String& password, MenuStep* owner){
    SERIAL_IMPL.printf("Got Password: %s\n", password.c_str());
    WifiListStep* This = (WifiListStep*)owner;
    This->connectingNetworkPassword = password;
    This->attemptConnectAtLoad = true;
}
void networkSelected(void* owner){
    WifiListStep* This = (WifiListStep*)owner;
    if (This->connectBegan)
        return;

    This->NeedsRedraw = true;
    StringListItem* selected = (StringListItem*)This->list->getSelected();
    This->connectingNetworkSSID = selected->ItemText;
    This->connectingNetworkSSID.trim();
    SERIAL_IMPL.printf("Network selected: %d\n", selected->ItemText.c_str());
    This->getttingPassword = true;
    textEntryStep.RequestTextEntry(This->connectingNetworkSSID, "Password", "", gotPassword, This);

}
static void selectionUpdated(void* caller, ListItem* selectedItem, int selectedIndex){
    WifiListStep* This = (WifiListStep*)caller;
    if (selectedIndex < 0) {
        This->NextStep = 0;
    }
    else {
        This->RetroNextStep = This->beginConnectStep;
    }
}
WifiListStep::WifiListStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkPaleYellow;
    BackColor = DarkPaleYellow;
    TextColor = ST7735_BLACK;
    Icon = &img_SD;
    RetroPreviousStep = &retroMainMenuStep;
    beginConnectStep = new DummyMenuStep(host);
    beginConnectStep->SetLoadCallBack(networkSelected, this);
    RetroNextStep = beginConnectStep;
    Title = "Available networks";
    NextActionString = "Select";
    TickPeriod = 50;
    list = new VerticalList(Host, "No SD Card");
    list->SetOnSelectionUpdated(this, selectionUpdated);
}
WifiListStep::~WifiListStep(){
    delete beginConnectStep;
    delete list;
}

void WifiListStep::Tick() {
    NeedsRedraw = true;
    if (Host->CurrentStep != this)
        return;
    if (!WiFi.isConnected() && millis() - connectionBeginAt > 10000 && !connectHasFailed){
        connectHasFailed = true;
        WiFi.disconnect();
        esp_wifi_disconnect();    // Abort any connection attempt
        esp_wifi_scan_stop();     // Abort any ongoing scan
    }
    if (!scanFinished){
        
        SERIAL_IMPL.printf("Scan Starting");
        foundNetworks = WiFi.scanNetworks();
        SERIAL_IMPL.printf(", found: : %d\n", foundNetworks);
        scanFinished = true;
        list->Clear();
    }
    else if (addedNetworks < foundNetworks){   
        String networkSSID = WiFi.SSID(addedNetworks);
        list->Add(new StringListItem(Host, 0, networkSSID, 0, 16));        
        SERIAL_IMPL.printf("Adding: %s\n", networkSSID.c_str());
        addedNetworks++;
    }
    else if (connectBegan) {
        RetroNextStep = 0;
    }
}
void WifiListStep::Paint(BufferedDisplay* g) {
    //Serial.printf("SD Menu Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    if(!connectBegan) {
        if (foundNetworks == 0){
            if (scanFinished)
                centerString(g, "No networks found", Host->appWidth() / 2, retroTitleSectionHeight +Host->appHeight() / 2);
            else
                centerString(g, "Scanning...", Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2);
        }
        else {
            list->Paint(g, 0, 0, g->width(), g->height(), TextColor);
        }
    }
    else {
        if (WiFi.isConnected()){
            centerString(g, "Connected to", Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2 - 12);
            centerString(g, WiFi.SSID().c_str(), Host->appWidth() / 2, retroTitleSectionHeight + Host->appHeight() / 2 + 12);
        }
        else {
            if (millis() - connectionBeginAt > 10000)
                centerString(g, "Failed to connect", Host->appWidth() / 2,  retroTitleSectionHeight + Host->appHeight() / 2 - 7);
            else
                centerString(g, "Connecting", Host->appWidth() / 2,  retroTitleSectionHeight + Host->appHeight() / 2 - 7);

            centerString(g, connectingNetworkSSID.c_str(), Host->appWidth() / 2,  retroTitleSectionHeight + Host->appHeight() / 2 + 7);
        }
    }
}
void WifiListStep::IncrementValue() {
    HandleKeyPress(Keys::KEYPAD_UP);
}
void WifiListStep::DecrementValue() {
    HandleKeyPress(Keys::KEYPAD_DOWN);
}
void WifiListStep::HandleKeyPress(Keys key) {
    if (key == Keys::KEYPAD_DOWN)
        list->scrollDown();
    else if (key == Keys::KEYPAD_UP)
        list->scrollUp();
}    

void WifiListStep::LoadComplete(){    
    if (getttingPassword){
        getttingPassword = false;
        if(attemptConnectAtLoad) {
            connectBegan = true;
            SERIAL_IMPL.printf("Connecting: %s with %s\n", connectingNetworkSSID.c_str(), connectingNetworkPassword.c_str()); // Not needed
            esp_wifi_disconnect();
            esp_wifi_scan_stop();
            WiFi.begin(connectingNetworkSSID, connectingNetworkPassword);
            connectionBeginAt = millis();
        }
        else {
            SERIAL_IMPL.println("Connect cancelled");
        }
    }
    else {
        list->EmptyString = "Scanning..."; // Not needed
        scanFinished = false;
        foundNetworks = 0;
        addedNetworks = 0;
        connectHasFailed = false;
        connectBegan = false;
        attemptConnectAtLoad = false;
        list->Clear();
    }
}
void WifiListStep::UnloadComplete(){   
    connectingNetworkPassword = ""; // in case the text entry returns by cancellation, 
}
