#include "FilamentChange.h"
#include "..\..\Hardware\MarlinSpecific.h"
#include "..\Bitmaps.h"
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>

FilamentChangeStep::FilamentChangeStep(MenuHost* host, int index):MenuStep(host)
{
    filamentIndex = index;
    TextColor = ST7735_WHITE;
    Icon = &bmp_ChangeFilament;
    TickPeriod = 50;
}

FilamentChangeStep::~FilamentChangeStep()
{
}

void FilamentChangeStep::Tick()
{
    NeedsRedraw = true;
}

void FilamentChangeStep::LoadComplete(){
    SERIAL_IMPL.printf("Begin Change Filament %d\n", filamentIndex);
    writeTemp(filamentIndex, preHeatTemp);
}

void FilamentChangeStep::UnloadBegin(){
    SERIAL_IMPL.println("Drop 5mm");
    writeTemp(filamentIndex, 0);
}

void FilamentChangeStep::Paint(BufferedDisplay* g){            
    //SERIAL_IMPL.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    Color red = Color(DarkRed);
    int a = (readTemp(0) / preHeatTemp) * 255.0F;
    if (a > 255) a = 255;
    else if (a < 0) a = 0;
    red.a(a);
    red.BlendOn(DarkOceanBlue);
    BackColor = red;
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    if (readTemp(filamentIndex) < preHeatTemp){
        
        g->setFont(&FreeSans9pt7b);
        centerString(g, "Heating up...", g->width() / 2, g->height() / 2 - 10);
        g->setFont();
        String tempStatus = String("(") + String(readTemp(filamentIndex), 0) + "/" + String(preHeatTemp, 0) + String(")");
        centerString(g, tempStatus.c_str(), g->width() / 2, g->height() / 2 + 10);
    }
    else {
        int trSz = 12;
        int yo = -20;
        g->setFont(&FreeSans12pt7b);
        centerLeftString(g, "Unload", 5, g->height() / 2 + yo);
        g->fillTriangle(
            g->width() - 5 - trSz / 2, g->height() / 2 + yo - trSz / 2, 
            g->width() - 5 - trSz, g->height() / 2 + yo + trSz / 2, 
            g->width() - 5, g->height() / 2 + yo + trSz / 2,
            TextColor
        );
        
        yo = +20;
        centerLeftString(g, "Load", 5, g->height() / 2 + yo);
        g->fillTriangle(
            g->width() - 5 - trSz / 2, g->height() / 2 + yo + trSz / 2, 
            g->width() - 5 - trSz, g->height() / 2 + yo - trSz / 2, 
            g->width() - 5, g->height() / 2 + yo - trSz / 2,
            TextColor
        );
        
    }
    g->setFont();
}

void FilamentChangeStep::HandleKeyUp(Keys key) {
    if (key == Keys::KEYPAD_UP){
        SERIAL_IMPL.println("Retract");
    }
    else if (key == Keys::KEYPAD_DOWN){
        SERIAL_IMPL.println("Extrude");
    }
    else if (key == Keys::KEYPAD_RIGHT){
        Host->GotoNextStep(); // back to filament settings
    }
}