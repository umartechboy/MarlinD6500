#include "FilePreviewStep.h"
#include <SD.h>
#include "..\MenuApp.h"
#include "..\Bitmaps.h"

#define getTrimmedAfter(str, chr) str = str.substring(line.indexOf(chr) + 1)
#define getTrimmedBefore(str, chr) str = str.substring(0, line.indexOf(chr))


PNG png;

void PNGDraw(PNGDRAW *pDraw);

void PNGDraw(PNGDRAW *pDraw)
{
    pngDecodeParams* params = (pngDecodeParams*)pDraw->pUser;
    BufferedDisplay* g = params->g;
    uint16_t usPixels [128];
    uint8_t ucMask [16];
    png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_LITTLE_ENDIAN, 0xffffffff);
    if (png.getAlphaMask(pDraw, ucMask, 255)) { // if any pixels are opaque, draw them
        for (int i = 0 ; i < pDraw->iWidth; i++){
            if (ucMask[i / 8] >> (7 - (i % 8)) && usPixels[i]) {
                g->drawPixel(- params->pngWidth / 2 + params->x + i, params->y + pDraw->y, usPixels[i]);
            }
        }

        //spilcdWritePixelsMasked(&lcd, pPriv->xoff, pPriv->yoff + pDraw->y, (uint8_t *)usPixels, ucMask, pDraw->iWidth, DRAW_TO_LCD);
    }
} /* PNGDraw() */

FilePreviewStep::FilePreviewStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkPurple;
    BackColor = DarkPurple;
    TextColor = ST7735_WHITE;
    Icon = &bmp_PrintPreview;
}
void FilePreviewStep::Paint(BufferedDisplay* g) {
    //Serial.printf("File Preview Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the file preview screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    int y = 0;
    int lineHeight = 14; 
    if (pngParams.thumbnailData.bytesDecoded){
        pngParams.g = g;
        pngParams.x = g->width() / 2;
        pngParams.y = y;
        png.decode((void*)(&pngParams), 0);
        y += pngParams.pngHeight;
    }
    else{
        centerString(g, "No file preivew", g->width() / 2, lineHeight); // some margin at the top too
        y += lineHeight / 2;
    }
    uint8_t opBkp = g->GetOpacity();
    uint8_t lineMargin = 10;
    g->SetOpacity(10);
    y += 4;
    for (int i =0; i < 5; i++)
        g->drawLine(lineMargin + i * 2, y, g->width() - lineMargin - i * 2, y, TextColor);
    y += 4 + lineHeight;
    int divider = 55;
    int margin = 2;
    g->setFont();
    g->setTextColor(TextColor);
    if (time > 0){
        String h  = String(printTime / 3600);
        String m  = String((printTime / 60) % 60);
        String s  = String((printTime / 1) % 60);
        if (m.length() == 1)
            m = "0" + m;
        if (s.length() == 1)
            s = "0" + s;
        String timeStr = s + "s";
        if (printTime >= 60)
            timeStr = m + "m " + timeStr;
        if (printTime >= 3600)
            timeStr = h + "h, " + timeStr;
        g->SetOpacity(100);
        centerRightString(g, "Time", divider - margin, y);   
        centerLeftString(g, timeStr.c_str(), divider + margin + 1, y);
        g->SetOpacity(10);
        g->drawLine(divider, y - lineHeight / 2, divider, y - lineHeight / 2 + lineHeight, TextColor);
        y += lineHeight;
    }
    if (filamentUsed > 0){
        String fu  = String(filamentUsed) + "m";
        g->SetOpacity(100);
        centerRightString(g, "Material", divider - margin, y);   
        centerLeftString(g, fu.c_str(), divider + margin + 1, y);
        g->SetOpacity(10);
        g->drawLine(divider, y - lineHeight / 2, divider, y - lineHeight / 2 + lineHeight, TextColor);
        y += lineHeight;
    }
    if (layerCount > 0){
        String lc  = String(layerCount);
        g->SetOpacity(100);
        centerRightString(g, "Layers", divider - margin, y);   
        centerLeftString(g, lc.c_str(), divider + margin + 1, y);
        g->SetOpacity(10);
        g->drawLine(divider, y - lineHeight / 2, divider, y - lineHeight / 2 + lineHeight, TextColor);
        y += lineHeight;
    }
    g->SetOpacity(opBkp);
}
void FilePreviewStep::LoadBegin() {
    // Read the file and load data
    SERIAL_IMPL.printf("Begin SD Read: %s\n",  printStep.fileName.c_str());
    pngParams.thumbnailData.Reset();
    File f = SD.open(printStep.fileName);
    bool inThumbnail = false;
    if (f.available()){
        SERIAL_IMPL.printf("File opened for thumbnail: %s\n", printStep.fileName.c_str());
    }
    while (f.available())
    {
        String line = f.readStringUntil('\n');
        if (line.startsWith(";")){ // its a comment.
            line = line.substring(1);
            if (line.startsWith(" ") || line.startsWith("\t"))
                line = line.substring(1);
            if (line.startsWith("thumbnail begin")){
                
                SERIAL_IMPL.printf("Thumbnail found @: %s\n", line.c_str());
                getTrimmedAfter(line, " "); // begin 120 60 5892
                getTrimmedAfter(line, " "); // 120 60 5892
                pngParams.pngWidth = line.substring(0, line.indexOf(" ")).toInt();
                getTrimmedAfter(line, " "); // 60 5892
                pngParams.pngHeight = line.substring(0, line.indexOf(" ")).toInt();
                inThumbnail = true;
                continue;
            }
            else if (line.startsWith("thumbnail end")){                
                SERIAL_IMPL.printf("Thumbnail end @: %s, decoded: %d\n", line.c_str(), pngParams.thumbnailData.bytesDecoded);
                inThumbnail = false;
                // Serial.println("Thumbnail End!");
                png.openRAM(pngParams.thumbnailData.data, pngParams.thumbnailData.bytesDecoded, PNGDraw);
                SERIAL_IMPL.printf("png size: %dx%d\n", png.getWidth(), png.getHeight());

                // for (int i = 0 ; i < pngParams.thumbnailData.bytesDecoded; i++){
                //     if (i % 32 == 0)
                //         Serial.println();
                //     String num = String("0x") + String(pngParams.thumbnailData.data[i], 16);
                //     if (num.length() == 1)
                //         num = "0" + num;
                //     Serial.print(num + ", ");
                // }
                continue;
            }
            else if (inThumbnail){
                // Serial.print("Feeding thumbnail data: ");
                // Serial.println(line);
                for (int i = 0; i < line.length(); i++){
                    pngParams.thumbnailData.Feed(line[i]);
                }
            }
            else{
                line.toLowerCase();

                if (line.startsWith("time")){
                    getTrimmedAfter(line, ":");
                    printTime = line.toInt();
                    continue;
                }
                else if (line.startsWith("filament used")){
                    getTrimmedAfter(line, ":");
                    getTrimmedAfter(line, " ");
                    getTrimmedBefore(line, "m");
                    filamentUsed = line.toFloat();
                    continue;
                }
                else if (line.startsWith("minx")){
                    getTrimmedAfter(line, ":");
                    printStep.minX = line.toInt();
                    continue;
                }
                else if (line.startsWith("maxx")){
                    getTrimmedAfter(line, ":");
                    printStep.maxX = line.toInt();
                    continue;
                }
                else if (line.startsWith("miny")){
                    getTrimmedAfter(line, ":");
                    printStep.minY = line.toInt();
                    continue;
                }
                else if (line.startsWith("maxy")){
                    getTrimmedAfter(line, ":");
                    printStep.maxY = line.toInt();
                    continue;
                }
                else if (line.startsWith("maxz")){
                    getTrimmedAfter(line, ":");
                    maxZ = line.toInt();
                    continue;
                }
                else if (line.startsWith("layer_count")){
                    getTrimmedAfter(line, ":");
                    layerCount = line.toInt();
                    continue;
                }
                else if (line.startsWith("layer:0")){
                    Serial.println("Header end");
                    break;
                }
            }
        }
    }
    
    f.close();
}
void FilePreviewStep::HandleKeyUp(Keys key){    
    if (key == Keys::KEYPAD_RIGHT)
        Host->GotoNextStep();
    else if (key == Keys::KEYPAD_LEFT)
        Host->GotoPreviousStep();
}