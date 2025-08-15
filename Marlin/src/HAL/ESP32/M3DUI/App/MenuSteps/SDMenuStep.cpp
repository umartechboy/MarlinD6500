#include "SDMenuStep.h"
//#include <SD.h>
#include "..\MenuApp.h"
#include "..\Images.h"
#include "..\..\..\..\..\sd\cardreader.h"

// std::vector<String> seen;
// String toDOSNameFixed(const String& longName, const std::vector<String>& seenNames);

static void selectionUpdated(void* caller, ListItem* selectedItem, int selectedIndex){
    SDMenuStep* This = (SDMenuStep*)caller;
    if (selectedIndex < 0) {
        This->NextStep = 0;
    }
    else {
        This->NextStep = &filePreviewStep;
        This->RetroNextStep = &filePreviewStep; // same in retro
        mainScreenStep.fileName = String("/") + ((FileNameListItem*)selectedItem)->ItemText;
        mainScreenStep.DOSFileName = String("/") + ((FileNameListItem*)selectedItem)->DOSName;
        filePreviewStep.Title = mainScreenStep.fileName.substring(1, mainScreenStep.fileName.length() - 6);
        This->NeedsRedraw = true;
    }
}
SDMenuStep::SDMenuStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkPaleYellow;
    BackColor = DarkPaleYellow;
    TextColor = ST7735_BLACK;
    Icon = &img_SD;
    PreviousStep = &mainScreenStep;
    RetroPreviousStep = &retroMainMenuStep;
    NextStep = &filePreviewStep; 
    Title = "SD Card Contents";
    NextActionString = "Select";
    TickPeriod = 50;
    list = new VerticalList(Host, "No SD Card");
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
        list->Paint(g, 0, 0, g->width(), g->height(), TextColor);
    } else {
        centerString(g, "No SD Card", Host->appWidth() / 2, Host->appHeight() / 2);
    }
}
void SDMenuStep::IncrementValue() {
    HandleKeyPress(Keys::KEYPAD_UP);
}
void SDMenuStep::DecrementValue() {
    HandleKeyPress(Keys::KEYPAD_DOWN);
}
void SDMenuStep::HandleKeyPress(Keys key) {
    if (key == Keys::KEYPAD_DOWN)
        list->scrollDown();
    else if (key == Keys::KEYPAD_UP)
        list->scrollUp();
    if (!Host->Retro){
        if (key == Keys::KEYPAD_RIGHT)
            Host->GotoNextStep();
        else if (key == Keys::KEYPAD_LEFT)
            Host->GotoPreviousStep();
    }
}    
// void GetSDFilesList(void* caller, void (*callBack)(void* caller, String& dosName, String& fName)){    
//     seen.clear();
//     File root = SD.open("/");
//     if(callBack){
//         String dummyFileName = "Dummy File Name.gcode";
//         String dummyDosName = "DUMMYF~1.GCO";
//         callBack(caller, dummyFileName, dummyDosName);
//     }
//     while (root)
//     {
//         File f = root.openNextFile(); 
//         if (!f)
//             break;
//         String fName = String(f.name());
//         fName.toUpperCase();
//         if (fName.endsWith(".GCODE")){
            
//             // SERIAL_IMPL.printf("G code: %s\n", fName.c_str());
//             // See if the first 150 lines contain the printer stamp
//             // bool isCompatible = false;
//             // for (int i; i < 200; i++){
//             //     if (!f.available())
//             //         break;
//             //     String line =  f.readStringUntil('\n');
//             //     if (line.indexOf("M3D_D8500_GCode") >= 0){
//             //         isCompatible = true;
//             //         break;
//             //     }
//             // }
//             // f.close();
//             // if (isCompatible){
//             String dosName = toDOSNameFixed(fName, seen);
//             SERIAL_IMPL.printf("Compatible G code: {%s}, {%s}\n", fName.c_str(), dosName.c_str());
//             if (callBack)
//                 callBack(caller, dosName, fName);
//             // }
//             // else{                
//             //     SERIAL_IMPL.printf("Incompatible G code: %s\n", fName.c_str());
//             // }
//         }
//     }      
// }

void OnFileFound(void* caller, const char* dosName){
    SDMenuStep* This = (SDMenuStep*)caller;
    //String fName = card.getLongPath(dosName);
    SERIAL_IMPL.printf("Adding File: %s\n", dosName);
    This->list->Add(new FileNameListItem(This->Host, "", dosName, 6, 14));
}
void SDMenuStep::LoadComplete(){   
  SERIAL_IMPL.println("Starting SD");
    list->EmptyString = "Loading..."; 
    if (!card.isMounted()){
        card.mount();
    }
    hasSDCard = card.isMounted();

    if (!hasSDCard){
        SERIAL_IMPL.println("SD card not found");
        list->EmptyString = "No SD card"; 
        return;
    }

    SERIAL_IMPL.println("SD card FOUND!");
    list->Clear();
    card.ls(this, OnFileFound);
    SERIAL_IMPL.println("All Done. Finding long names:");
    for(int i = 0; i < list->Count(); i++){
        SERIAL_IMPL.printf("Existing Long: %s\n", ((FileNameListItem*)list->At(i))->ItemText.c_str());
        delay(1);
        SERIAL_IMPL.printf("Existing DOS: %s\n", ((FileNameListItem*)list->At(i))->DOSName.c_str());
        delay(1);
        const char* dosChar = ((FileNameListItem*)list->At(i))->DOSName.c_str();
        SERIAL_IMPL.println("Made Dos char*");
        delay(1);
        SERIAL_IMPL.printf("Existing DOS char *: %s\n", dosChar);
        delay(1);
        SERIAL_IMPL.printf("Existing DOS char * len: %d\n", strlen(dosChar));
        delay(1);
        char lp[128];  // choose a capacity that fits your deepest expected path
        if (card.getLongPath(dosChar, lp, sizeof(lp))) {
            String longName(lp);  // if you still want a String
            SERIAL_IMPL.printf("Long: %s\n", longName.c_str());
            ((FileNameListItem*)list->At(i))->ItemText = longName;
        } else {
            SERIAL_IMPL.println("getLongPath failed or overflow.");
        }
        
        SERIAL_IMPL.println(((FileNameListItem*)list->At(i))->ItemText.c_str());
    }

    //GetSDFilesList(this, OnFileFound);

    if(list->Count() == 0) {
        list->EmptyString = "No g-code files"; 
        list->selected = -1;
    }
    else {
        list->selected = 0;
    }
    list->InvokeSelectionChanged();
}

// String toDOSNameFixed(const String& longName, const std::vector<String>& seenNames) {
//   // Strip path
//   int lastSlash = longName.lastIndexOf('/');
//   String name = (lastSlash >= 0) ? longName.substring(lastSlash + 1) : longName;

//   // Uppercase it
//   name.toUpperCase();

//   // Split extension
//   int dotIndex = name.lastIndexOf('.');
//   String base = (dotIndex >= 0) ? name.substring(0, dotIndex) : name;
//   String ext = (dotIndex >= 0) ? name.substring(dotIndex + 1) : "";

//   // Remove spaces and invalid chars from base
//   String cleanBase = "";
//   for (char c : base) {
//     if (isalnum(c) || c == '_') {
//       cleanBase += c;
//     }
//   }

//   // Truncate to 6 characters for base
//   if (cleanBase.length() > 6)
//     cleanBase = cleanBase.substring(0, 6);

//   // Count duplicates
//   int suffix = 1;
//   String testName;
//   do {
//     testName = cleanBase + "~" + String(suffix);
//     suffix++;
//   } while (std::find(seenNames.begin(), seenNames.end(), testName) != seenNames.end());

//   // Truncate extension to 3
//   if (ext.length() > 3)
//     ext = ext.substring(0, 3);

//   // Compose final name
//   return ext.length() ? testName + "." + ext : testName;
// }
