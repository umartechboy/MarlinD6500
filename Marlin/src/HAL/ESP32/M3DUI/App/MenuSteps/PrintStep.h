#ifndef PRINT_STEP
#define PRINT_STEP
#include "../../Components/M3DUI.h"

class PrintStep : public MenuStep{
    public:
    String fileName;
    int minX = -1, maxX = -1, minY = -1, maxY = -1, maxZ = -1;
    int xOffset = 0;
    int yOffset = 0;
    PrintStep(MenuHost* host);
};

#endif
