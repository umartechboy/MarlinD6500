#include "BufferedDisplay.h"
void (*HW_startWrite)() = 0;
void (*HW_setAddressWindow)(int x, int y, int width, int height) = 0;
void (*HW_writePixels)(uint16_t *colors, uint16_t count) = 0;
void (*HW_endWrite)() = 0;

#define _swap_(a, b) \
    (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation

uint16_t *screenBuffer;


BufferedDisplay::BufferedDisplay(
    Adafruit_GFX &hw,
    void (*startWrite)(),
    void (*setAddressWindow)(int x, int y, int width, int height),
    void (*writePixels)(uint16_t *colors, uint16_t count),
    void (*endWrite)()) : Adafruit_GFX(hw.width(), hw.height())
{
    this->HW = &hw;
    HW_startWrite = startWrite;
    HW_setAddressWindow = setAddressWindow;
    HW_writePixels = writePixels;
    HW_endWrite = endWrite;
    //SERIAL_IMPL.printf("BufferedDisplay constructor: %d x %d\n", hw.width(), hw.height());
    screenBuffer = new uint16_t[hw.width() * hw.height()];
    for (int i = 0; i < hw.width() * hw.height(); i++)
    {
        screenBuffer[i] = 0xFFFF; // White
    }
}

void BufferedDisplay::drawPixel(int16_t x, int16_t y, Color color){
    if (color.a() == 0x00)
        return;
    else if (color.a() == 0xFF)
        drawPixel(x,y, color.toColor16());
    else 
    {
        Color existing = readPixel(x, y);
        existing.BlendBelow(color);
        drawPixel(x, y, existing.toColor16());
    }
}
void BufferedDisplay::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    //Serial.printf("drawPixel(%d x %d) = %d\n", x, y, color);
    x += xOffset;
    y += yOffset;
    if (drawOpacity == 0)
        return;
    else if (drawOpacity != 100){
        // Use mixing
        Color c(color);
        c.a(((int)drawOpacity * 255) / 100);
        Color e = readPixel(x - xOffset, y - yOffset); // need to remove the doule offseting
        e.BlendBelow(c);
        //color = 0b11111100000;
        color = e.toColor16();
    }
    // else, Opaque. No need to mix
    if ((x >= 0) && (x < width()) && (y >= 0) && (y < height()))
    {
        // Pixel is in-bounds. Rotate coordinates if needed.
        switch (getRotation())
        {
        case 1:
            _swap_(x, y);
            x = WIDTH - x - 1;
            break;
        case 2:
            x = WIDTH - x - 1;
            y = HEIGHT - y - 1;
            break;
        case 3:
            _swap_(x, y);
            y = HEIGHT - y - 1;
            break;
        }
        if (screenBuffer[x + y * WIDTH] != color)
        {
            // Serial.printf("screenBuffer[%d] = %d\n", x + y * WIDTH, color);
            screenBuffer[x + y * WIDTH] = color; // we never give 16 bit colors to this functions
            updateRequired = true;

            if (x < updateX0)
            {
                updateX0 = x;
                // Serial.printf("updateX0 set to %d\n", updateX0);
            }
            if (x > updateX1)
            {
                updateX1 = x;
                // Serial.printf("updateX1 set to %d\n", updateX1);
            }
            if (y < updateY0)
            {
                updateY0 = y;
                // Serial.printf("updateY0 set to %d\n", updateY0);
            }
            if (y > updateY1)
            {
                updateY1 = y;
                // Serial.printf("updateY1 set to %d\n", updateY1);
            }
        }
    }
}
uint16_t BufferedDisplay::readPixel(int16_t x, int16_t y)
{
    x += xOffset;
    y += yOffset;
    if ((x >= 0) && (x < width()) && (y >= 0) && (y < height()))
    {
        // Pixel is in-bounds. Rotate coordinates if needed.
        switch (getRotation())
        {
        case 1:
            _swap_(x, y);
            x = WIDTH - x - 1;
            break;
        case 2:
            x = WIDTH - x - 1;
            y = HEIGHT - y - 1;
            break;
        case 3:
            _swap_(x, y);
            y = HEIGHT - y - 1;
            break;
        }
        return screenBuffer[x + y * WIDTH];
    }
    return 0;
}
void BufferedDisplay::update(bool forceFullWidth, bool forceFullHeight)
{
    long st = millis();
    InUpdate = true;
    if (forceFullWidth)
    {
        //Serial.println("Force full width");
        updateX0 = 0;
        updateX1 = HW->width() - 1;
    }
    if (forceFullHeight)
    {
        //Serial.println("Force full Height");
        updateY0 = 0;
        updateY1 = HW->height() - 1;
    }
    if (updateX1 < updateX0 || updateY1 < updateY0)
    {
        //Serial.println("Nothing to update");
        return;
    }
    int updateX0 = this->updateX0;
    int updateX1 = this->updateX1;
    int updateY0 = this->updateY0;
    int updateY1 = this->updateY1;
    int w = updateX1 - updateX0 + 1;
    int h = updateY1 - updateY0 + 1;
    //Serial.printf("Update (%d, %d, %d, %d)\n", updateX0, updateY0, w, h);
    if (w == HW->width()) {// Dump whole color buffer at once
        //Serial.println("Update full width");
        HW_startWrite();
        HW_setAddressWindow(0, updateY0, w, h);
        HW_writePixels(screenBuffer + w * updateY0, w * h);
        HW_endWrite();
    }
    else {
        //Serial.println("Update partial width");
        for (int yi = 0; yi < h; yi++)
        {
            //Serial.printf("Update line (%d, %d, %d, %d)\n", updateX0, updateY0 + yi, w, 1);
            HW_startWrite();
            HW_setAddressWindow(updateX0, updateY0 + yi, w, 1);
            HW_writePixels(screenBuffer + HW->width() * (updateY0 + yi) + updateX0, w);
            HW_endWrite();
        }
    }
    this->updateX0 = HW->width();
    this->updateX1 = -1;
    this->updateY0 = HW->height();
    this->updateY1 = -1;
    InUpdate = false;
    //Serial.print("update took: ");
    //Serial.println(millis() - st);
}

uint8_t BufferedDisplay::GetOpacity()
{
    return drawOpacity;
}
// 0-100
void BufferedDisplay::SetOpacity(uint8_t opacity)
{
    drawOpacity = opacity;
    if (drawOpacity > 100)
        drawOpacity = 100;
}