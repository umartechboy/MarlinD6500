#include "SDMenuStep.h"
#include <SD.h>
#include "..\MenuApp.h"
#include "..\Bitmaps.h"

SDMenuStep::SDMenuStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkPaleYellow;
    BackColor = DarkPaleYellow;
    TextColor = ST7735_BLACK;
    Icon = &bmp_SD;
    TickPeriod = 50;
}
void SDMenuStep::Tick() {
    NeedsRedraw = true;
}
void SDMenuStep::Paint(BufferedDisplay* g) {
    //Serial.printf("SD Menu Step Paint called @ %d, %d\n", g->xOffset, g->yOffset);
    // Draw the idle screen
    g->fillScreen(BackColor);
    list.Paint(g, TextColor);
    updateSelection();
}
void SDMenuStep::BeginSD() {
    hasSDCard = SD.begin();
    if (!hasSDCard){
        SERIAL_IMPL.println("SD card not found");
        return;
    }
    SERIAL_IMPL.println("SD card FOUND!");
    
    File root = SD.open("/");
    list.Clear();
    while (root)
    {
        File f = root.openNextFile(); 
        if (!f)
            break;
        String fName = String(f.name());
        fName.toUpperCase();
        if (fName.endsWith(".GCODE")){
            list.Add(new StringListItem(fName, 6));
        }
    }      
    if(list.Count() == 0) {
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
void SDMenuStep::IncrementValue() {
    HandleKeyUp(Keys::KEYPAD_UP);
}
void SDMenuStep::DecrementValue() {
    HandleKeyUp(Keys::KEYPAD_DOWN);
}
void SDMenuStep::HandleKeyUp(Keys key) {
    if (key == Keys::KEYPAD_DOWN)
        list.scrollDown();
    else if (key == Keys::KEYPAD_UP)
        list.scrollUp();
    else if (key == Keys::KEYPAD_RIGHT)
        Host->GotoNextStep();
    else if (key == Keys::KEYPAD_LEFT)
        Host->GotoPreviousStep();
}    

void SDMenuStep::updateSelection(){
    if(list.getSelectedIndex() == lastSelected)
        return;
    lastSelected = list.getSelectedIndex();
    if (list.getSelectedIndex() < 0) {
        NextStep = 0;
        SERIAL_IMPL.printf("Selected index changed: %d\n", list.getSelectedIndex());
    }
    else {
        SERIAL_IMPL.printf("Selected index changed: %d\n", list.getSelectedIndex());
        NextStep = &fileOverViewStep;
        printStep.fileName = String("/") + ((StringListItem*)list.getSelected())->ItemText;
        NeedsRedraw = true;
    }
}