#ifndef __UTILS__
#define __UTILS__
#include "..\Hardware\BufferedDisplay.h"

void centerString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut = 0, int16_t* hOut = 0);
void centerRightString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut = 0, int16_t* hOut = 0);
void centerLeftString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut = 0, int16_t* hOut = 0);


enum MixType
{
    Linear,
    EaseIn,
    EaseOut,
    EaseInEaseOut,
    Retract
};

void simplifyMixType(float& p, MixType& mt);
float MixFloats(float f1, float f2, float p, MixType mt);

#endif