#include "MenuApp.h"

MenuHost menuHost;
FilePreviewStep fileOverViewStep(&menuHost);
PrintPositionStep printPositionStep(&menuHost);
IdleScreenStep idleScreenStep(&menuHost);
SDMenuStep sdMenuStep(&menuHost);
OptionsStep toolsMenuStep(&menuHost);
MaterialsStep materialsMenuStep(&menuHost);
FilamentChangeStep filament0ChangeStep(&menuHost, 0);
FilamentChangeStep filament1ChangeStep(&menuHost, 1);
PrinterInfoStep printerInfoStep(&menuHost);
BedLevelStep bedLevelStep(&menuHost);
RetroMainMenuStep retroMainMenuStep(&menuHost);

void BeginApp(){    
  SERIAL_IMPL.println("Goto menu pushed");
  menuHost.GotoStepFromNull(&idleScreenStep);

  // inverted next/back in menus behind idle because idle is at the center
}