#ifndef COLOR_H
#define COLOR_H
#include <Arduino.h>

#define rgb(r,g,b) Color(r,g,b)

// The class to convert between RGB and 565 color format
class Color{
    private:
    uint8_t _r = 0;
    uint8_t _g = 0;
    uint8_t _b = 0;
    uint8_t _a = 0xFF; // 0xFF = 100% opacity, 0x00 = 0% opacity
    public:
    Color (uint16_t color = 0);
    Color (int color); // same as uint16_t
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF);
    Color(const char* hexColor);
    uint8_t R(){
        return _r;
    }
    uint8_t G(){
        return _g;
    }
    uint8_t B(){
        return _b;
    }
    uint8_t a(){
        return _a;
    }
    void R(uint8_t value){
        _r = value;
    }
    void G(uint8_t value){
        _g = value;
    }
    void B(uint8_t value){
        _b = value;
    }
    void a(uint8_t value){
         _a = value;
    }
    uint16_t toColor16() const {
        return ((uint16_t)(_r * 0b011111) / 0xFF) << 11 |
               ((uint16_t)(_g * 0b111111) / 0xFF) << 5 |
               ((uint16_t)(_b * 0b011111) / 0xFF) << 0;
    }
    operator uint16_t() const {
        return toColor16();
    }
    void BlendOn(Color c);
    void BlendBelow(Color c);
};

extern Color DarkPaleYellow;
extern Color LightPaleYellow;
extern Color DarkOceanBlue;
extern Color LightOceanBlue;
extern Color DarkPurple;
extern Color LightPurple;
extern Color DarkRed;
extern Color MediumRed;
extern Color LightRed;
extern Color GoGreen;
extern Color OffWhite;
extern Color Charcoal;

#endif // COLOR_H
