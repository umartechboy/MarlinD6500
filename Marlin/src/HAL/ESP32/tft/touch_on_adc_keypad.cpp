#include "touch_on_adc_keypad.h"

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

#define hasBooleanTouch(pin) (touchRead(pin) > 50)

uint8_t touchOnADCKeyPad_getKey(){
    if(hasBooleanTouch(tUp_Pin) && hasBooleanTouch(tDown_Pin) && hasBooleanTouch(tLeft_Pin) && hasBooleanTouch(tRight_Pin))
        return BLEN_KEYPAD_MIDDLE;
    if (hasBooleanTouch(tUp_Pin)) return BLEN_KEYPAD_UP;
    if (hasBooleanTouch(tDown_Pin)) return BLEN_KEYPAD_DOWN;
    if (hasBooleanTouch(tLeft_Pin)) return BLEN_KEYPAD_LEFT;
    if (hasBooleanTouch(tRight_Pin)) return BLEN_KEYPAD_RIGHT;
    return 0;
}
