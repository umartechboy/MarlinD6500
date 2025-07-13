#include "utils.h"


// Utilities
void centerString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut, int16_t* hOut) {
    int16_t x1, y1;
    uint16_t w, h;
    g->setTextWrap(false);
    g->getTextBounds(str, x, y, &x1, &y1, &w, &h);
    int16_t errorInX = x1 - x;
    int16_t errorInY = y1 - y;
    //g->drawRect(x, y, w, h, ST7735_CYAN);
    //g->drawRect(x1, y1, w, h, ST7735_YELLOW);
    g->setCursor(x - w / 2 - errorInX, y - h / 2 - errorInY);
    g->print(str);
    //g->SetOpacity(30);
    //g->setCursor(x, y);
    //g->print(str);
    //g->SetOpacity(100);
    //g->print(str);
    if (wOut)
        *wOut = w;
    if (hOut)
        *hOut = h;
}
void centerRightString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut, int16_t* hOut) {
    int16_t x1, y1;
    uint16_t w, h;
    g->setTextWrap(false);
    g->getTextBounds(str, x, y, &x1, &y1, &w, &h);
    int16_t errorInX = x1 - x;
    int16_t errorInY = y1 - y;
    //g->drawRect(x, y, w, h, ST7735_CYAN);
    //g->drawRect(x1, y1, w, h, ST7735_YELLOW);
    g->setCursor(x - w - errorInX, y - h / 2 - errorInY);
    g->print(str);
    //g->SetOpacity(30);
    //g->setCursor(x, y);
    //g->print(str);
    //g->SetOpacity(100);
    //g->print(str);
    if (wOut)
        *wOut = w;
    if (hOut)
        *hOut = h;
}

void centerLeftString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut, int16_t* hOut) {
    int16_t x1, y1;
    uint16_t w, h;
    g->setTextWrap(false);
    g->getTextBounds(str, x, y, &x1, &y1, &w, &h);
    int16_t errorInX = x1 - x;
    int16_t errorInY = y1 - y;
    //g->drawRect(x, y, w, h, ST7735_CYAN);
    //g->drawRect(x1, y1, w, h, ST7735_YELLOW);
    g->setCursor(x - 0 - errorInX, y - h / 2 - errorInY);
    g->print(str);
    //g->SetOpacity(30);
    //g->setCursor(x, y);
    //g->print(str);
    //g->SetOpacity(100);
    //g->print(str);
    if (wOut)
        *wOut = w;
    if (hOut)
        *hOut = h;
}


float MixFloats(float f1, float f2, float p, MixType mt)
{
    simplifyMixType(p, mt);
    return f2 * p + f1 * (1 - p);
}

void simplifyMixType(float& p, MixType& mt)
{
    if (mt == MixType::Retract)
    {
        p = 2.0f * (float)(0.5 - abs(0.5 - p)); mt = MixType::Linear;
    }
    else if (mt == MixType::EaseIn)
    {
        p *= p * p;
        mt = MixType::Linear;
    }
    else if (mt == MixType::EaseOut)
    {
        p = (float)pow(p, 0.3);
        mt = MixType::Linear;
    }
    else if (mt == MixType::EaseInEaseOut)
    {
        if (p <= 0.5)
            p *= p * 2;
        else
        {
            p -= 0.5f;
            p = (float)pow(p, 0.5) / (float)pow(0.5, 0.5) * 0.5f;
            p += 0.5f;
        }
        mt = MixType::Linear;
    }
}

