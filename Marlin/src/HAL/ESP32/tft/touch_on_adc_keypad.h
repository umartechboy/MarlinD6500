#ifndef TOUCH_ON_ADC_KEYPAD
#define TOUCH_ON_ADC_KEYPAD
#include "..\..\..\inc\MarlinConfigPre.h"
#include "..\..\..\core\macros.h"
#if ENABLED(M3D_TouchPadDriverForADCKeypad)
#include "..\..\..\lcd\buttons.h" // buttons enum

#define tUp_Pin 34
#define tDown_Pin 34
#define tLeft_Pin 34
#define tRight_Pin 34

void touchOnADCKeyPad_Loop();
uint8_t touchOnADCKeyPad_getKey();
#endif
#endif