#ifndef __PRINTER_INFO_STEP__
#define __PRINTER_INFO_STEP__

#include "..\..\Components\M3DUI.h"
#define EnablerVersion "D8500 V3.1"

class PrinterInfoStep:public MenuStep
{
private:
public:
    PrinterInfoStep(MenuHost* host);
    void Paint(BufferedDisplay* g) override;
    void HandleKeyPress(Keys key) override;  
    void Tick() override;   
    void LoadComplete() override;
};

#endif