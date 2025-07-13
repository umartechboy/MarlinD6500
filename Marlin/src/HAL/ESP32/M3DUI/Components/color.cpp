#include "color.h"

Color::Color (uint16_t color) {
    _r = ((color >> 11) & 0b11111) * 255 / 0b011111;
    _g = ((color >> 5) & 0b111111) * 255 / 0b111111;
    _b = ((color >> 0) & 0b11111) * 255 / 0b011111;
    _a = 0xFF;
}
// call the one with uint16_t
Color::Color (int color): Color((uint16_t)(color & 0xFFFF)) {
}
Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    this->_r = r;
    this->_g = g;
    this->_b = b;
    this->_a = a;
}
    
    // cast #FFFFFF format strig directly to this color class
Color::Color(const char* hexColor) {
        uint32_t color = strtoul(hexColor, NULL, 16);
        _r = ((color >> 16) & 0xFF);
        _g = ((color >> 8) & 0xFF);
        _b = ((color >> 0) & 0xFF);
        _a = 0xFF;
}
void Color::BlendOn(Color cToBlend) {
    // Convert alpha values to float (range 0 to 1)
    float fOverlayAlpha = static_cast<float>(this->a()) / 255.f;
    float fColorAlpha = static_cast<float>(cToBlend.a()) / 255.f;

    // Blend the RGB channels based on the alpha values
    this->R(static_cast<uint8_t>(fOverlayAlpha * this->R() + fColorAlpha * (1 - fOverlayAlpha) * cToBlend.R()));
    this->G(static_cast<uint8_t>(fOverlayAlpha * this->G() + fColorAlpha * (1 - fOverlayAlpha) * cToBlend.G()));
    this->B(static_cast<uint8_t>(fOverlayAlpha * this->B() + fColorAlpha * (1 - fOverlayAlpha) * cToBlend.B()));

    // Blend the alpha channel
    this->a(static_cast<uint8_t>(255.f * (fOverlayAlpha + fColorAlpha * (1 - fOverlayAlpha))));

}
void Color::BlendBelow(Color cToBlend) {
    // Convert alpha values to float (range 0 to 1)
    float fOverlayAlpha = static_cast<float>(cToBlend.a()) / 255.f;
    float fColorAlpha = static_cast<float>(this->a()) / 255.f;

    // Blend the RGB channels based on the swapped alpha values
    this->R(static_cast<uint8_t>(fOverlayAlpha * cToBlend.R() + fColorAlpha * (1 - fOverlayAlpha) * this->R()));
    this->G(static_cast<uint8_t>(fOverlayAlpha * cToBlend.G() + fColorAlpha * (1 - fOverlayAlpha) * this->G()));
    this->B(static_cast<uint8_t>(fOverlayAlpha * cToBlend.B() + fColorAlpha * (1 - fOverlayAlpha) * this->B()));

    // Blend the alpha channel
    this->a(static_cast<uint8_t>(255.f * (fOverlayAlpha + fColorAlpha * (1 - fOverlayAlpha))));
}

Color DarkPaleYellow= rgb(255, 180, 0);
Color LightPaleYellow = rgb(255, 231, 173);
Color DarkOceanBlue = rgb(15, 113, 115);
Color LightOceanBlue = rgb(219, 250, 250);
Color DarkPurple = rgb(107, 45, 92);
Color LightPurple = rgb(237, 212, 231);
Color DarkRed = rgb(113, 11, 9);
Color MediumRed = rgb(242, 95, 92);
Color LightRed = rgb(250, 200, 198);
Color GoGreen = rgb(77, 194, 30);
Color OffWhite = rgb(221, 221, 218);
Color Charcoal = rgb(27, 37, 37);
