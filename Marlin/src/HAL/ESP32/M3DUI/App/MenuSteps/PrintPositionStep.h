#ifndef PRINT_POSITION_STEP
#define PRINT_POSITION_STEP

#include "..\..\Components\M3DUI.h"
#include "..\..\..\..\..\..\Configuration.h"

class PrintPositionStep : public MenuStep{
    public:
    static const int bedWidth = X_BED_SIZE;
    static const int bedHeight = Y_BED_SIZE;
    int inc = 5;
    int sideMargin = 10;
    PrintPositionStep(MenuHost* host);
    static int scaleX(int v);
    static int scaleY(int v);
    static int transformX(int v);
    static int transformY(int v);
    void Paint(BufferedDisplay* g) override;
    void HandleKeyPress(Keys key) override;
    void UnloadBegin() override;
};

#endif