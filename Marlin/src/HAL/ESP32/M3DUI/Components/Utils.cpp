#include "utils.h"
#include "Bitmap.h"
#include "M3DUI.h"


// Utilities
void centerStringWithImage(BufferedDisplay* g, Image* image, const char* str,  int16_t x, int16_t y, int16_t* wOut, int16_t* hOut) {
    int16_t x1, y1;
    uint16_t w, h;
    int padding = 2;
    g->setTextWrap(false);
    g->getTextBounds(str, x, y, &x1, &y1, &w, &h);
    int imgWidth = 0;
    if (image) imgWidth = image->width();
    w += padding + imgWidth;
    int16_t errorInX = x1 - x;
    int16_t errorInY = y1 - y;
    g->setCursor(x - w / 2 - errorInX + imgWidth + padding, y - h / 2 - errorInY);
    g->print(str);
    if (image)
        image->Draw(g, x - w / 2, y - image->height() / 2, false, true);
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
void drawMultilineCenteredText(BufferedDisplay* g, const String& text, int x, int y, int maxWidth, int lineHeight, int* _w, int* _h) {
    g->setTextWrap(false); // We'll manually handle wrapping

    // Step 1: Break text into words
    std::vector<String> words;
    int start = 0;
    for (int i = 0; i <= text.length(); i++) {
        if (i == text.length() || text[i] == ' ') {
            words.push_back(text.substring(start, i));
            start = i + 1;
        }
    }

    // Step 2: Assemble lines that fit within maxWidth
    std::vector<String> lines;
    String currentLine = "";
    for (size_t i = 0; i < words.size(); i++) {
        String testLine = currentLine.length() ? currentLine + " " + words[i] : words[i];

        int16_t bx, by;
        uint16_t bw, bh;
        g->getTextBounds(testLine, 0, 0, &bx, &by, &bw, &bh);

        if (bw <= maxWidth) {
            currentLine = testLine;
        } else {
            if (currentLine.length()) lines.push_back(currentLine);
            currentLine = words[i];
        }
    }
    if (currentLine.length()) lines.push_back(currentLine);

    // Step 3: Calculate total block height
    int totalHeight = lines.size() * lineHeight;

    // Step 4: Draw each line centered at (x, y)
    int currentY = y - totalHeight / 2 + lineHeight / 2;
    if (_w)
        *_w = 0;
    for (size_t i = 0; i < lines.size(); i++) {
        const String& line = lines[i];
        int16_t x1, y1;
        uint16_t w, h;
        g->getTextBounds(line, x, currentY, &x1, &y1, &w, &h);
        
        if (_w){
            if (w > (*_w))
                *_w = w;
        }
        int16_t errorInX = x1 - x;
        int16_t errorInY = y1 - currentY;
        g->setCursor(x - w / 2 - errorInX, currentY - h / 2 - errorInY);
        g->print(line);
        currentY += lineHeight;
    }
    if (_h){
        *_h = lines.size() * lineHeight;
    }
}


void centerLeftString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut, int16_t* hOut) {
    int16_t x1, y1;
    uint16_t w, h;
    g->setTextWrap(false);
    g->getTextBounds(str, x, y, &x1, &y1, &w, &h);
    int16_t errorInX = x1 - x;
    int16_t errorInY = y1 - y;
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

int retroNavSectionHeight = 18;
int retroTitleSectionHeight = 16;
void retro_optionsBar(BufferedDisplay* g, MenuHost* host, int y, Image* previousIcon, Image* icon, const char* text, Image* nextIcon, Color color){           
    uint8_t opBkp = g->GetOpacity();
    g->setFont();
    uint8_t lineMargin = 10;
    g->SetOpacity(10);
    for (int i =0; i < 5; i++)
        g->drawLine(lineMargin + i * 2, y, host->appWidth() - lineMargin - i * 2 - 6, y, color);
    g->SetOpacity(100);
    int iconSpaceLeft = 0;
    int iconSpaceRight = 0;
    int imgPad = 2;
    if (previousIcon){
        previousIcon->Draw(g, imgPad, y + retroNavSectionHeight / 2 - previousIcon->height() / 2, false, true);
        g->SetOpacity(20);
        g->drawLine(imgPad + previousIcon->width() + 1, y, imgPad + previousIcon->width() + 1, y + retroNavSectionHeight, color);        
        g->SetOpacity(100);
        iconSpaceLeft += previousIcon->width() + imgPad * 2;
        
    }
    if (nextIcon){
        nextIcon->Draw(g, g->width() - imgPad * 2 - nextIcon->width(), y + retroNavSectionHeight / 2 - nextIcon->height() / 2, false, true);
        g->SetOpacity(20);
        g->drawLine(g->width() - imgPad - 1 - nextIcon->width(), y, g->width() - imgPad - 1 - nextIcon->width(), y + retroNavSectionHeight, color);        
        g->SetOpacity(100);
        iconSpaceRight += nextIcon->width() + imgPad * 2;
    }
    g->setTextColor(color);
    centerStringWithImage(g, icon, text, iconSpaceLeft + (g->width() / 2 - iconSpaceLeft - iconSpaceRight), y + retroNavSectionHeight / 2);
    g->SetOpacity(opBkp);
}
void retro_titleBar(BufferedDisplay* g, MenuHost* host, int y, Image* icon, const char* text, Color color){              
    uint8_t opBkp = g->GetOpacity();
    g->setFont();
    uint8_t lineMargin = 10;
    g->SetOpacity(10);
    for (int i = 0; i < 5; i++)
        g->drawLine(lineMargin + i * 2, y + retroTitleSectionHeight, host->appWidth() - lineMargin - i * 2 - 6, y + retroTitleSectionHeight, color);
    g->SetOpacity(100);
    //g->drawLine(host->appWidth() / 2, host->appHeight() - tempSectionHeight, host->appWidth() / 2, host->appHeight(), g->readPixel(host->appWidth() / 2, host->appHeight() - tempSectionHeight));            
    g->setTextColor(color);
    centerStringWithImage(g, icon, text, g->width() / 2, y + retroTitleSectionHeight / 2);
    g->SetOpacity(opBkp);
}

// Zero Transitions from f1 to f2
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

