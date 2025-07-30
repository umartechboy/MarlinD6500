

#include "../../../MarlinCore.h"
#include <Arduino.h>
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7789
#include "Hardware/BufferedDisplay.h"
#include "App/MenuApp.h"
#ifndef __MARLIN_FIRMWARE__ // Marlin HAL handles the digital IO setup
#include "Hardware/digitalIO.h"
#endif



#define TFT_CS         17
#define TFT_DC         4
#define TFT_RST        200

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC,TFT_RST);

void TFT_startWrite()
{
    tft.startWrite();
}
void TFT_setAddressWindow(int x, int y, int width, int height)
{
    tft.setAddrWindow(x, y, width, height);
}
void TFT_writePixels(uint16_t *colors, uint16_t count)
{
    tft.writePixels(colors, count, false, false);
}
void TFT_endWrite()
{
    tft.endWrite();
}
BufferedDisplay* bTft;

#ifdef __MARLIN_FIRMWARE__
void UISetup()
#else
void setup()
#endif
{
#ifndef __MARLIN_FIRMWARE__
  Serial.begin(115200);
#endif
    // for (int i = 0; i < 5; i++){
    //     delay(1000);
    //     Serial.print(".");
    // }
SERIAL_IMPL.println("UI Starting up...");
#ifndef __MARLIN_FIRMWARE__
  beginDigitalIO(); // Initialize digital IO
#endif
  digitalWrite(TFT_RST, 0);
  delay(1);
  digitalWrite(TFT_RST, 1);
  delay(1);

  tft.initR(INITR_144GREENTAB);
  tft.fillScreen(ST77XX_WHITE);
  tft.setRotation(3);
  bTft = new BufferedDisplay(tft, TFT_startWrite, TFT_setAddressWindow, TFT_writePixels, TFT_endWrite);

  // initialize the menu step linkage
  BeginApp(); 
}

static bool inLoop = false;
#ifdef __MARLIN_FIRMWARE__
void UILoop()
#else
void loop() 
#endif
{  
  if (inLoop)
    return;
  inLoop = true;
  menuHost.Loop(bTft);
  inLoop = false;
}