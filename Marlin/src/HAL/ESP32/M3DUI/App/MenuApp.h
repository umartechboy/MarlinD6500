#ifndef __MENU_APP__
#define __MENU_APP__

#include "..\Components\M3DUI.h" // For the host and components
#include "MenuSteps\PrintPositionStep.h"
#include "MenuSteps\FilePreviewStep.h"
#include "MenuSteps\MainScreenStep.h"
#include "MenuSteps\SDMenuStep.h"
#include "MenuSteps\OptionsStep.h"
#include "MenuSteps\MaterialsStep.h"
#include "MenuSteps\FilamentChange.h"
#include "MenuSteps\PrinterInfoStep.h"
#include "MenuSteps\BedLevelStep.h"
#include "MenuSteps\RetroMainMenu.h"
#include "MenuSteps\InPrintMenuStep.h"
#include "MenuSteps\TextEntryStep.h"
#include "MenuSteps\WifiListStep.h"
#include "MenuSteps\SettingsStep.h"
#include "MenuSteps\UpdateStep.h"
#include "MenuSteps\HelpStep.h"
#include "MenuSteps\SensorInfoStep.h"
#include "MenuSteps\MotorMovementStep.h"

extern MenuHost menuHost;
extern FilePreviewStep filePreviewStep;
extern PrintPositionStep printPositionStep;
extern MainScreenStep mainScreenStep;
extern SDMenuStep sdMenuStep;
extern OptionsStep toolsMenuStep;
extern MaterialsStep materialsMenuStep;
extern FilamentChangeStep filament0ChangeStep;
extern FilamentChangeStep filament1ChangeStep;
extern PrinterInfoStep printerInfoStep;
extern BedLevelStep bedLevelStep;
extern RetroMainMenuStep retroMainMenuStep;
extern InPrintMenuStep inPrintMenuStep;
extern TextEntryStep textEntryStep;
extern WifiListStep wifiListStep;
extern SettingsStep settingsStep;
extern UpdateStep updateStep;
extern HelpStep helpStep;
extern SensorInfoStep sensorInfoStep;
extern MotorMovementStep motorMovementStep;

void BeginApp();
#ifdef __MARLIN_FIRMWARE__
extern void UISetup();
extern void UILoop();
#else
#define SERIAL_IMPL Serial
#endif

#endif