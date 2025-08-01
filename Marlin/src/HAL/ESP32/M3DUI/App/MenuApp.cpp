#include "MenuApp.h"

MenuHost menuHost;
FilePreviewStep filePreviewStep(&menuHost);
PrintPositionStep printPositionStep(&menuHost);
MainScreenStep mainScreenStep(&menuHost);
SDMenuStep sdMenuStep(&menuHost);
OptionsStep toolsMenuStep(&menuHost);
MaterialsStep materialsMenuStep(&menuHost);
FilamentChangeStep filament0ChangeStep(&menuHost, 0);
FilamentChangeStep filament1ChangeStep(&menuHost, 1);
PrinterInfoStep printerInfoStep(&menuHost);
BedLevelStep bedLevelStep(&menuHost);
RetroMainMenuStep retroMainMenuStep(&menuHost);
InPrintMenuStep inPrintMenuStep(&menuHost);
TextEntryStep textEntryStep(&menuHost);
WifiListStep wifiListStep(&menuHost);
SettingsStep settingsStep(&menuHost);

void BeginApp(){    
  SERIAL_IMPL.println("Goto menu pushed");
  menuHost.GotoStepFromAny(&mainScreenStep);

  // inverted next/back in menus behind idle because idle is at the center
}