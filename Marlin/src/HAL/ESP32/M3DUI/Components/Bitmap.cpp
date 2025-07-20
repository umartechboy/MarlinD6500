#include "Bitmap.h"


Image::Image(const uint8_t* bitmapDataPGM, int16_t xOffset, int16_t yOffset):data((byte*)bitmapDataPGM){
    this->xo = xOffset;
    this->yo = yOffset;
    if (data == 0)
        return;
    uint8_t palleteSize = pgm_read_byte(data + 2);
    for (int pi = 0; pi < palleteSize; pi++)
    {
        uint16_t color = ((uint16_t)pgm_read_byte(data + 3 + pi * 2 + 0) << 8) + ((uint16_t)pgm_read_byte(data + 3 + pi * 2 + 1) << 0);
        pallete[pi] = color;
    }
}
uint8_t Image::width(){
    if (data)
        return pgm_read_byte(data);
    return 0;
}
uint8_t Image::height(){
    if (data)
        return pgm_read_byte(data + 1);
    return 0;

}
void Image::DrawCentered(BufferedDisplay* g, int x, int y){
    Draw(g, x - width() / 2 - xo, y - height() / 2 - yo, false, true);
}
void Image::Draw(BufferedDisplay* g, int x, int y, bool invertOffset, bool useDisplayOffset, bool useDisplayOpacity){
    int w = width();
    int h = height();
    x += invertOffset?-xo:xo;
    y += invertOffset?-yo:yo;
    uint8_t palleteSize = pgm_read_byte(data + 2);
    int xofBkp = g->xOffset;
    int yofBkp = g->yOffset;
    g->xOffset = x;
    g->yOffset = y;
    if (useDisplayOffset){
        g->xOffset += xofBkp;
        g->yOffset += yofBkp;
    }
    int opBkp = g->GetOpacity();
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
        {
            byte pData = pgm_read_byte(data + j * w + i + 3 + palleteSize * 2);
            byte cIndex = (byte)((pData >> 4) & 0xF);
            byte op = (byte)(pData & 0xF);
            uint16_t c = pallete[cIndex];
            if (useDisplayOpacity){
                // Opacity is 4 bit here.
                g->SetOpacity((((op * 100) / 15) * opBkp) / 100);
            }
            else
                g->SetOpacity((op * 100) / 15);
            g->drawPixel(i, j, c);
        }
    g->xOffset = xofBkp;
    g->yOffset = yofBkp;
    g->SetOpacity(opBkp);
}
int16_t Image::xOffset(){
    return xo;
}
int16_t Image::yOffset(){
    return yo;
}
