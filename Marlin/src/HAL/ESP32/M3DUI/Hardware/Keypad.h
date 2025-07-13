#ifndef __KEY_PAD__
#define __KEY_PAD__
#include <Arduino.h>

class MenuHost;

enum Keys : byte {
    KEYPAD_NONE         = 0, 
    KEYPAD_RIGHT        = 1, 
    KEYPAD_UP           = 3, 
    KEYPAD_LEFT         = 5, 
    KEYPAD_DOWN         = 7, 
    KEYPAD_UP_RIGHT     = 2, 
    KEYPAD_UP_LEFT      = 4, 
    KEYPAD_DOWN_LEFT    = 6, 
    KEYPAD_DOWN_RIGHT   = 8, 
    KEYPAD_MIDDLE       = 9, 
};

class KeyPad{
public:
    void Loop(MenuHost* host);
private:
    int lastKey = 0;
    Keys keyToSend = Keys::KEYPAD_NONE;
    Keys lastKeyDown = Keys::KEYPAD_NONE;
    int dialValue = 0;
    long lastKeyCheck = 0;
    long lastIncrementSendAt = 0;
    Keys possibleSwipFrom = Keys::KEYPAD_NONE;
};

#endif