#include "SDMenuStep.h"
#include <SD.h>
#include "..\MenuApp.h"
#include "..\Bitmaps.h"

std::vector<String> seen;
String toDOSNameFixed(const String& longName, const std::vector<String>& seenNames);

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
    g->setTextColor(TextColor);
    if (hasSDCard){
        list.Paint(g, TextColor);
        updateSelection();
    } else {
        centerString(g, "No SD Card", g->width() / 2, g->height() / 2);
    }
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
    if (list.getSelectedIndex() < 0) {
        NextStep = 0;
    }
    if(list.getSelectedIndex() == lastSelected){
        return;
    }
    lastSelected = list.getSelectedIndex();
    if (list.getSelectedIndex() < 0) {
        SERIAL_IMPL.printf("Selected index changed: %d\n", list.getSelectedIndex());
    }
    else {
        SERIAL_IMPL.printf("Selected index changed: %d\n", list.getSelectedIndex());
        NextStep = &fileOverViewStep;
        idleScreenStep.fileName = String("/") + ((FileNameListItem*)list.getSelected())->ItemText;
        idleScreenStep.DOSFileName = String("/") + ((FileNameListItem*)list.getSelected())->DOSName;
        NeedsRedraw = true;
    }
}
void SDMenuStep::LoadComplete(){    
  SERIAL_IMPL.println("Starting SD");
    list.EmptyString = "Loading..."; 
    hasSDCard = SD.begin();
    seen.clear();

    if (!hasSDCard){
        SERIAL_IMPL.println("SD card not found");
        list.EmptyString = "No SD card"; 
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
            
            // SERIAL_IMPL.printf("G code: %s\n", fName.c_str());
            // See if the first 150 lines contain the printer stamp
            // bool isCompatible = false;
            // for (int i; i < 200; i++){
            //     if (!f.available())
            //         break;
            //     String line =  f.readStringUntil('\n');
            //     if (line.indexOf("M3D_D8500_GCode") >= 0){
            //         isCompatible = true;
            //         break;
            //     }
            // }
            // f.close();
            // if (isCompatible){
            String dosName = toDOSNameFixed(fName, seen);
            SERIAL_IMPL.printf("Compatible G code: {%s}, {%s}\n", fName.c_str(), dosName.c_str());
            list.Add(new FileNameListItem(fName, dosName, 6));                
            // }
            // else{                
            //     SERIAL_IMPL.printf("Incompatible G code: %s\n", fName.c_str());
            // }
        }
    }      
    if(list.Count() == 0) {
        list.EmptyString = "No g-code files"; 
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

String toDOSNameFixed(const String& longName, const std::vector<String>& seenNames) {
  // Strip path
  int lastSlash = longName.lastIndexOf('/');
  String name = (lastSlash >= 0) ? longName.substring(lastSlash + 1) : longName;

  // Uppercase it
  name.toUpperCase();

  // Split extension
  int dotIndex = name.lastIndexOf('.');
  String base = (dotIndex >= 0) ? name.substring(0, dotIndex) : name;
  String ext = (dotIndex >= 0) ? name.substring(dotIndex + 1) : "";

  // Remove spaces and invalid chars from base
  String cleanBase = "";
  for (char c : base) {
    if (isalnum(c) || c == '_') {
      cleanBase += c;
    }
  }

  // Truncate to 6 characters for base
  if (cleanBase.length() > 6)
    cleanBase = cleanBase.substring(0, 6);

  // Count duplicates
  int suffix = 1;
  String testName;
  do {
    testName = cleanBase + "~" + String(suffix);
    suffix++;
  } while (std::find(seenNames.begin(), seenNames.end(), testName) != seenNames.end());

  // Truncate extension to 3
  if (ext.length() > 3)
    ext = ext.substring(0, 3);

  // Compose final name
  return ext.length() ? testName + "." + ext : testName;
}
