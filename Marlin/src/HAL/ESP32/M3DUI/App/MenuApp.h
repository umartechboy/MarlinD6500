#ifndef __MENU_APP__
#define __MENU_APP__

#include "..\Components\M3DUI.h" // For the host
#include "MenuSteps\PrintStep.h"
#include "MenuSteps\PrintPositionStep.h"
#include "MenuSteps\FilePreviewStep.h"
#include "MenuSteps\IdleScreenStep.h"
#include "MenuSteps\SDMenuStep.h"
#include "MenuSteps\OptionsStep.h"
#include "MenuSteps\MaterialsStep.h"
#include "MenuSteps\FilamentChange.h"
#include "MenuSteps\PrinterInfoStep.h"
#include "MenuSteps\BedLevelStep.h"

extern MenuHost menuHost;
extern FilePreviewStep fileOverViewStep;
extern PrintPositionStep printPositionStep;
extern PrintStep printStep;
extern IdleScreenStep idleScreenStep;
extern SDMenuStep sdMenuStep;
extern OptionsStep toolsMenuStep;
extern MaterialsStep materialsMenuStep;
extern FilamentChangeStep filament0ChangeStep;
extern FilamentChangeStep filament1ChangeStep;
extern PrinterInfoStep printerInfoStep;
extern BedLevelStep bedLevelStep;

void BeginApp();
#ifdef __MARLIN_FIRMWARE__
extern void UISetup();
extern void UILoop();
#else
#define SERIAL_IMPL Serial
#endif

#endif