#include "SDMenuStep.h"
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
        mainScreenStep.fileName = ((FileNameListItem*)selectedItem)->ItemText;
        mainScreenStep.DOSFileName = ((FileNameListItem*)selectedItem)->DOSName;
        filePreviewStep.Title = mainScreenStep.fileName;
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
    JumpToStepOnNoActivity = &mainScreenStep;
    NoActivityTimeout = 15000; // 15 seconds
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
    RegisterActivity();
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
void OnFileFound(void* caller, const char* dosName) {
    SDMenuStep* This = (SDMenuStep*)caller;

    // Check if the filename ends with ".GCO" (case-insensitive)
    const char *ext = strrchr(dosName, '.');  // find last dot
    if (ext && strcasecmp(ext, ".GCO") == 0) {
        SERIAL_IMPL.printf("Adding File: %s\n", dosName);
        This->list->AddAtBeginning(new FileNameListItem(This->Host, "", dosName, 0, 14));
    }
    else {
        SERIAL_IMPL.printf("Skipping non-GCO file: %s\n", dosName);
    }
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
        const char* dosChar = ((FileNameListItem*)list->At(i))->DOSName.c_str();
        char lp[128];  // choose a capacity that fits your deepest expected path
        if (card.getLongPath(dosChar, lp, sizeof(lp))) {
            String longName(lp);  // if you still want a String
            longName = longName.substring(1, longName.length() - 6); // Trim / and .gcode
            SERIAL_IMPL.printf("%s > %s\n", dosChar, longName.c_str());
            ((FileNameListItem*)list->At(i))->ItemText = longName;
        } else {
            SERIAL_IMPL.println("getLongPath failed or overflow.");
        }
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
