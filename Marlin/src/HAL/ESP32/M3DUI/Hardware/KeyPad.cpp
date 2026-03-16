#include "Keypad.h"
#include "..\Components\M3DUI.h"
#include "..\App\MenuApp.h"
#include "..\../../..\inc/MarlinConfig.h"
#include "..\../../..\gcode/gcode.h"
#include <Preferences.h>

#define DebugKeys 0
static int touchPinMap [] = {14, 13, 0, 12};
static float totalTouchNoise[] = {0, 0, 0, 0};
static bool touchReadCache[] = {0, 0, 0, 0};
int touchReadBaseUpdateCount = 0;
static float touchReadBase[] = {0, 0, 0, 0};
static int lastTouchRead[] = {0, 0, 0, 0};
static long lastNoiseSenseLoop = 0;

bool hasJoyStick = false;
uint32_t readADCMV(const pin_t pin);
Keys touchOnADCKeyPad_getKey();

void noiseSenseLoop(){
  if (millis() - lastNoiseSenseLoop < 2)
      return;
  lastNoiseSenseLoop = millis();
  int noiseThreshold = 10;
  int stabilityTolerance = 7;
  float N = 10;
  float mix = 0.05;
  // SERIAL_IMPL.print("Pins: ");
  for (int i = 0; i < 4; i++) {
    //all[i] = readPad(i);
    int thisRead = touchRead(touchPinMap[i]);
    // Lets first calculate based on noise
    int delta =  thisRead - lastTouchRead[i];
    totalTouchNoise[i] += abs(delta) - 2;
    
    float percDev = 0;
    if (touchReadBaseUpdateCount < N)
      touchReadBase[i] += (float)thisRead  / N;
    else {
      touchReadBase[i] = touchReadBase[i] * (1 - mix) + (float)touchRead(touchPinMap[i]) * mix;
    
      // SERIAL_IMPL.printf("{%f", totalTouchNoise[i]);
      // We have the base. Now calculate % abs deviation
      float absDev = abs(thisRead - touchReadBase[i]);
      percDev = (absDev / touchReadBase[i]) * 100.0;
      totalTouchNoise[i] += percDev / 30;
    }
    // SERIAL_IMPL.printf("+ %f = %f},\t", percDev, totalTouchNoise[i]);

    // Clamp and pull down    
    if (totalTouchNoise[i] < 0) totalTouchNoise[i] = 0;
    else if (totalTouchNoise[i] > noiseThreshold) totalTouchNoise[i] = noiseThreshold;
    
    touchReadCache[i] = totalTouchNoise[i] >= noiseThreshold - stabilityTolerance; 
    lastTouchRead[i] = thisRead;
  }
  // SERIAL_IMPL.println();
  touchReadBaseUpdateCount++;
}

Keys adcKeyMap [] = {
Keys::KEYPAD_RIGHT,
Keys::KEYPAD_MIDDLE,
Keys::KEYPAD_UP,
Keys::KEYPAD_BACK,
Keys::KEYPAD_DOWN,
Keys::KEYPAD_LEFT,
};
int minADC [] =     { 843, 803, 736, 665, 495, 5,};
int maxADC [] =     { 880, 842, 802, 715, 535, 45,};
// int typicalADC [] = { 860, 827, 779, 696, 521, 23,};
// int typicalADC [] = { 860, 823, 776, 684, 516, 23,};
// int typicalADC [] = { 850, 818, 771, 682, 503, 23,};

long lastTempKeyDebug = 0;
float touchOnADCKeyPad_getKeyIntensity(Keys key){
  if (hasJoyStick){
    return (key == touchOnADCKeyPad_getKey())? 10:0;
  }
  if (key == Keys::KEYPAD_RIGHT || key == Keys::KEYPAD_UP || key == Keys::KEYPAD_LEFT || key == Keys::KEYPAD_DOWN) {
    return totalTouchNoise[(key - 1) / 2];
  }
  else if (key == Keys::KEYPAD_DOWN_LEFT) {
    return totalTouchNoise[2] / 2 + totalTouchNoise[3] / 2;
  }
  else if (key == Keys::KEYPAD_DOWN_RIGHT) {
    return totalTouchNoise[0] / 2 + totalTouchNoise[3] / 2;
  }
  else if (key == Keys::KEYPAD_UP_LEFT) {
    return totalTouchNoise[1] / 2 + totalTouchNoise[2] / 2;
  }
  else if (key == Keys::KEYPAD_UP_RIGHT) {
    return totalTouchNoise[1] / 2 + totalTouchNoise[0] / 2;
  }
  else if (key == Keys::KEYPAD_MIDDLE) {
    return totalTouchNoise[0] / 4 + totalTouchNoise[1] / 4 +  totalTouchNoise[2] / 4 +  totalTouchNoise[3] / 4;
  }
  else return 0;
}
Keys touchOnADCKeyPad_getKey(){
  
    if (hasJoyStick){
      int adc = readADCMV(3);
      for (int i = 0; i < 6; i++){
        if ( adc >= minADC[i] && adc <= maxADC[i])
          return adcKeyMap[i];
      }
      // safe_delay(500);    
      // SERIAL_IMPL.printf("KP0: %d\n", );
      // return Keys::KEYPAD_NONE;
      return Keys::KEYPAD_NONE;
    }
noiseSenseLoop();
// Lets make a simpler loop;
// if (millis() - lastTempKeyDebug > 10){
//   // SERIAL_IMPL.printf("Keys: ");
//   // for (int i = 0; i < 4; i++) {
//   //   SERIAL_IMPL.printf("%f: %d,\t", totalTouchNoise[i], touchRead(touchPinMap[i]));
//   // }
//   // SERIAL_IMPL.println();
//   lastTempKeyDebug = millis();
// }
// return Keys::KEYPAD_NONE; // TEMP
int noOfKeysDown = 0;

for (int i = 0; i < 4; i++){
  if (totalTouchNoise[i] > 1)
    noOfKeysDown++;
}
if (noOfKeysDown >= 3)
  return Keys::KEYPAD_MIDDLE;

if (totalTouchNoise[2] > 3 && totalTouchNoise[3] > 3)
  return Keys::KEYPAD_DOWN_LEFT;
if (totalTouchNoise[0] > 3 && totalTouchNoise[3] > 3)
  return Keys::KEYPAD_DOWN_RIGHT;

if (totalTouchNoise[0] > 6)
  return Keys::KEYPAD_RIGHT;
else if (totalTouchNoise[1] > 6)
  return Keys::KEYPAD_UP;
else if (totalTouchNoise[2] > 6)
  return Keys::KEYPAD_LEFT;
else if (totalTouchNoise[3] > 6)
  return Keys::KEYPAD_DOWN;

return Keys::KEYPAD_NONE;


//SERIAL_IMPL.printf("Pins: %f, %f, %f, %f\n", totalTouchNoise[0], totalTouchNoise[1], totalTouchNoise[2], totalTouchNoise[3]);
return Keys::KEYPAD_NONE;
#if DebugKeys
    SERIAL_IMPL.printf("Cache values: %d %d %d %d", touchReadCache[0], touchReadCache[1], touchReadCache[2], touchReadCache[3]);
#endif
    // Check if its the middle button
    int aboveZero = 0;
    for (int i = 0; i < 4; i++) {
      if (touchReadCache[i]){
          aboveZero++;
        }
    }
    
#if DebugKeys
    SERIAL_IMPL.printf(", Above zero: %d", aboveZero);
#endif
    if (aboveZero >= 3){  
#if DebugKeys    
      SERIAL_IMPL.println();
#endif
      return Keys::KEYPAD_MIDDLE;
    }
    else if (aboveZero == 0) {
#if DebugKeys    
      SERIAL_IMPL.println();
#endif
        return Keys::KEYPAD_NONE;
    }
    else if (aboveZero == 1){
      // find the first ind      
      for (int i = 0; i < 4; i++) {
        if (touchReadCache[i]){
#if DebugKeys
          SERIAL_IMPL.printf(", %d\n", i * 2 + 1 );
#endif
            return (Keys)(i * 2 + 1);
        }
      }
    }
    else if (aboveZero == 2){
      // find the first ind      
      for (int i = 0; i < 4; i++) {
        if (touchReadCache[i]){
          if (i == 0) {
            if (touchReadCache[3]) { 
              continue;
            }
          }
#if DebugKeys
          SERIAL_IMPL.printf(", %d\n", i * 2 + 2 );
#endif
          return (Keys)(i * 2 + 2);
        }
      }
      
      // Means it wasn't a diagnol. Could be the middle button
#if DebugKeys    
      SERIAL_IMPL.println("~Middle");
#endif
      return Keys::KEYPAD_MIDDLE;
    }
  return Keys::KEYPAD_NONE;
}

#define AddKey(k, i) (Keys)(((k) + (i) > 8) ? ((k) + (i) - 8):(((k) + (i) < 1) ? ((k) + (i) + 8):((k) + (i))))

bool unknwonSwipe = false;
int swipeProgress = 0;
bool pressInProcess = false;
long keyDownSince = 0;
int pressesInARow = 0;
int pressPeriod = 500;
bool holdSent = false;
void setJoystick(bool joy){
  hasJoyStick = joy;
  
  Preferences prefs;
  prefs.begin("machine");
  prefs.putBool("joystick", joy);
  prefs.end();
}
void GcodeSuite::M38() {
  if (parser.seen('P')){
    setJoystick(parser.value_bool());
  }
  else {
    SERIAL_IMPL.print("Using: ");
    SERIAL_IMPL.print(hasJoyStick ? "JoyStick":"Touchpad");
    SERIAL_IMPL.println();

  }
}
void KeyPad::handleJoystick(MenuHost* host) {
    static Keys activeKey = KEYPAD_NONE;
    static unsigned long keyDownTime = 0;
    static unsigned long lastPressTime = 0;
    static long repeatDelay = 500;
    static int keyUpCount = 0;
    static long lastLoop = 0;
    int tooFastKeyDownDelay = 100; // Can't send presses faster than this.
    if (millis() - lastLoop < 10)
      return;
    lastLoop = millis();
    Keys currentKey = touchOnADCKeyPad_getKey();
    unsigned long now = millis();
    
    // 1. NEW KEY PRESS
    if (activeKey == KEYPAD_NONE) {
      if (currentKey != KEYPAD_NONE) { // new key press
        safe_delay(10); // settle time
        currentKey = touchOnADCKeyPad_getKey();
        if (currentKey == KEYPAD_NONE)
          return; // noise
          activeKey = currentKey;
          keyDownTime = now;
          repeatDelay = 500;
          keyUpCount = 0;
          
          if (millis() - lastPressTime < tooFastKeyDownDelay){ // skip press    
            SERIAL_IMPL.printf("[JOY] >> PRESS Skip 1: %d\n", currentKey);
          }
          else {        
            SERIAL_IMPL.printf("[JOY] >> PRESS 1: %d\n", currentKey);
            host->HandleKeyPress(activeKey);
          }
          lastPressTime = now;
          return;
      }
      else // no activity
      {
        host->NotifyNoKey();
        return;
      }
    }
    else {
      if (currentKey == Keys::KEYPAD_NONE){
        // Key Up
        if (keyUpCount++ < 2)
          SERIAL_IMPL.printf("[JOY] >> Key up skip\n"); // Give it some time
        else {
          activeKey = Keys::KEYPAD_NONE;
          SERIAL_IMPL.printf("[JOY] >> Key up\n"); // Give it some time
        }
        return;
      }
      else if (currentKey == activeKey) {
        // hold
        if (millis() - lastPressTime > repeatDelay){
          lastPressTime = millis();
          repeatDelay -= 100; 
          if (repeatDelay < 50)
            repeatDelay = 50;
          SERIAL_IMPL.printf("[JOY] >> PRESS Repeat: %d\n", currentKey);
          host->HandleKeyPress(activeKey);
        }
        if (millis() - keyDownTime > 2000 && !holdSent){
          holdSent = true;
          host->HandleKeyHold(currentKey);
        }
      }
      else { // bounce to other key. reset cycle.        
          activeKey = currentKey;
          keyDownTime = now;
          repeatDelay = 500;
          keyUpCount = 0;

          if (millis() - lastPressTime < tooFastKeyDownDelay){ // skip press    
            SERIAL_IMPL.printf("[JOY] >> PRESS Skip 2: %d\n", currentKey);
          }
          else {        
            SERIAL_IMPL.printf("[JOY] >> PRESS 2: %d\n", currentKey);
            host->HandleKeyPress(activeKey);
          }
          lastPressTime = now;
          return;
      }
    }
}

void KeyPad::Loop(MenuHost* host){
  
  // SERIAL_IMPL.printf("readADCmv: %d\n", readADCMV(39));
  // safe_delay(150);
  // return;
  if (hasJoyStick){
    handleJoystick(host);
    return;
  }
  //noiseSenseLoop();
  if (millis() - lastKeyCheck > ((lastKeyDown == Keys::KEYPAD_NONE)?5:20)){
    lastKeyCheck = millis();
    Keys key = touchOnADCKeyPad_getKey();
    // if (key || lastKeyDown)
    //   SERIAL_IMPL.printf("Got Key: %d\n", key);
    if (key == KEYPAD_NONE && lastKeyDown == KEYPAD_NONE){
      // Idle
      lastKeyDown = key;
      return;
    }
    // Its a change. Detect which one    
    if (key == KEYPAD_NONE){ // lastKey cannot be NONE too
      // Its a key Up
      if (swipeProgress > 1 && !unknwonSwipe){ 
        // End of swipe doesn't mean anything. At least not yet
        swipeProgress = 0;
        SERIAL_IMPL.println("End of Swipe");
      }
      else {
        // Send Key up
        SERIAL_IMPL.printf("Key Up (%d): %d, %d\n", pressInProcess, lastKeyDown, key);
        if (!pressInProcess)
          host->HandleKeyPress(lastKeyDown);
      }
    }
    else if (lastKeyDown == KEYPAD_NONE) {// Its a Key down.      
      // Reset the cycle here
      keyDownSince = millis();
      swipeProgress = 0; // Not needed, but still reset things
      unknwonSwipe = false;
      pressInProcess = false;
      pressPeriod = 500;
      pressesInARow = 0;
      holdSent = false;

      // We need to give the finger some to settle
      int settelingTime = 50;
      if (key == Keys::KEYPAD_MIDDLE) // already too down. Its conclusive
        settelingTime = 0;
      // We can't send key down because it might turn into a swipe
      else if (key == Keys::KEYPAD_UP || key == Keys::KEYPAD_DOWN || key == Keys::KEYPAD_LEFT || key == Keys::KEYPAD_RIGHT) // Single key
        settelingTime = 30;
      if (settelingTime){
        long settleStartAt = millis();
        while(millis() - settleStartAt < settelingTime){
          if (touchOnADCKeyPad_getKey() == Keys::KEYPAD_NONE || touchOnADCKeyPad_getKey() == Keys::KEYPAD_MIDDLE || touchOnADCKeyPad_getKey() == Keys::KEYPAD_DOWN_RIGHT || touchOnADCKeyPad_getKey() == Keys::KEYPAD_DOWN_LEFT){ // conclusive. Button has gone up or gone down
            break;
          }
          noiseSenseLoop();
          safe_delay(1);
        }
        Keys settledKey = touchOnADCKeyPad_getKey();
        if (settledKey == Keys::KEYPAD_NONE) { // The press was too fast but conclusive
          // don't update
        }
        else{
          key = settledKey;
        }
      }
      
      SERIAL_IMPL.printf("Key Down: %d\n", key);
      if (key == Keys::KEYPAD_MIDDLE){
        pressInProcess = true;
        SERIAL_IMPL.printf("Middle Key Down 1: %d\n", key);
        pressPeriod = 2000;
        host->HandleKeyPress(key);
        keyDownSince = millis() + pressPeriod;
        lastKeyDown = key;
        return;
      }
    }
    else {
      if (lastKeyDown == key) { // Key Hold
        if (swipeProgress > 1){
          // Just skip
        }
        else { // We process press here
          if (millis() - keyDownSince > pressPeriod){
            if (!holdSent) {
              holdSent = true;
              host->HandleKeyHold(key);
            }
            if (key == Keys::KEYPAD_UP || key == Keys::KEYPAD_DOWN || key == Keys::KEYPAD_LEFT || key == Keys::KEYPAD_RIGHT){
              pressesInARow++;
              if (pressesInARow * pressPeriod > 300){ // x seconds elapsed since the last acceleration
                pressPeriod -= 100;
                pressesInARow = 0;
              }
              if (pressPeriod < 50)
                pressPeriod = 50;
              SERIAL_IMPL.printf("Key Press: %d\n", key);
              host->HandleKeyPress(key);
              pressInProcess = true;
              keyDownSince = millis();
            }
          }
        }
      }
      else if (!pressInProcess){
        // Key to Key swipe
        if(swipeProgress == 0)
          swipeProgress = 1;
        // Detect the gesture

        // test for swipe to middle
        //if (key == Keys::KEYPAD_MIDDLE || key == KEYPAD_DOWN_LEFT || key == KEYPAD_DOWN_RIGHT)
        // Send a press because we are pretty sure about the key
        if (!pressInProcess) {
          // Its not a swipe, its a key down.
          swipeProgress = 0;
          pressInProcess = true;
          SERIAL_IMPL.printf("Middle Key Down 2: %d\n", key);
          pressPeriod = 2000;
          host->HandleKeyPress(key);
          keyDownSince = millis() + pressPeriod;
          lastKeyDown = key;
        }
        // // Test for Dial rotate
        // else if (key == AddKey(lastKeyDown, 1) && !unknwonSwipe){
        //   SERIAL_IMPL.printf("Dial Inc: %d > %d\n", lastKeyDown, key);
        //   swipeProgress++;
        //   if (swipeProgress > 1){
        //     host->HandleDialIncrement();
        //     if (millis() - lastDialRotateSentAt < 30) 
        //         host->HandleDialIncrement(); // accelerate
        //     lastDialRotateSentAt = millis(); 
        //   }
        // }
        // else if (key == AddKey(lastKeyDown, -1) && !unknwonSwipe){
        //   SERIAL_IMPL.printf("Dial Dec: %d > %d\n", lastKeyDown, key);
        //   swipeProgress++;
        //   if (swipeProgress > 1){
        //     host->HandleDialDecrement();
        //     if (millis() - lastDialRotateSentAt < 30) 
        //         host->HandleDialDecrement(); // accelerate
        //     lastDialRotateSentAt = millis(); 
        //   }
        // }
        // else if (key == KEYPAD_MIDDLE && lastKeyDown == KEYPAD_DOWN || key == KEYPAD_UP && lastKeyDown == KEYPAD_MIDDLE){
        //   SERIAL_IMPL.printf("Swipe Up: %d > %d\n", lastKeyDown, key);
        //   host->HandleDialIncrement();
        // }
        // else if (key == KEYPAD_MIDDLE && lastKeyDown == KEYPAD_UP || key == KEYPAD_DOWN && lastKeyDown == KEYPAD_MIDDLE){
        //   SERIAL_IMPL.printf("Swipe Down: %d > %d\n", lastKeyDown, key);
        //   host->HandleDialDecrement();
        // }
        else { // Swipe but not a dial rotate
          SERIAL_IMPL.printf("Unknown Swipe: %d > %d\n", lastKeyDown, key);
          unknwonSwipe = true;
        }
      }
    }
    lastKeyDown = key;
  }
}
