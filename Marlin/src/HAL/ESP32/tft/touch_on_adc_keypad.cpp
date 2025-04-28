#include "touch_on_adc_keypad.h"
#include "..\..\..\inc\MarlinConfigPre.h"

#if M3D_TouchPadDriverForADCKeypad

void touchOnADCKeyPad_Loop(){ // no gestures supported yet.

}

// return something from 
// BLEN_KEYPAD_F3, 
// BLEN_KEYPAD_F2, 
// BLEN_KEYPAD_F1, 
// BLEN_KEYPAD_DOWN, 
// BLEN_KEYPAD_RIGHT, 
// BLEN_KEYPAD_MIDDLE, 
// BLEN_KEYPAD_UP, 
// BLEN_KEYPAD_LEFT, 
int pinMap [] = {14, 13, 15, 12};
int keyMap [] = {BLEN_KEYPAD_RIGHT, BLEN_KEYPAD_UP, BLEN_KEYPAD_LEFT, BLEN_KEYPAD_DOWN, BLEN_KEYPAD_MIDDLE};
int baseValues [4];
#define readPad(i) (min(100, (min(100, (max(0, (baseValues[(((uint32_t)i) % 4)] - touchRead(pinMap[(((uint32_t)i) % 4)]))) * 100 / baseValues[(((uint32_t)i) % 4)]) * 2))) / 100.0F)

bool first = true;
uint8_t touchOnADCKeyPad_getKey(){
    if (first){
        for (int i = 0; i < 4; i++){
          for (int ri = 0; ri < 10; ri ++)
            baseValues [i] += touchRead(pinMap[i]);
          baseValues[i] /= 10;
        }
      first = false;
    }
    float all [4];
    for (int i = 0; i < 4; i++) {
      all[i] = readPad(i);
      Serial.print(all[i]);
      Serial.print(", ");
    }
    Serial.println();
    int maxI = 0;
    
    // Check if its the middle button
    bool allAbove0 = true;
    for (int i = 0; i < 4; i++) {
      if (all[i] < 0.05){
          allAbove0 = false;
          break;
        }
    }
    if (allAbove0){
      Serial.println(keyMap[maxI]);
      return keyMap[4];
      //return 0;
    }
    
    for (int i = 0; i < 4; i++) {
      if (all[i] > all[maxI])
        maxI = i;
    }
    
    if (all[maxI] < 0.5){
      return 0;
    }
    Serial.println(keyMap[maxI]);
    return keyMap[maxI];
    //return 0;
}

#endif