#include "utils.h"
#include "Bitmap.h"
#include "M3DUI.h"


// Utilities
void centerStringWithImage(BufferedDisplay* g, Image* image, const char* str,  int16_t x, int16_t y, int16_t* wOut, int16_t* hOut) {
    int16_t x1, y1;
    uint16_t w, h;
    g->setTextWrap(false);
    g->getTextBounds(str, x, y, &x1, &y1, &w, &h);
    w += 2 + image->width();
    int16_t errorInX = x1 - x;
    int16_t errorInY = y1 - y;
    g->setCursor(x - w / 2 - errorInX, y - h / 2 - errorInY);
    g->print(str);
    image->Draw(g, x - w / 2, y, false, true);
    if (wOut)
        *wOut = w;
    if (hOut)
        *hOut = h;
}
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

void retro_singleMenuOption(BufferedDisplay* g, MenuHost* host, Image* icon, const char* text, Color color){           
        uint8_t opBkp = g->GetOpacity();
        uint8_t lineMargin = 10;
        uint8_t tempSectionHeight = 20;
        g->SetOpacity(10);
        for (int i =0; i < 5; i++)
            g->drawLine(lineMargin + i * 2, host->appHeight() - tempSectionHeight, host->appWidth() - lineMargin - i * 2, host->appHeight() - tempSectionHeight, color);
        g->SetOpacity(opBkp);
        //g->drawLine(host->appWidth() / 2, host->appHeight() - tempSectionHeight, host->appWidth() / 2, host->appHeight(), g->readPixel(host->appWidth() / 2, host->appHeight() - tempSectionHeight));            
        centerStringWithImage(g, icon, text, host->appWidth() / 2, host->appHeight() + 1 - tempSectionHeight / 2);
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

