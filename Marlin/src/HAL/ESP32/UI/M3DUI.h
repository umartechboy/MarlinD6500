#ifndef M3DUI_H
#define M3DUI_H
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7789
#include "BufferedDisplay.h"
#include "color.h"
#include "Images.h"
#include <Fonts/FreeSans12pt7b.h>
#include "Bitmap.h"
#include <SD.h>
#include "base64Stream.h"
#include <PNGdec.h>
#include "MarlinSpecific.h"

void centerString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut = 0, int16_t* hOut = 0);
void centerRightString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut = 0, int16_t* hOut = 0);
void centerLeftString(BufferedDisplay* g, const char* str,  int16_t x, int16_t y, int16_t* wOut = 0, int16_t* hOut = 0);

void UISetup();
void UILoop();

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

enum StepAnimationStage{
    MainStep = 0,
    GoingToOverLay,
    InOverlay,
    GoingToStep,
};

enum MenuTransitionStage{
    TransitionaingScreens,
    InStep,
};
enum PrintStatus{
    Idle,
    Paused,
    Printing,
    ToResume
};
enum Keys : byte {
    KEYPAD_NONE         = 0, 
    KEYPAD_RIGHT        = 1, 
    KEYPAD_UP           = 3, 
    KEYPAD_LEFT         = 5, 
    KEYPAD_DOWN         = 7, 
    KEYPAD_UP_RIGHT     = 2, 
    KEYPAD_UP_LEFT      = 4, 
    KEYPAD_DOWN_LEFT    = 6, 
    KEYPAD_DOWN_RIGHT   = 8, 
    KEYPAD_MIDDLE       = 9, 
};

class MenuStep {
   public:
        bool NeedsRedraw = true;
        virtual void Paint(BufferedDisplay* g) {}
        MenuStep* NextStep = 0;
        MenuStep* PreviousStep = 0;
        Color ButtonColor;
        Color BackColor;
        Color TextColor;
        Bitmap* Icon = 0;
        virtual void OnGoingToStep() {}
        virtual void HandleKeyUp(Keys key) {}
        virtual void IncrementValue() {}
        virtual void DecrementValue() {}
        virtual void BeginLoad() {}
        void loop(){
            if (TickPeriod == 0) return; 
            if (millis() - lastTick > TickPeriod){
                lastTick = millis();
                Tick();
            }
        }
    protected: 
        long lastTick = 0;
        long TickPeriod = 0;
    protected: 
        virtual void Tick() {}
};

extern Bitmap bmp_Back;
extern Bitmap bmp_ChangeFilament;
extern Bitmap bmp_Home;
extern Bitmap bmp_Pause;
extern Bitmap bmp_Play;
extern Bitmap bmp_SD;
extern Bitmap bmp_Utilities;
extern Bitmap bmp_StartPrint;
extern Bitmap bmp_PrintPreview;
extern Bitmap bmp_MovePrint;

class IdleScreenStep: public MenuStep {
    public:
        PrintStatus CurrentPrintStatus = PrintStatus::Idle;
        IdleScreenStep() {
            ButtonColor = DarkRed;
            BackColor = DarkRed;
            TextColor = ST7735_WHITE;            
            Icon = &bmp_Home;
            TickPeriod = 250;
        }
        void Tick() override {
            NeedsRedraw = true;
        }
        void Paint(BufferedDisplay* g) override {
            
            //Serial.printf("Idle Screen Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
            // Draw the idle screen
            g->fillScreen(BackColor);
            g->setTextColor(TextColor);
            if(CurrentPrintStatus == PrintStatus::Idle){
                g->setFont(&FreeSans12pt7b);
                centerString(g, "Ready!", g->width() / 2, g->height() / 2);
                uint8_t opBkp = g->GetOpacity();
                uint8_t lineMargin = 10;
                uint8_t tempSectionHeight = 20;
                g->SetOpacity(10);
                for (int i =0; i < 5; i++)
                    g->drawLine(lineMargin + i * 2, g->height() - tempSectionHeight, g->width() - lineMargin - i * 2, g->height() - tempSectionHeight, TextColor);
                g->SetOpacity(opBkp);
                g->drawLine(g->width() / 2, g->height() - tempSectionHeight, g->width() / 2, g->height(), g->readPixel(g->width() / 2, g->height() - tempSectionHeight));
                
                g->setFont();
                String temp1 = String(readTemp1(), 1) + " C";
                String temp2 = String(readTemp2(), 1) + " C";
                int16_t tw, th;
                centerString(g, temp1.c_str(), g->width() / 4, g->height() + 1 - tempSectionHeight / 2, &tw, &th);
                g->drawCircle(g->width() / 4 + tw / 2 - 9, g->height() + 1 - tempSectionHeight + 5, 1, TextColor);
                centerString(g, temp2.c_str(), (3 * g->width()) / 4, g->height() + 1 - tempSectionHeight / 2, &tw, &th);
                g->drawCircle((3 * g->width()) / 4 + tw / 2 - 9, g->height() + 1 - tempSectionHeight + 5, 1, TextColor);
            }
        }
        
};

extern PNG png;
struct pngDecodeParams{
    BufferedDisplay* g;
    Base64Decoder thumbnailData;
    int pngWidth;
    int pngHeight;
    int x = 0, y = 0;
};
void PNGDraw(PNGDRAW *pDraw);

class VerticalList{
    public:
        String *files;
        int filesCount = 0;
        int scrollOffset = 0;
        int targetScrollOffset = 0;
        int lineHeight = 0;
        int MaxLength = 0;
        int selected = -1;
        String EmptyString;
        VerticalList(int maxLength, int lineHeight, String emptyString = ""){
            files = new String[maxLength];
            this->MaxLength = maxLength;
            this->lineHeight = lineHeight;
            EmptyString = emptyString;
        }
        ~VerticalList(){
            if (files){
                delete [] files; 
            }
            MaxLength = 0;
            files = 0;
        }
        int getSelected(){
            return selected;
        }
        void scrollDown(){            
            targetScrollOffset -= lineHeight;
        }
        void scrollUp(){            
            targetScrollOffset += lineHeight;
        }
        void Paint(BufferedDisplay* g, Color TextColor){
            g->setTextColor(TextColor);
            if (targetScrollOffset != scrollOffset){
                scrollOffset = (scrollOffset + targetScrollOffset) / 2;
            }
            if (filesCount == 0)
                centerString(g, emptyString.c_str(), g->width() / 2, g->height() / 2);
            else{
    
                // Adjust underflow from the top
                if (scrollOffset > 0) {
                    targetScrollOffset = 0;
                    //Serial.println("Adjust underflow 1");
                }
                else if ((filesCount - 1) * lineHeight + scrollOffset < 0) {
                    targetScrollOffset = - (filesCount - 1) * lineHeight;
                    //Serial.printf("Adjust underflow 2 %d < %d\n", FileLineCenter(filesCount - 1), g->height() - lineHeight / 2);
                }
                for (int i = 0; i < filesCount; i++){
                    g->setFont();
                    //if (i * lineHeight + scrollOffset > - lineHeight / 2 && i * lineHeight + scrollOffset < lineHeight / 2){
                    int opBkp = g->GetOpacity();
                    int16_t op = (abs(i * lineHeight + scrollOffset) * 100) / g->height();
                    if (op < 0) op = 0;
                    if (op > 100) op = 100;
                    op = 100 - op;
                    // 100, 90, 80, 70, 60
                    if (op < 95){ 
                        op -= 50; 
                        if (op < 0) op = 0;
                    }
    
                    //Serial.printf("Printing SD File %d: %s @ %d\n", i, files[i].c_str(), op);
                    
                    // if (op > 100)
                    //     op = 100;
                    // op = 100 - op;
                    // if (op < 95){
                    //     op -= 50;
                    //     op *= 2;
                    // }
    
                    g->SetOpacity(op);
                    g->setTextColor(TextColor);
                    // }
                    // else
                    //     g->setTextColor(TextColor);
                    //Serial.printf("Printing SD File %d: %s @ %d, %d\n", i, files[i].c_str(), g->width() / 2, FileLineCenter(i));
                    int16_t w = 0;
                    int lY = g->height() / 2 + i * lineHeight + scrollOffset;
                    centerString(g, files[i].c_str(), g->width() / 2, lY, &w);
                    if (op > 95){
                        selected = i;
                        w = g->width() - w - 8;
                        g->drawLine(0, lY, w / 2, lY, TextColor);
                        g->drawLine(g->width(), lY, g->width() - w / 2, lY, TextColor);
                    }
                    g->SetOpacity(opBkp);
                }
            }
        }
};

class PrintStep;
class PrintPositionStep;
class FilePreviewStep;
class IdleScreenStep;
class SDMenuStep;
class MenuStep;

extern PrintStep printStep;
extern PrintPositionStep printPositionStep;
extern FilePreviewStep fileOverViewStep;
extern IdleScreenStep idleScreenStep;
extern SDMenuStep sdMenuStep;

class PrintStep : public MenuStep{
    public:
    String fileName;
    int minX = -1, maxX = -1, minY = -1, maxY = -1, maxZ = -1;
    int xOffset = 0;
    int yOffset = 0;
    PrintStep() {
        ButtonColor = DarkPurple;
        BackColor = DarkPurple;
        TextColor = ST7735_WHITE;
        Icon = &bmp_StartPrint;
    }
    void OnGoingToStep() override{
        //card.openAndPrintFile(fileName.c_str());
        // card.percentDone();
        // if (card.jobRecoverFileExists())
        //     queue.enqueue_now(F("M1000"));
        
        // queue.inject(F("M25"));                                 // Queue Pause
        // queue.inject(F("M24"));                                 // Queue resume
        // queue.inject(F("G91\nG0 Z10 F600\nG90"));                 // Raise Z soon after returning to main loop
        // card.abortFilePrintSoon();                                // There is a delay while the current steps play out
        // planner.synchronize();    
        // TERN_(HAS_STEPPER_RESET, disableStepperDrivers());          // Disable all steppers - now!
        // print_job_timer
    }
};
class PrintPositionStep : public MenuStep{
    public:
    int bedWidth = 200;
    int bedHeight = 150;
    int inc = 5;
    int sideMargin = 10;
    PrintPositionStep() {
        ButtonColor = DarkPurple;
        BackColor = DarkPurple;
        TextColor = ST7735_WHITE;
        Icon = &bmp_MovePrint;
    }
    int scaleX(int v) {
        return round((127.0F * v) / (float)bedWidth);
    }
    int scaleY(int v) {
        return round((127.0F * v) / (float)bedWidth);
    }
    int transformX(int v) {
        return scaleX(v);
    }
    int transformY(int v) {
        float fo = (127 * bedHeight) / bedWidth;
        return (127 - fo) / 2 + fo - scaleX(v);
    }
    void Paint(BufferedDisplay* g) override{
        g->fillScreen(BackColor);
        g->setFont();
        g->setTextColor(TextColor);
        centerString(g, "Use movement keys to", g->width() / 2, 7);
        centerString(g, "adjust print position", g->width() / 2, 121);
        g->drawRect(transformX(0), transformY(bedHeight), scaleX(bedWidth), scaleY(bedHeight), ST7735_RED);
        g->drawRect(transformX(sideMargin), transformY(bedHeight - sideMargin), scaleX(bedWidth - 2 * sideMargin), scaleY(bedHeight - 2 * sideMargin), ST7735_YELLOW);
        Serial.printf("fill: %dx%d, %dx%d\n",
            printStep.minX + printStep.xOffset, 
            printStep.maxY + printStep.yOffset,
            printStep.maxX - printStep.minX,
            printStep.maxY - printStep.minY);
        g->fillRect(
            transformX(printStep.minX + printStep.xOffset), transformY(printStep.maxY + printStep.yOffset), 
            scaleX(printStep.maxX - printStep.minX), scaleY(printStep.maxY - printStep.minY),
            ST7735_WHITE);
    }
    void HandleKeyUp(Keys key) override{
        switch (key)
        {
            case Keys::KEYPAD_RIGHT: printStep.xOffset += inc; break;
            case Keys::KEYPAD_LEFT: printStep.xOffset -= inc; break;
            case Keys::KEYPAD_DOWN: printStep.yOffset -= inc; break;
            case Keys::KEYPAD_UP: printStep.yOffset += inc; break;
        }
        if (printStep.minX + printStep.xOffset < inc) printStep.xOffset = inc - printStep.minX;
        if (printStep.maxX + printStep.xOffset > (bedWidth - inc)) printStep.xOffset = (bedWidth - inc) - printStep.maxX;
        if (printStep.minY + printStep.yOffset < inc) printStep.yOffset = inc - printStep.minY;
        if (printStep.maxY + printStep.yOffset > (bedHeight - inc)) printStep.yOffset = (bedHeight - inc) - printStep.maxY;
        NeedsRedraw = true;
    }
};
#define getTrimmedAfter(str, chr) str = str.substring(line.indexOf(chr) + 1)
#define getTrimmedBefore(str, chr) str = str.substring(0, line.indexOf(chr))
class FilePreviewStep: public MenuStep {
    public:
    pngDecodeParams pngParams;
    int printTime = -1;
    float filamentUsed = -1;
    int maxZ = -1;
    int layerCount = -1;
    FilePreviewStep() {
        ButtonColor = DarkPurple;
        BackColor = DarkPurple;
        TextColor = ST7735_WHITE;
        Icon = &bmp_PrintPreview;
    }
    void Paint(BufferedDisplay* g) override {
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
    void BeginLoad() override {
        // Read the file and load data
        Serial.printf("Begin SD Read: %s\n",  printStep.fileName.c_str());
        pngParams.thumbnailData.Reset();
        File f = SD.open(printStep.fileName);
        bool inThumbnail = false;
        while (f.available())
        {
            String line = f.readStringUntil('\n');
            if (line.startsWith(";")){ // its a comment.
                line = line.substring(1);
                if (line.startsWith(" ") || line.startsWith("\t"))
                    line = line.substring(1);
                if (line.startsWith("thumbnail begin")){
                    getTrimmedAfter(line, " "); // begin 120 60 5892
                    getTrimmedAfter(line, " "); // 120 60 5892
                    pngParams.pngWidth = line.substring(0, line.indexOf(" ")).toInt();
                    getTrimmedAfter(line, " "); // 60 5892
                    pngParams.pngHeight = line.substring(0, line.indexOf(" ")).toInt();
                    inThumbnail = true;
                    continue;
                }
                else if (line.startsWith("thumbnail end")){
                    inThumbnail = false;
                    // Serial.println("Thumbnail End!");
                    png.openRAM(pngParams.thumbnailData.data, pngParams.thumbnailData.bytesDecoded, PNGDraw);
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
};

class SDMenuStep: public MenuStep {
    public:
    bool hasSDCard = false;
    VerticalList list = VerticalList(64, 14);
    SDMenuStep() {
        ButtonColor = DarkPaleYellow;
        BackColor = DarkPaleYellow;
        TextColor = ST7735_BLACK;
        Icon = &bmp_SD;
        TickPeriod = 50;
    }
    void Tick() override {
        NeedsRedraw = true;
    }
    void Paint(BufferedDisplay* g) override {
        //Serial.printf("SD Menu Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
        // Draw the idle screen
        g->fillScreen(BackColor);
        list.Paint(g, TextColor);
        updateSelection();
    }
    void BeginSD() {
        hasSDCard = SD.begin();
        if (!hasSDCard){
            Serial.println("SD card not found");
            return;
        }
        Serial.println("SD card FOUND!");
        
        File root = SD.open("/");
        list.filesCount = 0;
        while (root && list.filesCount < list.MaxLength)
        {
            File f = root.openNextFile(); 
            if (!f)
                break;
            String fName = String(f.name());
            list.files[list.filesCount] = fName;
            fName.toUpperCase();
            if (fName.endsWith(".GCODE")){
                list.files[list.filesCount] = list.files[list.filesCount].substring(0, list.files[list.filesCount].length() - 6);
                list.filesCount++;
            }
        }      
        if(list.filesCount == 0) {
            list.EmptyString = hasSDCard?"No g-code files":"No SD card found"; 
            list.selected = -1;
        }
        else {
            list.selected = 0;
        }
        // for (int i = 0; i < 15; i++){
        //     list.filesCount++;
        //     list.files[i] = String("File ") + String(rand(), 16) + " " + String(i);
        // }
        updateSelection();
    }
    void IncrementValue() override{
        HandleKeyUp(Keys::KEYPAD_UP);
    }
    void DecrementValue() override{
        HandleKeyUp(Keys::KEYPAD_DOWN);
    }
    void HandleKeyUp(Keys key) override {
        if (key == Keys::KEYPAD_DOWN)
            list.scrollDown();
        else if (key == Keys::KEYPAD_UP)
            list.scrollUp();
    }    
    
    void updateSelection(){
        if(list.getSelected() == lastSelected)
            return;
        lastSelected = list.getSelected();
        if (list.getSelected() < 0) {
            NextStep = 0;
            Serial.printf("Selected index changed: %d\n", list.getSelected());
        }
        else {
            Serial.printf("Selected index changed: %d\n", list.getSelected());
            NextStep = &fileOverViewStep;
            printStep.fileName = String("/") + list.files[list.getSelected()] + ".gcode";
            NeedsRedraw = true;
        }
    }
    private:
    int lastSelected = -1;
};
class UtilitiesMenuStep: public MenuStep {
    public:
    UtilitiesMenuStep() {
            ButtonColor = DarkOceanBlue;
            BackColor = LightOceanBlue;
            TextColor = ST7735_BLACK;
        }
        void Paint(BufferedDisplay* g) override {
            Serial.printf("SD Menu Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
            // Draw the idle screen
            g->fillScreen(BackColor);
            g->setTextColor(TextColor);
            centerString(g, "No SD Card", g->width() / 2, g->height() / 2);
        }
        
};

enum MixType
{
    Linear,
    EaseIn,
    EaseOut,
    EaseInEaseOut,
    Retract
};
enum TransitionDirection{
    Forward = 0,
    Backward,
};

void simplifyMixType(float& p, MixType& mt);
float MixFloats(float f1, float f2, float p, MixType mt);

class MenuHost{
    public:
    MenuStep* CurrentStep = 0;
    MenuStep* TargetStep = 0;
    MenuTransitionStage menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
    TransitionDirection menuTransitionDirection = TransitionDirection::Forward;
    StepAnimationStage stepAnimationStage = StepAnimationStage::MainStep;
    MenuStep* moveToMenuAfterStepAnim = 0;
    
    bool CanGotoNextStep() {
        return TargetStep == 0;
    }
    void GotoStep(MenuStep* step) {
        TargetStep = step;
        step->OnGoingToStep();
        ResetAnimationProgress(500);
        menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
    }
    bool NeedsMenuTransition() {
        return (CurrentStep != TargetStep) && TargetStep != 0;
    }
    
    void ResetAnimationProgress(long duration = 200){
        animationStepProgress = 0.0f;
        animationStartTime = millis();
        animationEndTime = animationStartTime + duration;
    }
    void IncrementAnimationProgress(){
        long currentTime = millis();
        if (currentTime < animationEndTime) {
            animationStepProgress = (float)(currentTime - animationStartTime) / (animationEndTime - animationStartTime);
        } else {
            animationStepProgress = 1.0f;
        }
    }
    long lastPaint = 0;
    void DrawButton(int index, MenuStep* step, BufferedDisplay* g, float progress){
        // Its a triangle at the top left corner in the button color of the step.
        //Serial.printf("Draw button %d @ %f\n", index, progress);
        
        int bSizeW = index == 3 ? 60:100;
        float aspect = 0.8F;
        int bSizeH = bSizeW * aspect;
        int width = MixFloats(6, bSizeW, progress, MixType::EaseInEaseOut); // goes from from 6 to 100 
        int height = width * aspect; // height is fraction of width
        int opBkp = g->GetOpacity();
        int mw = g->width() - 1;
        int mh = g->height() - 1;
        
        // Draw a shadow if the progress is more than 0
        if (progress > 0){
            //g->SetOpacity(10.0F * progress);
            //g->fillRect(0,0, g->width(), g->height(), rgb(0, 0, 0)); // backdrop // this is too much load. Gotto skip this one.
            g->SetOpacity(10);
            for (int si = 0; si <= progress * 20; si += 5)
            //int si = 28;
            { // maximum 5 times            
                // we need a polygon offsetting from the inner edge of the triangle. Gotto use two triangles for that.
                switch (index)
                {
                    case 1:
                    g->fillTriangle(width - 1, 0, width + si, 0, 0, height - 1, rgb(0, 0, 0));
                    g->fillTriangle(0, height + si, width + si, 0, 0, height - 1, rgb(0, 0, 0)); break;
                    case 2:
                    g->fillTriangle(mw - (width - 1), mh, mw - (width + si), mh, mw, mh - (height - 1), rgb(0, 0, 0));
                    g->fillTriangle(mw, mh - (height - 1), mw - (width + si), mh, mw, mh - (height + si), rgb(0, 0, 0)); break;
                    case 3: 
                    g->fillTriangle(mw - (width - 1), 0, mw - (width + si), 0, mw, height - 1, rgb(0, 0, 0));
                    g->fillTriangle(mw, height + si - 1, mw - (width + si), 0, mw, height - 1, rgb(0, 0, 0)); break;
                    default:
                    break;
                }
            }
        }
        g->SetOpacity(100);
        switch (index)
        {
            case 1: g->fillTriangle(0,0, width, 0, 0, height, step->ButtonColor); break;
            case 2: g->fillTriangle(mw - 0, mh - 0, mw - width, mh - 0, mw - 0, mh - height, step->ButtonColor); break;
            case 3: g->fillTriangle(mw - 0, 0, mw - width, 0, mw - 0, height, step->ButtonColor); break;
        
            default:
                break;
        }

        // Draw image
        switch (index)
        {
            case 1: g->fillTriangle(0,0, width, 0, 0, height, step->ButtonColor); break;
            case 2: g->fillTriangle(mw - 0, mh - 0, mw - width, mh - 0, mw - 0, mh - height, step->ButtonColor); break;
            case 3: g->fillTriangle(mw - 0, 0, mw - width, 0, mw - 0, height, step->ButtonColor); break;
        
            default:
                break;
        }
        
        if (index == 3 && progress > 0){
            int x = height * width / (height + width); // anim Bounding box width and height
            (&bmp_Back)->Draw(g, mw - x, x - (&bmp_Back)->height());
        }
        if(step->Icon && progress > 0){
            //int xOffset = MixFloats(width, 0, progress, MixType::EaseInEaseOut);
            //int yOffset = MixFloats(height, 0, progress, MixType::EaseInEaseOut);
            int x = height * width / (height + width); // anim Bounding box width and height
            switch (index)
            {
                case 1: step->Icon->Draw(g, x - step->Icon->width(), x - step->Icon->height()); break;
                case 2: step->Icon->Draw(g, mw - x, mh - x, true); break;
                //case 3: (&bmp_Back)->Draw(g, mw - x, mh - x); break;
            
                default:
                    break;
            }
        }

        g->SetOpacity(opBkp);
    }
    // Top left
    void DrawButton1(MenuStep* step, BufferedDisplay* g, float progress){
        DrawButton(1, step, g, progress);
    }
    // Bottom Right
    void DrawButton2(MenuStep* step, BufferedDisplay* g, float progress){
        // Just like the Button 1, but in the bottom right corner.
        DrawButton(2, step, g, progress);
    }   
    // Top Right
    void DrawButton3(MenuStep* step, BufferedDisplay* g, float progress){        
        // Just like the Button 1, but in the top right corner.
        DrawButton(3, step, g, progress);
    }
    void Paint(BufferedDisplay* bTft){
        if (millis() - lastPaint < 20) { // limit to 50 fps
            return;
        }
        if (menuTrasnsitionStage == MenuTransitionStage::InStep){      
            if (CurrentStep){
                if (stepAnimationStage == StepAnimationStage::MainStep || stepAnimationStage == StepAnimationStage::InOverlay) {
                    if (CurrentStep->NeedsRedraw) { // once there, draw only if needed     
                        CurrentStep->NeedsRedraw = false;                   
                        //Serial.println("In step");
                        CurrentStep->Paint(bTft);
                        if (stepAnimationStage == StepAnimationStage::InOverlay)
                            DrawButton3(CurrentStep, bTft, 1);
                        if (CurrentStep->NextStep){
                            DrawButton2(CurrentStep->NextStep, bTft, stepAnimationStage == StepAnimationStage::MainStep ? 0 : 1);
                        }
                        if (CurrentStep->PreviousStep){
                            DrawButton1(CurrentStep->PreviousStep, bTft, stepAnimationStage == StepAnimationStage::MainStep ? 0 : 1);
                        }
                    }
                }
                else if (stepAnimationStage == StepAnimationStage::GoingToOverLay || stepAnimationStage == StepAnimationStage::GoingToStep) {
                    //Serial.printf("Going to: %s\n", stepAnimationStage == StepAnimationStage::GoingToOverLay ? "overlay" : "step");
                    CurrentStep->Paint(bTft);
                    
                    DrawButton3(CurrentStep, bTft, stepAnimationStage == StepAnimationStage::GoingToOverLay? animationStepProgress:(1 - animationStepProgress));
                    if (CurrentStep->NextStep){
                        DrawButton2(CurrentStep->NextStep, bTft, stepAnimationStage == StepAnimationStage::GoingToOverLay? animationStepProgress:(1 - animationStepProgress));
                    }
                    if (CurrentStep->PreviousStep){
                        DrawButton1(CurrentStep->PreviousStep, bTft, stepAnimationStage == StepAnimationStage::GoingToOverLay? animationStepProgress:(1 - animationStepProgress));
                    }
                    IncrementAnimationProgress();
                    if (animationStepProgress >= 1){
                        if (stepAnimationStage == StepAnimationStage::GoingToOverLay) {
                            stepAnimationStage = StepAnimationStage::InOverlay;
                            CurrentStep->NeedsRedraw = true;     
                        }
                        else if (stepAnimationStage == StepAnimationStage::GoingToStep) {
                            stepAnimationStage = StepAnimationStage::MainStep;
                            if (moveToMenuAfterStepAnim) {// we are back to menu but need to move to the next or previous                                
                                ResetAnimationProgress(250);
                                menuTrasnsitionStage = MenuTransitionStage::TransitionaingScreens;
                                TargetStep = moveToMenuAfterStepAnim; // this should trigger the menu change animation
                                moveToMenuAfterStepAnim = 0;
                            }
                            CurrentStep->NeedsRedraw = true;     
                        }
                    }
                }
                    
            }
        }
        else if (menuTrasnsitionStage == MenuTransitionStage::TransitionaingScreens){
            //Serial.printf("Transitioning screens: %f\n", animationStepProgress);
            // There are no buttons, just need to slide the screens
            
            bTft->xOffset = MixFloats(menuTransitionDirection==TransitionDirection::Forward?128:-128, 0, animationStepProgress, MixType::EaseInEaseOut);                
            if (TargetStep)
                TargetStep->Paint(bTft);
            bTft->xOffset = MixFloats(0, menuTransitionDirection==TransitionDirection::Forward?-128:128, animationStepProgress, MixType::EaseInEaseOut);
            if (CurrentStep)
                CurrentStep->Paint(bTft);
            
            bTft->xOffset = 0;
                
            IncrementAnimationProgress();
            if (animationStepProgress >= 1.0f) {
                ResetAnimationProgress(250);
                menuTrasnsitionStage = MenuTransitionStage::InStep;
                CurrentStep = TargetStep;
                TargetStep = 0;
            }
        }
        
        if (TargetStep != 0) { // we have a transition animation
            
        }
        bTft->update(true, true); 
        
    }
    private:
        float animationStepProgress = 0.0f;
        long animationStartTime = 0;
        long animationEndTime = 0;
};

#endif // M3DUI