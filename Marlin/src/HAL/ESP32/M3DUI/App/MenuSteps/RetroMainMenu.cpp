#include "RetroMainMenu.h"
#include "..\MenuApp.h"
#include "..\Images.h"

RetroMainMenuStep::RetroMainMenuStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkPaleYellow;
    BackColor = DarkPaleYellow;
    TextColor = ST7735_BLACK;
    Icon = &img_SD;
    TickPeriod = 50;
    list = new VerticalList(Host, 14, 128, "No SD Card");
}
RetroMainMenuStep::~RetroMainMenuStep(){
    delete list;
}
void RetroMainMenuStep::Tick() {
    NeedsRedraw = true;
}
void RetroMainMenuStep::Paint(BufferedDisplay* g) {
    //Serial.printf("SD Menu Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    list->Paint(g, TextColor);
}
void RetroMainMenuStep::IncrementValue() {
    HandleKeyUp(Keys::KEYPAD_UP);
}
void RetroMainMenuStep::DecrementValue() {
    HandleKeyUp(Keys::KEYPAD_DOWN);
}
void RetroMainMenuStep::HandleKeyUp(Keys key) {
    if (key == Keys::KEYPAD_DOWN)
        list->scrollDown();
    else if (key == Keys::KEYPAD_UP)
        list->scrollUp();
    else if (key == Keys::KEYPAD_RIGHT)
        Host->GotoNextStep();
    else if (key == Keys::KEYPAD_LEFT)
        Host->GotoPreviousStep();
}    

void RetroMainMenuStep::LoadComplete(){    
}
