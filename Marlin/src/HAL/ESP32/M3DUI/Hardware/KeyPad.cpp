#include "Keypad.h"
#include "..\Components\M3DUI.h"
#include "..\App\MenuApp.h"

#define DebugKeys 0
static int touchPinMap [] = {14, 13, 0, 12};
static float totalTouchNoise[] = {0, 0, 0, 0};
static bool touchReadCache[] = {0, 0, 0, 0};
static int lastTouchRead[] = {0, 0, 0, 0};
static long lastNoiseSenseLoop = 0;

void noiseSenseLoop(){
    if (millis() - lastNoiseSenseLoop < 10)
        return;
    lastNoiseSenseLoop = millis();
    int noiseThreshold = 10;
    int stabilityTolerance = 7;
    for (int i = 0; i < 4; i++) {
      //all[i] = readPad(i);
      int thisRead = touchRead(touchPinMap[i]);
      int delta =  thisRead - lastTouchRead[i];
      totalTouchNoise[i] += abs(delta) - 2;
      if (totalTouchNoise[i] < 0) totalTouchNoise[i] = 0;
      else if (totalTouchNoise[i] > noiseThreshold) totalTouchNoise[i] = noiseThreshold;
      touchReadCache[i] = totalTouchNoise[i] >= noiseThreshold - stabilityTolerance; 

      lastTouchRead[i] = thisRead;
    }
}

Keys touchOnADCKeyPad_getKey(){

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
}

#define AddKey(k, i) (Keys)(((k) + (i) > 8) ? ((k) + (i) - 8):(((k) + (i) < 1) ? ((k) + (i) + 8):((k) + (i))))

bool unknwonSwipe = false;
int swipeProgress = 0;
bool pressInProcess = false;
long keyDownSince = 0;
void KeyPad::Loop(MenuHost* host){
  noiseSenseLoop();
  if (millis() - lastKeyCheck > ((lastKeyDown == Keys::KEYPAD_NONE)?10:50)){
    lastKeyCheck = millis();
    Keys key = touchOnADCKeyPad_getKey();
    if (key || lastKeyDown)
      SERIAL_IMPL.printf("Got Key: %d\n", key);
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
      keyDownSince = millis();
      swipeProgress = 0; // Not needed, but still reset things
      unknwonSwipe = false;
      pressInProcess = false;

      // We need to give the finger some to settle
      int settelingTime = 200;
      if (key == Keys::KEYPAD_MIDDLE) // already too down. Its conclusive
        settelingTime = 0;
      // We can't send key down because it might turn into a swipe
      else if (key == Keys::KEYPAD_UP || key == Keys::KEYPAD_DOWN || key == Keys::KEYPAD_LEFT || key == Keys::KEYPAD_RIGHT) // Single key
        settelingTime = 100;
      if (settelingTime){
        long settleStartAt = millis();
        while(millis() - settleStartAt < settelingTime){
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
        host->HandleKeyPress(key);
        keyDownSince = millis() + 500;
        lastKeyDown = key;
        return;
      }
    }
    else {
      if (lastKeyDown == key) { // Key Hold
        if (swipeProgress > 1){
          // Just skip
        }
        else { // We can process a hold or press here
          // skip for now
          if (millis() - keyDownSince > 500){
            host->HandleKeyPress(key);
            pressInProcess = true;
            keyDownSince = millis();
          }
        }
      }
      else if (!pressInProcess){
        // Key to Key swipe
        if(swipeProgress == 0)
          swipeProgress = 1;
        // Detect the gesture

        // test for swipe to middle
        if (key == Keys::KEYPAD_MIDDLE){
          // Its not a swipe, its a key down.
          swipeProgress = 0;
          pressInProcess = true;
          host->HandleKeyPress(key);
          keyDownSince = millis() + 500;
        }
        // Test for Dial rotate
        else if (key == AddKey(lastKeyDown, 1) && !unknwonSwipe){
          SERIAL_IMPL.printf("Dial Inc: %d > %d\n", lastKeyDown, key);
          swipeProgress++;
          if (swipeProgress > 1){
            host->HandleDialIncrement();
            if (millis() - lastDialRotateSentAt < 30) 
                host->HandleDialIncrement(); // accelerate
            lastDialRotateSentAt = millis(); 
          }
        }
        else if (key == AddKey(lastKeyDown, -1) && !unknwonSwipe){
          SERIAL_IMPL.printf("Dial Dec: %d > %d\n", lastKeyDown, key);
          swipeProgress++;
          if (swipeProgress > 1){
            host->HandleDialDecrement();
            if (millis() - lastDialRotateSentAt < 30) 
                host->HandleDialDecrement(); // accelerate
            lastDialRotateSentAt = millis(); 
          }
        }
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