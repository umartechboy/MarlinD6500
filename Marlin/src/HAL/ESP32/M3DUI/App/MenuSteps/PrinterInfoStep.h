#ifndef __PRINTER_INFO_STEP__
#define __PRINTER_INFO_STEP__

#include "..\..\Components\M3DUI.h"

class PrinterInfoStep:public MenuStep
{
private:
public:
    PrinterInfoStep(MenuHost* host);
    void Paint(BufferedDisplay* g) override;
    void HandleKeyPress(Keys key) override;  
    void Tick() override;   
};

#endif