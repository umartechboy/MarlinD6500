// #ifndef __KEY_PAD__
// #define __KEY_PAD__
// #include <Arduino.h>

// class MenuHost;

// enum Keys : byte {
//     KEYPAD_NONE         = 0, 
//     KEYPAD_RIGHT        = 1, 
//     KEYPAD_UP           = 3, 
//     KEYPAD_LEFT         = 5, 
//     KEYPAD_DOWN         = 7, 
//     KEYPAD_UP_RIGHT     = 2, 
//     KEYPAD_UP_LEFT      = 4, 
//     KEYPAD_DOWN_LEFT    = 6, 
//     KEYPAD_DOWN_RIGHT   = 8, 
//     KEYPAD_MIDDLE       = 9, 
//     KEYPAD_OPTIONS      = KEYPAD_DOWN_RIGHT,
//     KEYPAD_BACK         = KEYPAD_DOWN_LEFT,
// };

// class KeyPad_old{
// public:
//     void Loop(MenuHost* host);
// private:
//     Keys lastKeyDown = Keys::KEYPAD_NONE;
//     int dialValue = 0;
//     long lastKeyCheck = 0;
//     long lastDialRotateSentAt = 0;
//     Keys possibleSwipFrom = Keys::KEYPAD_NONE;
// };

// #endif