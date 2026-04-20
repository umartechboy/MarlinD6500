

#include "../../../MarlinCore.h"
#include <Arduino.h>
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7789
#include "Hardware/BufferedDisplay.h"
#include "App/MenuApp.h"
#ifndef __MARLIN_FIRMWARE__ // Marlin HAL handles the digital IO setup
#include "Hardware/digitalIO.h"
#endif


#include <SPI.h>
class Adafruit_ST7735s: public Adafruit_ST7735 {
  public: 
  Adafruit_ST7735s(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst):
    Adafruit_ST7735(spiClass, cs, dc, rst){}
  void ST7735sPatch(){
    _height = ST7735_TFTHEIGHT_128;
    _width = ST7735_TFTWIDTH_128;    
    uint8_t madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST7735_MADCTL_BGR;
    sendCommand(ST77XX_MADCTL, &madctl, 1);
  }
};
Adafruit_ST7735s tft = Adafruit_ST7735s(&SPI, TFT_CS, TFT_DC, TFT_RST);

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

extern void DrawSplash(BufferedDisplay* g);
bool displayHasInit = false;
void InitDisplayBasic(){
  if (displayHasInit)
    return;
  displayHasInit = true;
  //SERIAL_IMPL.println("UI Starting up...");
  #ifndef __MARLIN_FIRMWARE__
    beginDigitalIO(); // Initialize digital IO
  #endif
  digitalWrite(TFT_RST, 1);
  delay(1);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);
  tft.ST7735sPatch();
  // ADd this to the end of init ST7735.cpp  
  //_height = ST7735_TFTHEIGHT_128;
  // _width = ST7735_TFTWIDTH_128;
  tft.fillScreen(ST77XX_WHITE);
  bTft = new BufferedDisplay(tft, TFT_startWrite, TFT_setAddressWindow, TFT_writePixels, TFT_endWrite);
  // Lets show the Splash.
  DrawSplash(bTft);
}
#ifdef __MARLIN_FIRMWARE__
void UISetup()
#else
void setup()
#endif
{
#ifndef __MARLIN_FIRMWARE__
  Serial.begin(115200);
#endif
  InitDisplayBasic(); // safe to recall
    // for (int i = 0; i < 5; i++){
    //     delay(1000);
    //     Serial.print(".");
    // }
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