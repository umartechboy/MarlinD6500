#include "M3DUI.h"
#include "BufferedDisplay.h"


#define DebugKeys 0

#define TFT_CS         17
#define TFT_DC         4
#define TFT_RST        200


Color DarkPaleYellow= rgb(255, 180, 0);
Color LightPaleYellow = rgb(255, 231, 173);
Color DarkOceanBlue = rgb(15, 113, 115);
Color LightOceanBlue = rgb(219, 250, 250);
Color DarkPurple = rgb(107, 45, 92);
Color LightPurple = rgb(237, 212, 231);
Color DarkRed = rgb(113, 11, 9);
Color MediumRed = rgb(242, 95, 92);
Color LightRed = rgb(250, 200, 198);
Color GoGreen = rgb(77, 194, 30);

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


MenuHost menuHost;
long lastUpdate = 0;


int pinMap [] = {14, 13, 0, 12};
int baseValues [4];
#define readPad(i) (min(100, (min(100, (max(0, (baseValues[(((uint32_t)i) % 4)] - touchRead(pinMap[(((uint32_t)i) % 4)]))) * 100 / baseValues[(((uint32_t)i) % 4)]) * 2))) / 100.0F)

bool first = true;
Keys touchOnADCKeyPad_getKey(){
    if (first){
        for (int i = 0; i < 4; i++){
          for (int ri = 0; ri < 10; ri ++){
            baseValues [i] += touchRead(pinMap[i]);
            delay(1);
          }
          baseValues[i] /= 10;
        }
      first = false;
    }
    // Make a cache
    float all [4];
    for (int i = 0; i < 4; i++) {
      all[i] = readPad(i);
    }
#if DebugKeys
    Serial.printf("Cache values: %f %f %f %f", all[0], all[1], all[2], all[3]);
#endif
    // Check if its the middle button
    int aboveZero = 0;
    for (int i = 0; i < 4; i++) {
      if (all[i] > 0.05){
          aboveZero++;
        }
    }
    
#if DebugKeys
    Serial.printf(", Above zero 1: %d", aboveZero);
#endif
    if (aboveZero >= 4){      
      Serial.println();
      return Keys::KEYPAD_MIDDLE;
    }
    // Check if its a diagonal button
    aboveZero = 0;
    for (int i = 0; i < 4; i++) {
      if (all[i] > 0.1){
          aboveZero++;
        }
    }
    
#if DebugKeys
    Serial.printf(", Above zero 2: %d", aboveZero);
#endif
    if (aboveZero == 2){
      // find the first ind
      
      for (int i = 0; i < 4; i++) {
        if (all[i] > 0.1){
          if (i == 0) {
            if (all[3] > 0.1) { 
              continue;
            }
          }
#if DebugKeys
          Serial.printf(", %d\n", i * 2 + 2 );
#endif
          return (Keys)(i * 2 + 2);
        }
      }
    }
    // Single Button Detection
    int maxI = 0;
    for (int i = 0; i < 4; i++) {
      if (all[i] > all[maxI])
        maxI = i;
    }
    
    if (all[maxI] < 0.5){
      
#if DebugKeys
      Serial.printf(", No key\n");
#endif
      return Keys::KEYPAD_NONE;
    }
    else{
      
#if DebugKeys
      Serial.printf(", One Key %d\n", maxI * 2 + 1 );
#endif
      return (Keys)(maxI * 2 + 1);
    }
}

void centerString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut, int16_t* hOut) {
    int16_t x1, y1;
    uint16_t w, h;
    g->setTextWrap(false);
    g->getTextBounds(str, x, y, &x1, &y1, &w, &h);
    int16_t errorInX = x1 - x;
    int16_t errorInY = y1 - y;
    //g->drawRect(x, y, w, h, ST7735_CYAN);
    //g->drawRect(x1, y1, w, h, ST7735_YELLOW);
    g->setCursor(x - w / 2 - errorInX, y - h / 2 - errorInY);
    g->print(str);
    //g->SetOpacity(30);
    //g->setCursor(x, y);
    //g->print(str);
    //g->SetOpacity(100);
    //g->print(str);
    if (wOut)
        *wOut = w;
    if (hOut)
        *hOut = h;
}
void centerRightString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut, int16_t* hOut) {
    int16_t x1, y1;
    uint16_t w, h;
    g->setTextWrap(false);
    g->getTextBounds(str, x, y, &x1, &y1, &w, &h);
    int16_t errorInX = x1 - x;
    int16_t errorInY = y1 - y;
    //g->drawRect(x, y, w, h, ST7735_CYAN);
    //g->drawRect(x1, y1, w, h, ST7735_YELLOW);
    g->setCursor(x - w - errorInX, y - h / 2 - errorInY);
    g->print(str);
    //g->SetOpacity(30);
    //g->setCursor(x, y);
    //g->print(str);
    //g->SetOpacity(100);
    //g->print(str);
    if (wOut)
        *wOut = w;
    if (hOut)
        *hOut = h;
}

void centerLeftString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut, int16_t* hOut) {
    int16_t x1, y1;
    uint16_t w, h;
    g->setTextWrap(false);
    g->getTextBounds(str, x, y, &x1, &y1, &w, &h);
    int16_t errorInX = x1 - x;
    int16_t errorInY = y1 - y;
    //g->drawRect(x, y, w, h, ST7735_CYAN);
    //g->drawRect(x1, y1, w, h, ST7735_YELLOW);
    g->setCursor(x - 0 - errorInX, y - h / 2 - errorInY);
    g->print(str);
    //g->SetOpacity(30);
    //g->setCursor(x, y);
    //g->print(str);
    //g->SetOpacity(100);
    //g->print(str);
    if (wOut)
        *wOut = w;
    if (hOut)
        *hOut = h;
}


float MixFloats(float f1, float f2, float p, MixType mt)
{
    simplifyMixType(p, mt);
    return f2 * p + f1 * (1 - p);
}

void simplifyMixType(float& p, MixType& mt)
{
    if (mt == MixType::Retract)
    {
        p = 2.0f * (float)(0.5 - abs(0.5 - p)); mt = MixType::Linear;
    }
    else if (mt == MixType::EaseIn)
    {
        p *= p * p;
        mt = MixType::Linear;
    }
    else if (mt == MixType::EaseOut)
    {
        p = (float)pow(p, 0.3);
        mt = MixType::Linear;
    }
    else if (mt == MixType::EaseInEaseOut)
    {
        if (p <= 0.5)
            p *= p * 2;
        else
        {
            p -= 0.5f;
            p = (float)pow(p, 0.5) / (float)pow(0.5, 0.5) * 0.5f;
            p += 0.5f;
        }
        mt = MixType::Linear;
    }
}



Bitmap bmp_Back = Bitmap(menuIcon_Back, -13, 2);
Bitmap bmp_ChangeFilament = Bitmap(menuIcon_ChangeFilament, -3, 3);
Bitmap bmp_Home = Bitmap(menuIcon_Home, -2, -4);
Bitmap bmp_Pause = Bitmap(menuIcon_Pause);
Bitmap bmp_Play = Bitmap(menuIcon_Play);
Bitmap bmp_SD = Bitmap(menuIcon_SD, -6, 2);
Bitmap bmp_Utilities = Bitmap(menuIcon_Utilities);
Bitmap bmp_StartPrint = Bitmap(menuIcon_StartPrint, 0, 0);
Bitmap bmp_PrintPreview = Bitmap(menuIcon_PrintPreview, 0, 0);
Bitmap bmp_MovePrint = Bitmap(menuIcon_MovePrint, 0, 0);

FilePreviewStep fileOverViewStep;
PrintPositionStep printPositionStep;
PrintStep printStep;
IdleScreenStep idleScreenStep;
SDMenuStep sdMenuStep;

PNG png;

void PNGDraw(PNGDRAW *pDraw)
{
    pngDecodeParams* params = (pngDecodeParams*)pDraw->pUser;
    BufferedDisplay* g = params->g;
    uint16_t usPixels [128];
    uint8_t ucMask [16];
    png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_LITTLE_ENDIAN, 0xffffffff);
    if (png.getAlphaMask(pDraw, ucMask, 255)) { // if any pixels are opaque, draw them
        for (int i = 0 ; i < pDraw->iWidth; i++){
            if (ucMask[i / 8] >> (7 - (i % 8)) && usPixels[i]) {
                g->drawPixel(- params->pngWidth / 2 + params->x + i, params->y + pDraw->y, usPixels[i]);
            }
        }

        //spilcdWritePixelsMasked(&lcd, pPriv->xoff, pPriv->yoff + pDraw->y, (uint8_t *)usPixels, ucMask, pDraw->iWidth, DRAW_TO_LCD);
    }
} /* PNGDraw() */


void UISetup() {  
  Serial.begin(115200);
    for (int i = 0; i < 5; i++){
        delay(1000);
        Serial.print(".");
    }
  Serial.println("Starting up...");
  digitalWrite(TFT_RST, 0);
  delay(1);
  digitalWrite(TFT_RST, 1);
  delay(1);

  tft.initR(INITR_144GREENTAB);
  tft.fillScreen(ST77XX_WHITE);
  tft.setRotation(3);
  bTft = new BufferedDisplay(tft, TFT_startWrite, TFT_setAddressWindow, TFT_writePixels, TFT_endWrite);
  
  Serial.println("Starting SD");
  sdMenuStep.BeginSD();
  
  Serial.println("Goto menu pushed");
  menuHost.GotoStep(&idleScreenStep);
  lastUpdate = millis();

  idleScreenStep.NextStep = &sdMenuStep;
  sdMenuStep.PreviousStep = &idleScreenStep;
  sdMenuStep.NextStep = &fileOverViewStep; 
  fileOverViewStep.PreviousStep = &sdMenuStep;
  fileOverViewStep.NextStep = &printPositionStep;
  printPositionStep.PreviousStep = &fileOverViewStep;
  printPositionStep.NextStep = &printStep;
  printStep.PreviousStep = &printPositionStep;
  //Serial.println("Begin with uint16");    
  // Serial.print("Red: 0b");
  // Serial.print(Color(ST7735_RED).toColor16(), 2);
  // Serial.printf(", r: %d, g: %d, b: %d\n", Color(ST7735_RED).R(), Color(ST7735_RED).B(), Color(ST7735_RED).A());
  // Serial.print("Green: 0b");
  // Serial.print(Color(ST7735_GREEN).toColor16(), 2);
  // Serial.printf(", r: %d, g: %d, b: %d\n", Color(ST7735_GREEN).R(), Color(ST7735_GREEN).B(), Color(ST7735_GREEN).A());
  // Serial.print("Blue: 0b");
  // Serial.println(Color(ST7735_BLUE).toColor16(), 2);
  // Serial.printf(", r: %d, g: %d, b: %d\n", Color(ST7735_BLUE).R(), Color(ST7735_BLUE).B(), Color(ST7735_BLUE).A());
  // Serial.print("Cyan: 0b");
  // Serial.println(Color(ST7735_CYAN).toColor16(), 2);
  // Serial.printf(", r: %d, g: %d, b: %d\n", Color(ST7735_CYAN).R(), Color(ST7735_CYAN).B(), Color(ST7735_CYAN).A());
  // Serial.println("Color test rgb() and cast to uint16");  
  // Serial.print("Red: 0b");
  // Serial.print(rgb(255,0,0).toColor16(), 2);
  // Serial.printf(", r: %d, g: %d, b: %d\n", rgb(255,0,0).R(), rgb(255,0,0).B(), rgb(255,0,0).A());
  // Serial.print("Green: 0b");
  // Serial.print(rgb(0,255,0).toColor16(), 2);
  // Serial.printf(", r: %d, g: %d, b: %d\n", rgb(255,0,0).R(), rgb(255,0,0).B(), rgb(255,0,0).A());
  // Serial.print("Blue: 0b");
  // Serial.println(rgb(0, 0, 255).toColor16(), 2);
  // Serial.printf(", r: %d, g: %d, b: %d\n", rgb(255,0,0).R(), rgb(255,0,0).B(), rgb(255,0,0).A());  
  // Serial.println("Blend test");  
  // Color r = rgb(255, 0, 0);
  // Color g = rgb(0, 255, 0);
  // Color b = rgb(0, 0, 255);
  // Serial.print("r + g 100%: ");
  // r.BlendBelow(g);
  // Serial.println(r, 2);  
  // r = rgb(255, 0, 0);
  // Serial.printf(", r: %d, g: %d, b: %d, a: %d\n", r.R(), r.G(), r.B(), r.A());
  // Serial.println("r + g 50%: ");  
  // g.A(127);  
  // Serial.printf("Green , r: %d, g: %d, b: %d, a: %d\n", g.R(), g.G(), g.B(), g.A());
  // r.BlendBelow(g);
  // Serial.print(r, 2);
  // Serial.printf(", New Red: r: %d, g: %d, b: %d, a: %d\n", r.R(), r.G(), r.B(), r.A());
  // g.A(10);  
  // r = rgb(255, 0, 0);
  // Serial.printf("Green , r: %d, g: %d, b: %d, a: %d\n", g.R(), g.G(), g.B(), g.A());
  // r.BlendBelow(g);
  // Serial.print(r, 2);
  // Serial.printf(", New Red: r: %d, g: %d, b: %d, a: %d\n", r.R(), r.G(), r.B(), r.A());
  // Serial.print("r + g 0%: ");
  // g.A(0);
  // Serial.println(r.Blend(g));
  // Serial.printf(", r: %d, g: %d, b: %d\n", r.Blend(g).R(), r.Blend(g).B(), r.Blend(g).A());
  //while(true) delay(1);
}
int lastKey = 0;
Keys keyToSend = Keys::KEYPAD_NONE;
Keys lastKeyDown = Keys::KEYPAD_NONE;
int dialValue = 0;
long lastKeyCheck = 0;
long lastIncrementSendAt = 0;
Keys possibleSwipFrom = Keys::KEYPAD_NONE;

#define AddKey(k, i) (Keys)(((k) + (i) > 8) ? ((k) + (i) - 8):(((k) + (i) < 1) ? ((k) + (i) + 8):((k) + (i))))
void UILoop() {
  
    menuHost.Paint(bTft);
    if (menuHost.CurrentStep)
      menuHost.CurrentStep->loop();
    if (menuHost.TargetStep)
      menuHost.TargetStep->loop();

    if (millis() - lastKeyCheck > 30){
      lastKeyCheck = millis();
      Keys key = touchOnADCKeyPad_getKey();
      if (key){ // we just need to wait for it to go up
        if (key != lastKeyDown && lastKeyDown != Keys::KEYPAD_NONE){ // dial rotate or swipe
          Serial.printf("Dial Rotate: %d > %d\n", lastKeyDown, key);

          if (key == AddKey(lastKeyDown, 1)) {
            if (menuHost.CurrentStep){ 
              menuHost.CurrentStep->IncrementValue(); 
              if (millis() - lastIncrementSendAt < 30) 
                menuHost.CurrentStep->IncrementValue(); // accelerate
              lastIncrementSendAt = millis(); 
            }
          }
          else if (key ==  AddKey(lastKeyDown, -1)) {
              if (menuHost.CurrentStep) { 
              menuHost.CurrentStep->DecrementValue(); 
              if (millis() - lastIncrementSendAt < 30) 
                menuHost.CurrentStep->DecrementValue(); // accelerate
              lastIncrementSendAt = millis(); 
            }
          }
          else {
            lastKeyDown = Keys::KEYPAD_NONE;
          }
        }
        else
          keyToSend = key;
        lastKeyDown = key;
      }
      else {
        //  cannot be a dial rotate
        lastKeyDown = Keys::KEYPAD_NONE;
        possibleSwipFrom = Keys::KEYPAD_NONE;
        if(keyToSend){
          Serial.printf("Key proessed: %d\n", keyToSend);
          if (menuHost.TargetStep == 0){
            if (keyToSend == KEYPAD_MIDDLE || keyToSend == KEYPAD_UP_RIGHT){
              Serial.println("Middle or up right key pressed");
              // begin buttons overlay
              if(menuHost.stepAnimationStage == StepAnimationStage::MainStep && keyToSend == KEYPAD_MIDDLE){
                menuHost.stepAnimationStage = StepAnimationStage::GoingToOverLay;
                menuHost.moveToMenuAfterStepAnim = 0;
                menuHost.ResetAnimationProgress(250);
              }
              else if (menuHost.stepAnimationStage == StepAnimationStage::InOverlay){
                menuHost.stepAnimationStage = StepAnimationStage::GoingToStep;
                menuHost.moveToMenuAfterStepAnim = 0;
                menuHost.ResetAnimationProgress(250);
              }
            }
            else if (keyToSend == KEYPAD_UP_LEFT && menuHost.CurrentStep->PreviousStep && menuHost.stepAnimationStage == StepAnimationStage::InOverlay){
              Serial.println("Going to previous step");
              // begin Next menu transition
              menuHost.stepAnimationStage = StepAnimationStage::GoingToStep;
              menuHost.moveToMenuAfterStepAnim = menuHost.CurrentStep->PreviousStep;
              menuHost.menuTransitionDirection = TransitionDirection::Backward;
              menuHost.ResetAnimationProgress(250);
            }
            else if (keyToSend == KEYPAD_DOWN_RIGHT && menuHost.CurrentStep->NextStep && menuHost.stepAnimationStage == StepAnimationStage::InOverlay){
              Serial.println("Going to next step");
              // begin Next menu transition
              menuHost.stepAnimationStage = StepAnimationStage::GoingToStep;
              menuHost.moveToMenuAfterStepAnim = menuHost.CurrentStep->NextStep;
              menuHost.CurrentStep->NextStep->BeginLoad();
              menuHost.menuTransitionDirection = TransitionDirection::Forward;
              menuHost.ResetAnimationProgress(250);
            }
            else { // Let the step handle this key 
              menuHost.CurrentStep->HandleKeyUp(keyToSend);
            }
          }
          // else just discard this button
          keyToSend = Keys::KEYPAD_NONE;
        }
      }
    }
}

