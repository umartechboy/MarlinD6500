#include "PrintPositionStep.h"
#include "..\MenuApp.h"
#include "..\Bitmaps.h"
#include "..\..\Hardware\MarlinSpecific.h"

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
        idleScreenStep.minX + idleScreenStep.xOffset, 
        idleScreenStep.maxY + idleScreenStep.yOffset,
        idleScreenStep.maxX - idleScreenStep.minX,
        idleScreenStep.maxY - idleScreenStep.minY);
    g->fillRect(
        transformX(idleScreenStep.minX + idleScreenStep.xOffset), transformY(idleScreenStep.maxY + idleScreenStep.yOffset), 
        scaleX(idleScreenStep.maxX - idleScreenStep.minX), scaleY(idleScreenStep.maxY - idleScreenStep.minY),
        ST7735_WHITE);
}
void PrintPositionStep::HandleKeyUp(Keys key) {
    switch (key)
    {
        case Keys::KEYPAD_RIGHT: idleScreenStep.xOffset += inc; break;
        case Keys::KEYPAD_LEFT: idleScreenStep.xOffset -= inc; break;
        case Keys::KEYPAD_DOWN: idleScreenStep.yOffset -= inc; break;
        case Keys::KEYPAD_UP: idleScreenStep.yOffset += inc; break;
        case Keys::KEYPAD_UP_RIGHT: HandleKeyUp(Keys::KEYPAD_UP); HandleKeyUp(Keys::KEYPAD_RIGHT); break;
        case Keys::KEYPAD_UP_LEFT:  HandleKeyUp(Keys::KEYPAD_UP); HandleKeyUp(Keys::KEYPAD_LEFT); break; 
        case Keys::KEYPAD_DOWN_RIGHT:   HandleKeyUp(Keys::KEYPAD_DOWN); HandleKeyUp(Keys::KEYPAD_RIGHT); break; 
        case Keys::KEYPAD_DOWN_LEFT:    HandleKeyUp(Keys::KEYPAD_DOWN); HandleKeyUp(Keys::KEYPAD_LEFT); break; 
    }
    if (idleScreenStep.minX + idleScreenStep.xOffset < inc) idleScreenStep.xOffset = inc - idleScreenStep.minX;
    if (idleScreenStep.maxX + idleScreenStep.xOffset > (bedWidth - inc)) idleScreenStep.xOffset = (bedWidth - inc) - idleScreenStep.maxX;
    if (idleScreenStep.minY + idleScreenStep.yOffset < inc) idleScreenStep.yOffset = inc - idleScreenStep.minY;
    if (idleScreenStep.maxY + idleScreenStep.yOffset > (bedHeight - inc)) idleScreenStep.yOffset = (bedHeight - inc) - idleScreenStep.maxY;
    NeedsRedraw = true;
}
void PrintPositionStep::UnloadBegin(){
    // Apply the offset
    // This happen even if the step is to the previous step
    // We need to take care of this limitation in File preview
    String offsetCom = String("M206 X") + String(-idleScreenStep.xOffset) + String("Y") + String(-idleScreenStep.yOffset);
    idleScreenStep.printStatus = PrintStatus::FileToPrint;
    enqueueComs({offsetCom});
}