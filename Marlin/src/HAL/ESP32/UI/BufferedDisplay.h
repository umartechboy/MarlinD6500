#ifndef BUFFERED_DISPLAY_H
#define BUFFERED_DISPLAY_H
#include <Adafruit_GFX.h>
#include "color.h"
#define ForceFullWidthUpdate

class BufferedDisplay : public Adafruit_GFX
{
private:
    Adafruit_GFX *HW;
    bool InUpdate = false;
    bool updateRequired = false;
    int updateX0 = 1000, updateX1 = -1, updateY0 = 1000, updateY1 = -1;
    uint8_t drawOpacity = 100;
public:
    BufferedDisplay(
        Adafruit_GFX &hw,
        void (*startWrite)(),
        void (*setAddressWindow)(int x, int y, int width, int height),
        void (*writePixels)(uint16_t *colors, uint16_t count),
        void (*endWrite)());
    int16_t xOffset = 0, yOffset = 0;
    void SetOpacity(uint8_t opacity);
    uint8_t GetOpacity();
    void drawPixel(int16_t x, int16_t y, uint16_t color) override;
    void drawPixel(int16_t x, int16_t y, Color color);
    uint16_t readPixel(int16_t x, int16_t y);
    void update(bool forceFullWidth = true, bool forceFullHeight = false);
};

#endif // BUFFERED_DISPLAY_H