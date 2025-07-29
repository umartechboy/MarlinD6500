#include "FilePreviewStep.h"
#include <SD.h>
#include "..\MenuApp.h"
#include "..\Images.h"
#include "..\MenuApp.h"
#include "..\..\..\..\..\sd\cardreader.h"

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
    Icon = &img_PrintPreview;    
    PreviousStep = &sdMenuStep;
    NextStep = &printPositionStep;
    RetroPreviousStep = &sdMenuStep;
    RetroNextStep = &printPositionStep;
    NextActionString = "Continue";
}
void FilePreviewStep::Paint(BufferedDisplay* g) {
    //Serial.printf("File Preview Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the file preview screen
    g->fillScreen(BackColor);
    g->setTextColor(TextColor);
    int y = 0;
    int lineHeight = 12; 
    if (pngParams.thumbnailData.bytesDecoded){
        pngParams.g = g;
        pngParams.x = Host->appWidth() / 2;
        pngParams.y = y;
        png.decode((void*)(&pngParams), 0);
        y += 55; // can't rely on PNG data.
    }
    else{
        centerString(g, "No file preivew", Host->appWidth() / 2, lineHeight); // some margin at the top too
        y += lineHeight / 2;
    }
    uint8_t opBkp = g->GetOpacity();
    uint8_t lineMargin = 10;
    g->SetOpacity(10);
    y += 4;
    for (int i =0; i < 5; i++)
        g->drawLine(lineMargin + i * 2, y, Host->appWidth() - lineMargin - i * 2, y, TextColor);
    y += lineHeight / 2 + 1;
    int divider = 55;
    int margin = 2;
    g->setFont();
    g->setTextColor(TextColor);
    g->SetOpacity(100);
    if (estimatedPrintingTime.length() > 0){
        centerRightString(g, "Time", divider - margin, y);   
        centerLeftString(g, estimatedPrintingTime.c_str(), divider + margin + 1, y);
        g->SetOpacity(10);
        g->drawLine(divider, y - lineHeight / 2, divider, y - lineHeight / 2 + lineHeight, TextColor);
        y += lineHeight;
    }
    if (filamentUsed_mm > 0 || filamentUsed_g > 0){
        String fu  = String(filamentUsed_mm / 1000.0F, 1) + "m";
        if (filamentUsed_mm <= 0)
            fu = "";
        if (filamentUsed_g > 0){
            if (filamentUsed_mm > 0)
                fu += " (";
            fu += String(filamentUsed_g , 0) + "g";
            if (filamentUsed_mm > 0)
                fu += ")";
        }
        g->SetOpacity(100);
        centerRightString(g, "Material", divider - margin, y);   
        centerLeftString(g, fu.c_str(), divider + margin + 1, y);        
        g->SetOpacity(10);
        g->drawLine(divider, y - lineHeight / 2, divider, y - lineHeight / 2 + lineHeight, TextColor);
        y += lineHeight;
    }
    if (filament0Used || filament1Used){
        g->SetOpacity(100);
        if (filamentUsed_mm <= 0 && filamentUsed_g <= 0) {// We already haven't written Material
            centerRightString(g, "Material", divider - margin, y);   
        }
        int x = divider + margin;
        if (filament0Used) {
            g->fillRoundRect(x, y - 4, 8, 8, 1, e0Color);
            x += 12;
        }
        if (filament1Used) {
            g->fillRoundRect(x, y - 4, 8, 8, 1, e1Color);
        } 
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
    SERIAL_IMPL.printf("Begin SD Read: %s\n",  mainScreenStep.fileName.c_str());
    pngParams.thumbnailData.Reset();
    File f = SD.open(mainScreenStep.fileName);
    bool inThumbnail = false;
    bool isMarlin = false;
    if (f.available()){
        SERIAL_IMPL.printf("File opened for thumbnail: %s\n", mainScreenStep.fileName.c_str());
    }
    bool weAreNearTheEnd = false;
    while (f.available())
    {
        bool parsed = true;
        String line = f.readStringUntil('\n');
        if (line.startsWith("G1"))
        {
            if(isMarlin){                
                SERIAL_IMPL.println("G1. Skipping the rest.");
                break;
            }
            if (!weAreNearTheEnd){
                weAreNearTheEnd = true;
                // Lets seek near the end
                SERIAL_IMPL.println("Seeking near the end");
                if (f.size() < 20000){
                    SERIAL_IMPL.printf("File not big enough: %d\n", f.size());
                    break;
                }
                f.seek(f.size() - 20000, SeekMode::SeekSet);
                continue;
            }
        }
        if (line.startsWith(";")){ // its a comment.
            line = line.substring(1);
            if (line.startsWith(" ") || line.startsWith("\t"))
                line = line.substring(1);
            if (line.startsWith("thumbnail begin")){
                
                SERIAL_IMPL.printf("Thumbnail found @: %s\n", line.c_str());
                getTrimmedAfter(line, " "); // begin 120x60 5892
                getTrimmedAfter(line, " "); // 120x60 5892
                pngParams.pngWidth = line.substring(0, line.indexOf("x")).toInt();
                getTrimmedAfter(line, " "); // 60 5892
                pngParams.pngHeight = line.substring(0, line.indexOf("x")).toInt();
                inThumbnail = true;
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
            }
            else if (inThumbnail) {
                // Serial.print("Feeding thumbnail data: ");
                // Serial.println(line);
                for (int i = 0; i < line.length(); i++){
                    pngParams.thumbnailData.Feed(line[i]);
                }
            }
            else {
                line.toLowerCase();
                if (line.startsWith("flavor:marlin")){
                    isMarlin = true;
                }
                else if (line.startsWith("minx")){
                    getTrimmedAfter(line, ":");
                    mainScreenStep.minX = line.toInt();
                }
                else if (line.startsWith("maxx")){
                    getTrimmedAfter(line, ":");
                    mainScreenStep.maxX = line.toInt();
                }
                else if (line.startsWith("miny")){
                    getTrimmedAfter(line, ":");
                    mainScreenStep.minY = line.toInt();
                }
                else if (line.startsWith("maxy")){
                    getTrimmedAfter(line, ":");
                    mainScreenStep.maxY = line.toInt();
                }
                else if (line.startsWith("max_z_height") || line.startsWith("maxz:")){
                    getTrimmedAfter(line, ":");
                    maxZ = line.toInt();
                }
                else if (line.startsWith("total layer number") || line.startsWith("layer_count")){
                    getTrimmedAfter(line, ":");
                    layerCount = line.toInt();
                }
                else if (line.startsWith("time")){ // Cura
                    getTrimmedAfter(line, ":");
                    long printTime = line.toInt();
                    
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
                    estimatedPrintingTime = timeStr;
                }
                else if (line.startsWith("filament used [mm]")){ // Orca
                    getTrimmedAfter(line, "=");
                    filamentUsed_mm = line.toFloat();
                }
                else if (line.startsWith("total filament used [g]")){ // Orca
                    getTrimmedAfter(line, "=");
                    filamentUsed_g = line.toFloat();
                }
                else if (line.startsWith("used_0")){ // Orca
                    getTrimmedAfter(line, ":");
                    filament0Used = line == "true";
                }
                else if (line.startsWith("used_1")){ // Orca
                    getTrimmedAfter(line, ":");
                    filament1Used = line == "true";
                }
                else if (line.startsWith("filament used:")){ // Cura
                    getTrimmedAfter(line, ":");
                    getTrimmedAfter(line, " ");
                    getTrimmedBefore(line, "m");
                    filamentUsed_mm = line.toFloat() * 1000.0F;
                }
                else if (line.startsWith("estimated printing time")){   // Orca
                    //SERIAL_IMPL.printf("estimated printing time: %s\n", line.c_str());
                    getTrimmedAfter(line, "=");
                    line.trim();
                    estimatedPrintingTime = line;
                }
                else if (line.startsWith("config_block_start")){        
                    Serial.println("Orca header end");
                    break;
                }
                else {
                    //SERIAL_IMPL.printf("Unparsed (1): %s\n", line.c_str());
                    parsed = false;
                }
            }
        }
        else {
            //SERIAL_IMPL.printf("Unparsed (2): %s\n", line.c_str());
            parsed = false;
        }
        if (parsed)
            SERIAL_IMPL.printf("Parsed: %s\n", line.c_str());
    }    
    f.close();
}
void FilePreviewStep::HandleKeyUp(Keys key){    
    if (!Host->Retro){
        if (key == Keys::KEYPAD_RIGHT)
            Host->GotoNextStep();
        else if (key == Keys::KEYPAD_LEFT)
            Host->GotoPreviousStep();
    }
}
void FilePreviewStep::LoadComplete(){
    // in case the menu is coming back from the print position step, we need to notify the home screen
    mainScreenStep.printStatus = PrintStatus::Idle;
    Preferences prefs;
    prefs.begin("material");
    e0Color = AvailableColors[prefs.getInt("e1_c", 0)];
    e1Color = AvailableColors[prefs.getInt("e2_c", 1)];
    prefs.end();
}