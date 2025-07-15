#include "PrinterInfoStep.h"
#include "..\..\Hardware\MarlinSpecific.h"
#include "..\Bitmaps.h"
#include "Fonts\FreeMono12pt7b.h"
#include "Fonts\FreeMono9pt7b.h"
#include "..\..\..\..\..\module\printcounter.h"
#include <WiFi.h>

PrinterInfoStep::PrinterInfoStep(MenuHost* host):MenuStep(host)
{
    TextColor = ST7735_WHITE;
    BackColor = DarkRed;
    Icon = &bmp_M3D;
    TickPeriod = 1000;
}
#define printInfoPair(a, b) { g->SetOpacity(60); centerRightString(g, (a), div - px, y); g->SetOpacity(100); centerLeftString(g, (b), div + px, y); y += 11; }

void PrinterInfoStep::Paint(BufferedDisplay* g){            
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    uint8_t opBkp = g->GetOpacity();
    g->setFont(&FreeMono9pt7b);
    int y = 9;
    centerString(g, "M3D Enabler", g->width() / 2, y);  y += 15;
    g->setFont();  
    centerString(g, "D8500 V1.0", g->width() / 2, y); y += 8;
    uint8_t lineMargin = 10;
    uint8_t tempSectionHeight = 20;
    g->SetOpacity(10);
    for (int i =0; i < 5; i++)
        g->drawLine(lineMargin + i * 2, y, g->width() - lineMargin - i * 2, y, TextColor);
    y += 10;
    int div = 41;
    int px = 2;
    printInfoPair("WiFi", WiFi.isConnected() ? WiFi.SSID().c_str():("--"));
    printInfoPair("Status", WiFi.isConnected() ? "Connected":"Disconnected");
    printInfoPair("Host" ,  WiFi.getHostname());
    printInfoPair("IP", WiFi.isConnected() ? WiFi.localIP().toString().c_str():"--");
    printInfoPair("AP", WiFi.softAPSSID().c_str());

    char str[30] = "";
    duration_t(print_job_timer.getStats().printTime).toDigital(str, true);
    printInfoPair("Life" ,  str);
    printInfoPair("Jobs", String(print_job_timer.getStats().totalPrints).c_str());

    g->SetOpacity(opBkp);

}
void PrinterInfoStep::Tick(){
    NeedsRedraw = true;
}

void PrinterInfoStep::HandleKeyUp(Keys key) {
    if (key == Keys::KEYPAD_RIGHT){
        Host->GotoNextStep(); // back to options menu
    }
}
