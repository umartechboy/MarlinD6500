#include "MarlinSpecific.h"

float readTemp1(){
    return thermalManager.degHotend(0);
}
float readTemp2(){
    return thermalManager.degHotend(1);
}