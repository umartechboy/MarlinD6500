#include "SensorInfoStep.h"
#include "..\Images.h"
#include "..\MenuApp.h"
#include "Fonts\FreeMono9pt7b.h"
#include "Fonts\FreeMono12pt7b.h"
#include "..\..\Hardware\MarlinSpecific.h"

SensorInfoStep::SensorInfoStep(MenuHost* host):MenuStep(host)
{
    TextColor = ST7735_BLACK;
    BackColor = ST7735_WHITE;
    Icon = &img_M3D;
    TickPeriod = 50;
    NextStep = &toolsMenuStep;
    Title = "Sensor info";
    RetroPreviousStep = 0; // prohibit going to back 
}
extern float touchOnADCKeyPad_getKeyIntensity(Keys key);
extern Keys touchOnADCKeyPad_getKey();

void SensorInfoStep:: Tick(){
    if (autoBackTimeoutStartAt > 0){
        timeLeft = 5000 - (millis() - autoBackTimeoutStartAt);
        if (timeLeft <= 0) {
            autoBackTimeoutStartAt = -1;
            timeLeft = -1;
            RetroPreviousStep = &settingsStep;
            Host->GotoPreviousStep();
        }
    }
    NeedsRedraw = true;
}
Keys keyMap [] = {
    Keys::KEYPAD_UP_LEFT, Keys::KEYPAD_UP, Keys::KEYPAD_UP_RIGHT,
    Keys::KEYPAD_LEFT, Keys::KEYPAD_MIDDLE, Keys::KEYPAD_RIGHT,
    Keys::KEYPAD_DOWN_LEFT, Keys::KEYPAD_DOWN, Keys::KEYPAD_DOWN_RIGHT,
};

extern float readProbeAnalog();

void SensorInfoStep::Paint(BufferedDisplay* g){            
    //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);

    int bz = 20;
    Keys currentKey = touchOnADCKeyPad_getKey();
    for (int yi = 0; yi < 3; yi++){
        for (int xi = 0; xi < 3; xi++){
            int x = Host->appWidth() / 2 - (bz * 3) / 2 + xi * bz;
            int y = Host->appHeight() / 2 - (bz * 3) / 2 + yi * bz + 5;
            Keys key = keyMap[xi + yi * 3];
            
            float intensityFac = touchOnADCKeyPad_getKeyIntensity(key) / 10.0F;
            if (intensityFac > 1) intensityFac = 1; if (intensityFac < 0) intensityFac = 0;
            g->drawRect(x, y, bz, bz, rgb(184, 184, 184));
            int opBkp = g->GetOpacity();
            g->SetOpacity(100 * intensityFac);
            g->fillRect(x, y, bz, bz, ST7735_RED);
            g->SetOpacity(opBkp);
            if (key == currentKey){
                g->drawRect(x, y, bz, bz, rgb(255, 238, 0));
            }
        }
    }
    g->setFont();
    if (timeLeft > 0){
        String tStr = String("Leaving in ") + String(timeLeft / 1000) + String("s");
        centerString(g, tStr.c_str(), Host->appWidth() / 2,  retroTitleSectionHeight + Host->appHeight() - 24);
    }
    else {
        String tStr = String(keysToBack) + String(" keys to go back");
        centerString(g, tStr.c_str(), Host->appWidth() / 2,  retroTitleSectionHeight + Host->appHeight() - 24);
    }

    int pHeight = readProbeAnalog();
    g->fillRect(g->width() - 3, g->height() - pHeight, 3, pHeight, ST7735_RED);

    String tempStr = String(readTemp1()) + String(", ") + String(readTemp1()) + String(" C");
    centerString(g, tempStr.c_str(), Host->appWidth() / 2,  retroTitleSectionHeight + 7);
    
    // lets draw three 
    int pinx [] = {X_STOP_PIN, Y_STOP_PIN, Z_STOP_PIN};
    for (int i = 0; i < 3; i++){
        int y = Host->appHeight() / 2 - (bz * 3) / 2 + i * bz + 5;
        int x = 0;
        g->fillRect(x, y, bz, bz, digitalRead(pinx[i]) == HIGH ? ST7735_GREEN : ST7735_RED);
        g->drawRect(x, y, bz, bz, rgb(184, 184, 184));
        centerString(g, String((char)('X' + i)).c_str(), x + bz / 2, y + bz / 2 - 3);
    }

    g->setFont(0);
}


void SensorInfoStep::HandleKeyPress(Keys key) {
    NeedsRedraw = true;
    
    if (keysToBack == 0 && autoBackTimeoutStartAt == -1){
        autoBackTimeoutStartAt = millis();
    }
    autoBackTimeoutStartAt = millis();
    keysToBack--;
    if (keysToBack < 0) keysToBack = 0;
}
void SensorInfoStep::LoadBegin(){
    enqueueComs({"M401"}); // Deploy probe
    RetroPreviousStep = 0;
    keysToBack = 5;
}
void SensorInfoStep::UnloadBegin(){
    enqueueComs({"M402"}); // Stow probe

}
bool SensorInfoStep::CanJumpToMainMenu(){
    return false;
}
