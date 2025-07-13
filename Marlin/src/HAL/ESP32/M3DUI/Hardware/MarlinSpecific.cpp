#include "MarlinSpecific.h"
#include "..\..\..\..\module\temperature.h"
#include "..\..\..\..\gcode\queue.h"

static int target [2] = {30, 30};

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
    queue.enqueue_one_now(com.c_str());
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
bool ABLCompleteFlag = false;
bool ABLFailedFlag = false;
void ABLDone(){
    ABLCompleteFlag = true;
    ABLFailedFlag = false;
}
void ABLFailed(){
    ABLCompleteFlag = true;
    ABLFailedFlag = true;
}
void ABLStarted(){
    ABLCompleteFlag = false;
    ABLFailedFlag = false;
}
bool checkABLComplete(){
    return ABLCompleteFlag;
}
bool checkABLFailed(){
    return ABLFailedFlag;
}
int getABLIndex(){
    return (millis() / 10000);  
}
void ABLMeshUpdate(){

}