#include "Bitmap.h"


Bitmap::Bitmap(const uint8_t* bitmapDataPGM, int16_t xOffset, int16_t yOffset):data((byte*)bitmapDataPGM){
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
uint8_t Bitmap::width(){
    if (data)
        return pgm_read_byte(data);
    return 0;
}
uint8_t Bitmap::height(){
    if (data)
        return pgm_read_byte(data + 1);
    return 0;

}
void Bitmap::Draw(BufferedDisplay* g, int x, int y, bool invertOffset){
    int w = width();
    int h = height();
    x += invertOffset?-xo:xo;
    y += invertOffset?-yo:yo;
    uint8_t palleteSize = pgm_read_byte(data + 2);
    int xofBkp = g->xOffset;
    int yofBkp = g->yOffset;
    g->xOffset = x;
    g->yOffset = y;
    int opBkp = g->GetOpacity();
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
        {
            byte pData = pgm_read_byte(data + j * w + i + 3 + palleteSize * 2);
            byte cIndex = (byte)((pData >> 4) & 0xF);
            byte op = (byte)(pData & 0xF);
            uint16_t c = pallete[cIndex];
            g->SetOpacity((op * 255) / 15);
            g->drawPixel(i, j, c);
        }
    g->xOffset = xofBkp;
    g->yOffset = yofBkp;
    g->SetOpacity(opBkp);
}
int16_t Bitmap::xOffset(){
    return xo;
}
int16_t Bitmap::yOffset(){
    return yo;
}
