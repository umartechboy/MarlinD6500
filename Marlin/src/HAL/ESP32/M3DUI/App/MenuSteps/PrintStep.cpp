#include "PrintStep.h"
#include "../Bitmaps.h"

PrintStep::PrintStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkPurple;
    BackColor = DarkPurple;
    TextColor = ST7735_WHITE;
    Icon = &bmp_StartPrint;
}