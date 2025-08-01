#ifndef __WIFI_LIST_STEP__
#define __WIFI_LIST_STEP__

#include "..\..\Components\M3DUI.h"

class WifiListStep: public MenuStep {
private:
    bool scanFinished = false;
    int foundNetworks = 0;
    int addedNetworks = 0;
    long connectionBeginAt = 0;
    bool connectHasFailed = false;
public:
    VerticalList* list;
    WifiListStep(MenuHost* host);
    ~WifiListStep();
    void LoadComplete() override;
    void UnloadComplete() override;
    void Tick() override;
    void Paint(BufferedDisplay* g) override;
    void IncrementValue() override;
    void DecrementValue() override;
    void HandleKeyPress(Keys key) override;
    String connectingNetworkSSID;
    String connectingNetworkPassword;
    DummyMenuStep* beginConnectStep;
    bool connectBegan = false;
    bool getttingPassword = false;
    bool attemptConnectAtLoad = false;
};

#endif