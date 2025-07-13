#include "MenuApp.h"

MenuHost menuHost;
FilePreviewStep fileOverViewStep(&menuHost);
PrintPositionStep printPositionStep(&menuHost);
PrintStep printStep(&menuHost);
IdleScreenStep idleScreenStep(&menuHost);
SDMenuStep sdMenuStep(&menuHost);
OptionsStep toolsMenuStep(&menuHost);
MaterialsStep materialsMenuStep(&menuHost);
FilamentChangeStep filament0ChangeStep(&menuHost, 0);
FilamentChangeStep filament1ChangeStep(&menuHost, 1);
PrinterInfoStep printerInfoStep(&menuHost);
BedLevelStep bedLevelStep(&menuHost);

void BeginApp(){    
  SERIAL_IMPL.println("Starting SD");
  sdMenuStep.BeginSD();
  
  SERIAL_IMPL.println("Goto menu pushed");
  menuHost.GotoStepFromNull(&idleScreenStep);

  idleScreenStep.NextStep = &sdMenuStep;
  idleScreenStep.PreviousStep = &toolsMenuStep;

  // inverted next/back in menus behind idle because idle is at the center
  toolsMenuStep.NextStep = &idleScreenStep; 
  materialsMenuStep.NextStep = &toolsMenuStep;
  printerInfoStep.NextStep = &toolsMenuStep;
  bedLevelStep.NextStep = 0; // Bed level step decides when it is finished

  sdMenuStep.PreviousStep = &idleScreenStep;
  sdMenuStep.NextStep = &fileOverViewStep; 
  fileOverViewStep.PreviousStep = &sdMenuStep;
  fileOverViewStep.NextStep = &printPositionStep;
  printPositionStep.PreviousStep = &fileOverViewStep;
  printPositionStep.NextStep = &printStep;
  printStep.PreviousStep = &printPositionStep;
}