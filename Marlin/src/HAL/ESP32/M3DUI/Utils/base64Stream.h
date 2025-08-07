#ifndef __BASE64_STREAM__
#define __BASE64_STREAM__

#include <Arduino.h>

#define Base64DecoderBuffer 1024*8*2
extern byte * data_global;

extern const char * base64_chars;

class Base64Decoder{
public:
    byte b4 [4];
    byte b3 [4];
    int b4Count = 0;
    int bytesDecoded = 0;
    //byte* data = data_global;
    byte* data = data_global;
    Base64Decoder();
    ~Base64Decoder();
    void Feed(char chr);
    void Reset();
private:
    static inline bool is_base64(byte c);
    int base64_chars_find(char chr);
};

#endif