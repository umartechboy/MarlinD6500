#include "MarlinSpecific.h"
#include "..\..\..\..\module\temperature.h"
#include "..\..\..\..\gcode\queue.h"
#include "..\..\..\..\gcode\gcode.h"
#include "..\..\..\..\sd\cardreader.h"

static int target [2] = {30, 30};

bool hasComsQueued() {
    return queue.has_commands_queued();
}
void enqueueComs(String commands) {
    queue.enqueue_one(commands.c_str());
}
void enqueueComs(std::initializer_list<String> commands) {
    for (const auto& com : commands) {
        queue.enqueue_one(com.c_str());
    }
}
void writeTemp(int index, float temp){
    String com = String("M104 S") + String(temp, 0) + String(" T") + String(index);
    queue.enqueue_one(com.c_str());
    //target[index] = temp;
}
float readTemp1(){
    return thermalManager.degHotend(0);
    // float t = 24.0 + (float)millis() / 1000.0F * 2.5F; // 2.5 degree per second
    // if (t > target[0]){
    //     t = target[0] + (float)(millis() % 30) / 100.0F;
    // }
    // return t;
}
float readTemp2(){
    return thermalManager.degHotend(1);
    // float t = 20.0 + (float)millis() / 1000.0F * 2.5F; // 2.4 degree per second
    // if (t > target[1]){
    //     t = target[1] + (float)(millis() % 30) / 100.0F;
    // }
    // return t;
}
float readTemp(int index){
    if (index == 0)
        return readTemp1();
    return
        readTemp2();
}
static bool ABLStartedFlag = false;
static bool ABLCompleteFlag = false;
static bool ABLFailedFlag = false;
void ABLDone(){
    SERIAL_IMPL.println("ABLDone()");
    ABLCompleteFlag = true;
    ABLFailedFlag = false;
    ABLStartedFlag = false;
    enqueueComs("M500");
}
void ABLFailed(){
    SERIAL_IMPL.println("ABLFailed()");
    ABLCompleteFlag = true;
    ABLFailedFlag = true;
    ABLStartedFlag = false;
}
void ABLStarted(){
    SERIAL_IMPL.println("ABLStarted()");
    // We don't use this
}
static bool canDoABL = true;
static bool pauseABFlag = false;
void StartABL(){
    SERIAL_IMPL.println("StartABL()");
    canDoABL = true;
    ABLStartedFlag = true;
    ABLCompleteFlag = false;
    ABLFailedFlag = false;
    queue.enqueue_one("G29");
    pauseABFlag = false;
}
bool CanContinueDoingG29(){
    return canDoABL;
}
// Puts the flag to abort, actual aborting may take some time.
void AbortABL(){
    SERIAL_IMPL.println("AbortABL()");
    resumeABL(); // in case its paused, it needs to come out of it first
    canDoABL = false;
}
bool checkABLComplete(){
    return ABLCompleteFlag;
}
bool checkABLFailed(){
    return ABLFailedFlag;
}
bool checkABLPaused(){
    return pauseABFlag;
}
void pauseABL(){
    SERIAL_IMPL.println("pauseABL()");
    pauseABFlag = true;
}
void resumeABL(){
    SERIAL_IMPL.println("resumeABL()");
    pauseABFlag = false;
}
static int pointsDone = 0;
int getABLIndex(){
    return pointsDone;
}
void ABLMeshUpdate(int _pointsDone){
    SERIAL_IMPL.printf("ABLMeshUpdate(%d)\n", _pointsDone);
    pointsDone = _pointsDone;
}

bool nozzleCleaningSent = false;
bool filamentPositioningSent = false;
bool printComsSent = false;

void printJobTick(){

}

extern float lastRecoverySavedAt;
void prepareForPrint(String dosFileName, bool hasExtruder1, bool hasExtruder2, void (*printStartedCallback)(void*), void* sender){
    nozzleCleaningSent = false;
    filamentPositioningSent = false;
    printComsSent = false;
    lastRecoverySavedAt = 0.1;
    
    // FOr now, return right away.
    enqueueComs({"M413 S1"});
    String m23 = String("M23 ") + dosFileName;
    enqueueComs({"M21", m23, "M24"}); // Init SD, Select File, Put to print
    (*printStartedCallback)(sender);
}
void pausePrint(){
    SERIAL_IMPL.println("pausePrint()");
    card.pauseSDPrint();    

    // gcode.process_subcommands_now(F("G91"));
    // gcode.process_subcommands_now(F("G1 Z5 E-5 F1000"));
    // gcode.process_subcommands_now(F("G90"));
    // gcode.process_subcommands_now(F("G1 X0 Y150 F1000"));
}
void resumePrint(){
    SERIAL_IMPL.println("resumePrint()");
    // gcode.process_subcommands_now(F("G91"));
    // gcode.process_subcommands_now(F("G1 Z-5 E4 F1000"));
    // gcode.process_subcommands_now(F("G90"));
    card.startOrResumeFilePrinting();
}
void abortPrint(){    
    SERIAL_IMPL.println("abortPrint()");
    pausePrint();
    card.abortFilePrintNow();
    enqueueComs({"G1 Y200 X100 F2000"});    
}