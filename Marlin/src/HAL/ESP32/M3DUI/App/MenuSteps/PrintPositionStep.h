#ifndef PRINT_POSITION_STEP
#define PRINT_POSITION_STEP

#include "..\..\Components\M3DUI.h"

class PrintPositionStep : public MenuStep{
    public:
    static const int bedWidth = 220;
    static const int bedHeight = 150;
    int inc = 5;
    int sideMargin = 10;
    PrintPositionStep(MenuHost* host);
    static int scaleX(int v);
    static int scaleY(int v);
    static int transformX(int v);
    static int transformY(int v);
    void Paint(BufferedDisplay* g) override;
    void HandleKeyUp(Keys key) override;
};

#endif