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
    enqueueComs({"G91", "G1 Z5", "G90"});
    writeTemp(filamentIndex, preHeatTemp);
    donePreHeating = false;
    doneExtruding = false;
    doneRetracting = false;
}

void FilamentChangeStep::UnloadBegin(){
    SERIAL_IMPL.println("Drop 5mm");
    enqueueComs({"G91", "G1 Z-5", "G90"});
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
    int opBkp = g->GetOpacity();
    if (readTemp(filamentIndex) < preHeatTemp && !donePreHeating){
        
        g->setFont(&FreeSans9pt7b);
        centerString(g, "Heating up...", g->width() / 2, g->height() / 2 - 10);
        g->setFont();
        String tempStatus = String("(") + String(readTemp(filamentIndex), 0) + "/" + String(preHeatTemp, 0) + String(")");
        centerString(g, tempStatus.c_str(), g->width() / 2, g->height() / 2 + 10);
    }
    else {
        donePreHeating = true; // latch preheat check
        int trSz = 12;
        int yo = -20;
        g->setFont(&FreeSans12pt7b);
        if (doneRetracting)
            g->SetOpacity(50);
        else
            g->SetOpacity(100);
        centerLeftString(g, "Unload", 5, g->height() / 2 + yo);
        g->fillTriangle(
            g->width() - 5 - trSz / 2, g->height() / 2 + yo - trSz / 2, 
            g->width() - 5 - trSz, g->height() / 2 + yo + trSz / 2, 
            g->width() - 5, g->height() / 2 + yo + trSz / 2,
            TextColor
        );
        
        if (doneExtruding)
            g->SetOpacity(50);
        else
            g->SetOpacity(100);
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
    g->SetOpacity(opBkp);
}

void FilamentChangeStep::HandleKeyUp(Keys key) {
    if (key == Keys::KEYPAD_UP){
        if (donePreHeating && !doneRetracting){
            SERIAL_IMPL.println("Retract");        
            enqueueComs({"M83", "G1 E5 F500", "G1 E-100 F4000", "G1 E-100", "G1 E-100", "G1 E-100", "G1 E-100", "G1 E-100", "M82"});
            doneRetracting = true;
        }
    }
    else if (key == Keys::KEYPAD_DOWN){
        if (donePreHeating && !doneExtruding){
            SERIAL_IMPL.println("Extrude");
            enqueueComs({"M83", "G1 E100 F4000", "G1 E100", "G1 E100", "G1 E100", "G1 E10", "G1 E100 F100", "M82"});
            doneExtruding = true;
        }
    }
    else if (key == Keys::KEYPAD_RIGHT){
        Host->GotoNextStep(); // back to filament settings
    }
}