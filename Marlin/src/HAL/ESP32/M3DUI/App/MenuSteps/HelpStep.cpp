#include "HelpStep.h"
#include "..\Images.h"
#include "..\MenuApp.h"
#include "Fonts\FreeMono9pt7b.h"
#include "Fonts\FreeMono12pt7b.h"

HelpStep::HelpStep(MenuHost* host):MenuStep(host)
{
    TextColor = ST7735_BLACK;
    BackColor = ST7735_WHITE;
    Icon = &img_M3D;
    TickPeriod = 1000;
    NextStep = &toolsMenuStep;
    Title = "Find help";
    RetroPreviousStep = &retroMainMenuStep;
}
void HelpStep::Paint(BufferedDisplay* g){            
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    if (displayingQR)
        img_CommunityQR.Draw(g, (Host->appWidth() - img_CommunityQR.width()) / 2, Host->appTop() + (Host->appHeight() - img_CommunityQR.height()) / 2);
    else {
        g->setTextColor(TextColor);
        g->setFont(&FreeMono9pt7b);
        centerString(g, "http://bit.ly/", Host->appWidth() / 2, Host->appTop() + Host->appHeight() / 2 - 10);
        g->setFont(&FreeMono12pt7b);
        centerString(g, "3VxBlVO", Host->appWidth() / 2, Host->appTop() + Host->appHeight() / 2 + 10);
    }    
    g->setFont(0);
}

void HelpStep::HandleKeyPress(Keys key) {
    displayingQR = !displayingQR;
    NeedsRedraw = true;
}
