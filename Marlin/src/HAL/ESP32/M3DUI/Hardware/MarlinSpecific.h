#ifndef UI_MARLIN_SPECIFIC
#define UI_MARLIN_SPECIFIC
#include <Arduino.h>
//extern void ABLMeshUpdate(const xy_int8_t &pos);
extern void ABLDone();
extern void ABLFailed();
extern void ABLStarted();
extern int getABLIndex();
extern bool checkABLComplete();
extern bool checkABLFailed();
extern void ABLMeshUpdate();

extern float readTemp1();
extern float readTemp2();
extern void writeTemp(int index, float temp);
extern float readTemp(int index);

extern void enqueueComs(String commands);
extern void enqueueComs(std::initializer_list<String> commands);



#endif
