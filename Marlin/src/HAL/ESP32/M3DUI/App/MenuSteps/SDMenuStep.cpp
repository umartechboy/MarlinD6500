#include "SDMenuStep.h"
#include <SD.h>
#include "..\MenuApp.h"
#include "..\Images.h"

std::vector<String> seen;
String toDOSNameFixed(const String& longName, const std::vector<String>& seenNames);

void selectionUpdated(void* caller, ListItem* selectedItem, int selectedIndex){
    SDMenuStep* This = (SDMenuStep*)caller;
    if (selectedIndex < 0) {
        This->NextStep = 0;
    }
    else {
        This->NextStep = &fileOverViewStep;
        idleScreenStep.fileName = String("/") + ((FileNameListItem*)selectedItem)->ItemText;
        idleScreenStep.DOSFileName = String("/") + ((FileNameListItem*)selectedItem)->DOSName;
        This->NeedsRedraw = true;
    }
}
SDMenuStep::SDMenuStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkPaleYellow;
    BackColor = DarkPaleYellow;
    TextColor = ST7735_BLACK;
    Icon = &img_SD;
    TickPeriod = 50;
    list = new VerticalList(Host, 14, 128, "No SD Card");
    list->SetOnSelectionUpdated(this, selectionUpdated);
}
SDMenuStep::~SDMenuStep(){
    delete list;
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
        list->Paint(g, TextColor);
    } else {
        centerString(g, "No SD Card", Host->appWidth() / 2, Host->appHeight() / 2);
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
        list->scrollDown();
    else if (key == Keys::KEYPAD_UP)
        list->scrollUp();
    else if (key == Keys::KEYPAD_RIGHT)
        Host->GotoNextStep();
    else if (key == Keys::KEYPAD_LEFT)
        Host->GotoPreviousStep();
}    

void SDMenuStep::LoadComplete(){    
  SERIAL_IMPL.println("Starting SD");
    list->EmptyString = "Loading..."; 
    hasSDCard = SD.begin();
    seen.clear();

    if (!hasSDCard){
        SERIAL_IMPL.println("SD card not found");
        list->EmptyString = "No SD card"; 
        return;
    }
    SERIAL_IMPL.println("SD card FOUND!");

    File root = SD.open("/");
    list->Clear();
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
            list->Add(new FileNameListItem(Host, fName, dosName, 6));                
            // }
            // else{                
            //     SERIAL_IMPL.printf("Incompatible G code: %s\n", fName.c_str());
            // }
        }
    }      
    if(list->Count() == 0) {
        list->EmptyString = "No g-code files"; 
        list->selected = -1;
    }
    else {
        list->selected = 0;
    }
    list->InvokeSelectionChanged();
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
