#ifndef __BITMAP__
#define __BITMAP__

#include <Arduino.h>
#include "..\Hardware\BufferedDisplay.h"

class Image

{
    private:
        const uint8_t* data;
        uint16_t pallete[16];
        int16_t xo = 0, yo = 0;
    public:
        Image(const uint8_t* bitmapDataPGM = 0, int16_t xOffset = 0, int16_t yOffset = 0);
        uint8_t width();
        uint8_t height();
        int16_t xOffset();
        int16_t yOffset();
        void Draw(BufferedDisplay* g, int x, int y, bool invertOffset = false, bool useDisplayOffset = false);
        void DrawCentered(BufferedDisplay* g, int x, int y);
};

#endif