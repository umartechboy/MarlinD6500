#ifndef __UTILS__
#define __UTILS__
#include "..\Hardware\BufferedDisplay.h"
#include "MenuHost.h"


extern int retroNavSectionHeight;
extern int retroTitleSectionHeight;

class MenuHost;
void centerString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut = 0, int16_t* hOut = 0);
void leftString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut = 0, int16_t* hOut = 0);
void centerRightString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut = 0, int16_t* hOut = 0);
void centerLeftString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut = 0, int16_t* hOut = 0);
void centerStringWithImage(BufferedDisplay* g, Image* image, const char* str,  int16_t x, int16_t y, int16_t* wOut = 0, int16_t* hOut = 0, bool useOpacityWithImage = false);
void drawMultilineCenteredText(BufferedDisplay* g, const String& text, int x, int y, int maxWidth, int lineHeight, int* w = 0, int* h = 0);
void retro_drawNavigationBar(BufferedDisplay* g, MenuHost* host, int y, Image* previousIcon, Image* icon, const char* text, Image* nextIcon, Color color);
void retro_drawTitleBar(BufferedDisplay* g, MenuHost* host, int y, Image* icon, const char* text, Color color);

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