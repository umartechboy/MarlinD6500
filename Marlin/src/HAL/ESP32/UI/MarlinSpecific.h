#ifndef UI_MARLIN_SPECIFIC
#define UI_MARLIN_SPECIFIC

#include "../../../MarlinCore.h"
#include "../../../gcode/queue.h"
#include "../../../module/printcounter.h"

extern void ABLMeshUpdate(const xy_int8_t &pos);
extern void ABLDone();
extern void ABLFailed();
extern void ABLStarted();

#include "../../../module/temperature.h"

extern float readTemp1();
extern float readTemp2();

#endif
