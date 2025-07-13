#include "base64Stream.h"

const char * base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

byte data_global [Base64DecoderBuffer];

Base64Decoder::~Base64Decoder(){
    Reset();
}
void Base64Decoder::Feed(char chr){
    // if (data == 0) {
    //     data = new byte[Base64DecoderBuffer];
    // }
    if (!is_base64(chr) || chr == '=' || chr == ' '|| chr == '\n' || chr == '\r' || chr == ';')
        return;
    byte b = base64_chars_find(chr);
    b4[b4Count] = b;
    b4Count++;
    if (b4Count >= 4){
        // Decode 4 bytes
        b3[0] = ((b4[0] & 0xFF) << 2) + ((b4[1] & 0x30) >> 4);
        b3[1] = ((b4[1] & 0x0F) << 4) + ((b4[2] & 0x3F) >> 2);
        b3[2] = ((b4[2] & 0x03) << 6) + ((b4[3] & 0xFF) >> 0);
        for (int i = 0; (i < 3); i++) {
            data_global[bytesDecoded] = b3[i];
            bytesDecoded++;
        }
        b4Count = 0;
    }
}
void Base64Decoder::Reset(){
    b4Count = 0;
    // if (data)
    //     delete [] data;
    // data = 0;
    bytesDecoded = 0;
}

inline bool Base64Decoder::is_base64(byte c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

int Base64Decoder::base64_chars_find(char chr){
    for (int i = 0; i < 64; i++){
        if (base64_chars[i] == chr)
            return i;
    }
    return -1;
}