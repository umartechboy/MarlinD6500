#include "IdleScreenStep.h"
#include "..\Bitmaps.h"
#include <Fonts/FreeSans12pt7b.h>
#include "..\..\Hardware\MarlinSpecific.h"
#include "..\MenuApp.h"

IdleScreenStep::IdleScreenStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkRed;
    BackColor = DarkRed;
    TextColor = ST7735_WHITE;            
    Icon = &bmp_Home;
    TickPeriod = 250;
}
void IdleScreenStep::Tick() {
    NeedsRedraw = true;
}
void IdleScreenStep::Paint(BufferedDisplay* g) {
    
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    if(CurrentPrintStatus == PrintStatus::Idle){
        g->setFont(&FreeSans12pt7b);
        centerString(g, "Ready!", g->width() / 2, g->height() / 2);
        uint8_t opBkp = g->GetOpacity();
        uint8_t lineMargin = 10;
        uint8_t tempSectionHeight = 20;
        g->SetOpacity(10);
        for (int i =0; i < 5; i++)
            g->drawLine(lineMargin + i * 2, g->height() - tempSectionHeight, g->width() - lineMargin - i * 2, g->height() - tempSectionHeight, TextColor);
        g->SetOpacity(opBkp);
        g->drawLine(g->width() / 2, g->height() - tempSectionHeight, g->width() / 2, g->height(), g->readPixel(g->width() / 2, g->height() - tempSectionHeight));
        
        g->setFont();
        String temp1 = String(readTemp1(), 1) + " C";
        String temp2 = String(readTemp2(), 1) + " C";
        int16_t tw, th;
        centerString(g, temp1.c_str(), g->width() / 4, g->height() + 1 - tempSectionHeight / 2, &tw, &th);
        g->drawCircle(g->width() / 4 + tw / 2 - 9, g->height() + 1 - tempSectionHeight + 5, 1, TextColor);
        centerString(g, temp2.c_str(), (3 * g->width()) / 4, g->height() + 1 - tempSectionHeight / 2, &tw, &th);
        g->drawCircle((3 * g->width()) / 4 + tw / 2 - 9, g->height() + 1 - tempSectionHeight + 5, 1, TextColor);
    }
}
void IdleScreenStep::HandleKeyUp(Keys key){
    if (key == KEYPAD_LEFT){
        Host->GotoPreviousStep();
    }
    else if (key == KEYPAD_RIGHT){
        Host->GotoNextStep();
    }
}