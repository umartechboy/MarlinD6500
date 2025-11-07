#include "PrintPositionStep.h"
#include "..\MenuApp.h"
#include "..\Images.h"
#include "..\..\Hardware\MarlinSpecific.h"

PrintPositionStep::PrintPositionStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkPurple;
    BackColor = DarkPurple;
    TextColor = ST7735_WHITE;
    Icon = &img_MovePrint;
    Title = "Print Position";
    RetroIcon = &img_RetroPrintPosition;
    PreviousStep = &filePreviewStep;
    NextStep = &mainScreenStep; // If it gets triggered, the menu locks the screen
    RetroPreviousStep = &filePreviewStep;
    RetroNextStep = &mainScreenStep;
    NextActionString = "Print";
}

int PrintPositionStep::scaleX(int v) {
    return round((115.0F * v) / (float)bedWidth);
}
int PrintPositionStep::scaleY(int v) {
    return round((115.0F * v) / (float)bedWidth);
}
int PrintPositionStep::transformX(int v) {
    return scaleX(v) + 6;
}
int PrintPositionStep::transformY(int v) {
    int gHeight = scaleY(bedHeight);
    return (128 / 2) + gHeight / 2 - scaleY(v);
}
void PrintPositionStep::Paint(BufferedDisplay* g) {
    g->fillScreen(BackColor);
    g->setFont();
    g->setTextColor(TextColor);
    g->drawRect(transformX(0), transformY(bedHeight), scaleX(bedWidth), scaleY(bedHeight), ST7735_RED);
    g->drawRect(transformX(sideMargin), transformY(bedHeight - sideMargin), scaleX(bedWidth - 2 * sideMargin), scaleY(bedHeight - 2 * sideMargin), ST7735_YELLOW);
    Serial.printf("fill: %dx%d, %dx%d\n",
        mainScreenStep.minX + mainScreenStep.xOffset, 
        mainScreenStep.maxY + mainScreenStep.yOffset,
        mainScreenStep.maxX - mainScreenStep.minX,
        mainScreenStep.maxY - mainScreenStep.minY);
    g->fillRect(
        transformX(mainScreenStep.minX + mainScreenStep.xOffset), transformY(mainScreenStep.maxY + mainScreenStep.yOffset), 
        scaleX(mainScreenStep.maxX - mainScreenStep.minX), scaleY(mainScreenStep.maxY - mainScreenStep.minY),
        ST7735_WHITE);
}
void PrintPositionStep::HandleKeyPress(Keys key) {
    switch (key)
    {
        case Keys::KEYPAD_RIGHT: mainScreenStep.xOffset += inc; break;
        case Keys::KEYPAD_LEFT: mainScreenStep.xOffset -= inc; break;
        case Keys::KEYPAD_DOWN: mainScreenStep.yOffset -= inc; break;
        case Keys::KEYPAD_UP: mainScreenStep.yOffset += inc; break;
        case Keys::KEYPAD_UP_RIGHT: HandleKeyPress(Keys::KEYPAD_UP); HandleKeyPress(Keys::KEYPAD_RIGHT); break;
        case Keys::KEYPAD_UP_LEFT:  HandleKeyPress(Keys::KEYPAD_UP); HandleKeyPress(Keys::KEYPAD_LEFT); break; 
        case Keys::KEYPAD_DOWN_RIGHT:   HandleKeyPress(Keys::KEYPAD_DOWN); HandleKeyPress(Keys::KEYPAD_RIGHT); break; 
        case Keys::KEYPAD_DOWN_LEFT:    HandleKeyPress(Keys::KEYPAD_DOWN); HandleKeyPress(Keys::KEYPAD_LEFT); break; 
    }
    if (mainScreenStep.minX + mainScreenStep.xOffset < inc) mainScreenStep.xOffset = inc - mainScreenStep.minX;
    if (mainScreenStep.maxX + mainScreenStep.xOffset > (bedWidth - inc)) mainScreenStep.xOffset = (bedWidth - inc) - mainScreenStep.maxX;
    if (mainScreenStep.minY + mainScreenStep.yOffset < inc) mainScreenStep.yOffset = inc - mainScreenStep.minY;
    if (mainScreenStep.maxY + mainScreenStep.yOffset > (bedHeight - inc)) mainScreenStep.yOffset = (bedHeight - inc) - mainScreenStep.maxY;
    NeedsRedraw = true;
}
void PrintPositionStep::LoadBegin(){
    enqueueComs({"M206 X0 Y0"}); // Reset any previous offset
}
void PrintPositionStep::UnloadBegin(){
    // Apply the offset
    // This happen even if the step is to the previous step
    // We need to take care of this limitation in File preview
    String offsetCom = String("M206 X") + String(-mainScreenStep.xOffset) + String(" Y") + String(-mainScreenStep.yOffset);
    mainScreenStep.printStatus = PrintStatus::FileToPrint;
    enqueueComs({offsetCom, "M500"});
}