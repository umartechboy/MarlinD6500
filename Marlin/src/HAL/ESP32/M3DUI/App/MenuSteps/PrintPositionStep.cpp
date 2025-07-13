#include "PrintPositionStep.h"
#include "..\MenuApp.h"
#include "..\Bitmaps.h"

PrintPositionStep::PrintPositionStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkPurple;
    BackColor = DarkPurple;
    TextColor = ST7735_WHITE;
    Icon = &bmp_MovePrint;
}
int PrintPositionStep::scaleX(int v) {
    return round((127.0F * v) / (float)bedWidth);
}
int PrintPositionStep::scaleY(int v) {
    return round((127.0F * v) / (float)bedWidth);
}
int PrintPositionStep::transformX(int v) {
    return scaleX(v);
}
int PrintPositionStep::transformY(int v) {
    float fo = (127 * bedHeight) / bedWidth;
    return (127 - fo) / 2 + fo - scaleX(v);
}
void PrintPositionStep::Paint(BufferedDisplay* g) {
    g->fillScreen(BackColor);
    g->setFont();
    g->setTextColor(TextColor);
    centerString(g, "Use movement keys to", g->width() / 2, 7);
    centerString(g, "adjust print position", g->width() / 2, 121);
    g->drawRect(transformX(0), transformY(bedHeight), scaleX(bedWidth), scaleY(bedHeight), ST7735_RED);
    g->drawRect(transformX(sideMargin), transformY(bedHeight - sideMargin), scaleX(bedWidth - 2 * sideMargin), scaleY(bedHeight - 2 * sideMargin), ST7735_YELLOW);
    Serial.printf("fill: %dx%d, %dx%d\n",
        printStep.minX + printStep.xOffset, 
        printStep.maxY + printStep.yOffset,
        printStep.maxX - printStep.minX,
        printStep.maxY - printStep.minY);
    g->fillRect(
        transformX(printStep.minX + printStep.xOffset), transformY(printStep.maxY + printStep.yOffset), 
        scaleX(printStep.maxX - printStep.minX), scaleY(printStep.maxY - printStep.minY),
        ST7735_WHITE);
}
void PrintPositionStep::HandleKeyUp(Keys key) {
    switch (key)
    {
        case Keys::KEYPAD_RIGHT: printStep.xOffset += inc; break;
        case Keys::KEYPAD_LEFT: printStep.xOffset -= inc; break;
        case Keys::KEYPAD_DOWN: printStep.yOffset -= inc; break;
        case Keys::KEYPAD_UP: printStep.yOffset += inc; break;
    }
    if (printStep.minX + printStep.xOffset < inc) printStep.xOffset = inc - printStep.minX;
    if (printStep.maxX + printStep.xOffset > (bedWidth - inc)) printStep.xOffset = (bedWidth - inc) - printStep.maxX;
    if (printStep.minY + printStep.yOffset < inc) printStep.yOffset = inc - printStep.minY;
    if (printStep.maxY + printStep.yOffset > (bedHeight - inc)) printStep.yOffset = (bedHeight - inc) - printStep.maxY;
    NeedsRedraw = true;
}