#include "Update2Step.h"
#include "..\MenuApp.h"
#include "..\Images.h"
#include "..\..\..\..\..\sd\cardreader.h"

// std::vector<String> seen;
// String toDOSNameFixed(const String& longName, const std::vector<String>& seenNames);

void OnFileFound3(void* caller, const char* dosName) {
    Update2MenuStep* This = (Update2MenuStep*)caller;

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
static void selectionUpdated(void* caller, ListItem* selectedItem, int selectedIndex){
    Update2MenuStep* This = (Update2MenuStep*)caller;
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
Update2MenuStep::Update2MenuStep(MenuHost* host):MenuStep(host) {
    ButtonColor = DarkRed;
    BackColor = DarkRed;
    TextColor = ST7735_WHITE;            
    Title = "Firmware Update";
    RetroIcon = &img_RetroM3D;  
    PreviousStep = 0;
    RetroNextStep = 0;

    Icon = &img_Home;
    TickPeriod = 5;

    list = new VerticalList(Host, "No SD Card");
    list->SetOnSelectionUpdated(this, selectionUpdated);
}
Update2MenuStep::~Update2MenuStep(){
    delete list;
}

void Update2MenuStep::Tick() {
    NeedsRedraw = true;
    if (UpdateState == SdUpdateState::MountingSd){
        UpdateState = SdUpdateState::None;
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
        if (card.fileExists(UpdateFileName)){
            SERIAL_IMPL.println("Firmware file found!");
            card.openFileRead(UpdateFileName);
            if (card.isFileOpen()){
                UpdateState = SdUpdateState::UpdatingFromSd;
                SERIAL_IMPL.println("File open for update.");
                updateBuffer = new uint8_t[SdUpdateBufferSize];
            }
            else{
                SERIAL_IMPL.println("File could not be opened.");
            }
        }
        else{
            SERIAL_IMPL.println("No firmware file found");
        }
    } else if (UpdateState == SdUpdateState::UpdatingFromSd){
        int tRead = card.read(updateBuffer, SdUpdateBufferSize);
        SERIAL_IMPL.printf("Read %d/%d\n", tRead, SdUpdateBufferSize);
        if (tRead > 0) {
            // write update data
        }
        if (tRead == 0){
            delete updateBuffer;
            SERIAL_IMPL.println("EOF for Update file");
            card.closefile();
            UpdateState = SdUpdateState::RestartingAfterSd;
            updateFinishedAt = millis();
            TickPeriod = 1000;
        }
    }
    else if (UpdateState == SdUpdateState::RestartingAfterSd){
        if (millis() - updateFinishedAt < 5000){
            SERIAL_IMPL.printf("Restarting in %d\n", (5000 - (millis() - updateFinishedAt)) / 1000);
        }
        else{
            SERIAL_IMPL.println("Restarting");
            UpdateState = SdUpdateState::None;            
        }
    }
}
void Update2MenuStep::Paint(BufferedDisplay* g) {
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
void Update2MenuStep::IncrementValue() {
    HandleKeyPress(Keys::KEYPAD_UP);
}
void Update2MenuStep::DecrementValue() {
    HandleKeyPress(Keys::KEYPAD_DOWN);
}
void Update2MenuStep::HandleKeyPress(Keys key) {
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
void Update2MenuStep::LoadComplete(){   
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
