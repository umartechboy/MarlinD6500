
#include "M3DPrintVue.h"
#include "gcode/parser.h"
#include "MarlinCore.h"
#include "module/printcounter.h" // for print jobs
#include "module/motion.h"
#include "module/temperature.h"

#ifdef M3DPrintVueSupport
//#include <ArduinoBLE.h>

//HardwareSerial Serial2(2);
#define comSerial Serial2
#define print_job_timer_status_totalPrints print_job_timer.getStats().totalPrints
#define temp thermalManager.temp_hotend->celsius

bool allowDebug = false;

// M1101 start a new time lapse
bool M1101()
{
  if (allowDebug){
    SERIAL_ECHO("M1101 @ ");
    SERIAL_ECHOLN(print_job_timer_status_totalPrints);
  }
  //if(comSerial.connected())
    comSerial.println(String("begin ") + String(print_job_timer_status_totalPrints));
  return true;
}
// M1102 trigger a time-lapse photo
bool M1102()
{
  if (allowDebug){
    SERIAL_ECHO("M1102 @ ");
    SERIAL_ECHOLN(current_position.z);
  }
  //if(comSerial.connected())
  comSerial.println(String("inc ") + String(current_position.z)); 
  return true;
}
// M1103 End a time lapse
bool M1103()
{
  if (allowDebug){
    SERIAL_ECHOLN("M1103");
  }
  //if(comSerial.connected())
    comSerial.println(String("end "));
  return true;
}
// PrintVue Debug On
bool M1104()
{
  if (parser.seen('D'))
    allowDebug =  parser.value_bool();
  SERIAL_ECHO("PrintVue Debug: ");
  SERIAL_ECHOLN(allowDebug?"ON":"OFF");
  return true;
}


float lastX = -100;
float lastY = -100;
float lastZ = -100;
float lastE = -100;
float lastT = -100;

void M3DPrintVueSetup()
{
  if (allowDebug){
    SERIAL_ECHO("M3DPrintVueSetup()");
  }
  //comSerial.begin("M3D Enabler PV");
  //comSerial.register_callback(Bt_Status);

  comSerial.begin(115200, SERIAL_8N1, -1, 13);
  comSerial.println("M3D Print Vue server connected");
}
void M3DPrintVueLoop()
{  
  if (lastX != current_position.x)
  {
    lastX = current_position.x;
    //if(comSerial.connected())
      comSerial.println(String("x ") + String(current_position.x));
      
    if (allowDebug){
      SERIAL_ECHO("x change = ");
      SERIAL_ECHOLN(lastX);
    }
  }
  if (lastY != current_position.y)
  {
    lastY = current_position.y;
    //if(comSerial.connected())
      comSerial.println(String("y ") + String(current_position.y));
    if (allowDebug){
      SERIAL_ECHO("y change = ");
      SERIAL_ECHOLN(lastY);
    }
  }
  if (lastZ != current_position.z)
  {
    lastZ = current_position.z;
    //if(comSerial.connected())
      comSerial.println(String("z ") + String(current_position.z));
      
    if (allowDebug){
      SERIAL_ECHO("z change = ");
      SERIAL_ECHOLN(lastZ);
    }
  }
  if (lastE != current_position.e)
  {
    lastE = current_position.e;
    //if(comSerial.connected())
      comSerial.println(String("e ") + String(current_position.e));
      
    if (allowDebug){
      SERIAL_ECHO("e change = ");
      SERIAL_ECHOLN(lastE);
    }
  }
  if (lastT != temp)
  {
    lastT = temp;
    //if(comSerial.connected())
      comSerial.println(String("x ") + String(temp));
      
    if (allowDebug){
      //SERIAL_ECHO("temp change = ");
      //SERIAL_ECHOLN(lastT);
    }
  }
}

// String BLEName;

// // create service and characteristics:
// BLEService m3dPrintVueTelemetryService("8e088cd2-8100-11ee-b9d1-0242ac120002");
// BLEFloatCharacteristic positionXCharactristic("8e088cd2-7101-11ee-b9d1-0242ac120002", BLERead);
// BLEFloatCharacteristic positionYCharactristic("8e088cd2-7102-11ee-b9d1-0242ac120002", BLERead);
// BLEFloatCharacteristic positionZCharactristic("8e088cd2-7103-11ee-b9d1-0242ac120002", BLERead | BLENotify);
// BLEFloatCharacteristic positionECharactristic("8e088cd2-7104-11ee-b9d1-0242ac120002", BLERead);
// BLEFloatCharacteristic temperatureCharactristic("8e088cd2-7105-11ee-b9d1-0242ac120002", BLERead);
// BLEIntCharacteristic printingStatusCharactristic("8e088cd2-7106-11ee-b9d1-0242ac120002", BLERead | BLENotify);

// //BLEFloatCharacteristic servoBSpeedCharactristic("8e088cd2-6106-11ee-b9d1-0242ac120002", BLEWrite);
// //BLEStringCharacteristic expressionCharactristic("8e088cd2-6107-11ee-b9d1-0242ac120002", BLEWrite, 128);


// #if ENABLED(M3DPrintVueSupport)

// // M1101 start a new time lapse
// bool M1101(){
//     PV_Debugln("M1101");
//     printingStatusCharactristic.setValue(print_job_timer.getStats().totalPrints);
//     return true;
// }
// // M1102 trigger a time-lapse photo
// bool M1102(){
//     PV_Debugln("M1102");
//     return true;
// }
// // M1103 End a time lapse
// bool M1103(){
//     PV_Debugln("M1103");
//     printingStatusCharactristic.setValue(false);
//     return true;
// }

// void connectHandler(BLEDevice central)
// {
//     // central connected event handler
//     PV_Debug("PrintVue BLE Connected: ");
//     PV_Debugln(central.address());
// }

// void disconnectHandler(BLEDevice central)
// {
//     // central disconnected event handler
//     PV_Debug("PrintVue BLE Disconncted");
//     PV_Debugln(central.address());
// }

// void M3DPrintVueSetup(){
//     BLE.begin();
//     BLEName = "M3D Enabler";
//     BLE.setLocalName(BLEName.c_str());
    
//     BLE.setAdvertisedService(m3dPrintVueTelemetryService);
//     m3dPrintVueTelemetryService.addCharacteristic(positionXCharactristic);
//     m3dPrintVueTelemetryService.addCharacteristic(positionYCharactristic);
//     m3dPrintVueTelemetryService.addCharacteristic(positionZCharactristic);
//     m3dPrintVueTelemetryService.addCharacteristic(positionECharactristic);
//     m3dPrintVueTelemetryService.addCharacteristic(temperatureCharactristic);

//     BLE.setEventHandler(BLEConnected, connectHandler);
//     BLE.setEventHandler(BLEDisconnected, disconnectHandler);
// }
// float lastX = -100;
// float lastY = -100;
// float lastZ = -100;
// float lastE = -100;
// float lastT = -100;

// void M3DPrintVueLoop(){
//     if (lastX!=  current_position.x){
//         lastX = current_position.x;
//         positionXCharactristic.setValue(current_position.x);
//     }
//     if (lastY!=  current_position.y){
//         lastY = current_position.y;
//         positionYCharactristic.setValue(current_position.y);
//     }
//     if (lastZ!=  current_position.z){
//         lastZ = current_position.z;
//         positionZCharactristic.setValue(current_position.z);
//     }
//     if (lastE!=  current_position.e){
//         lastE = current_position.e;
//         positionECharactristic.setValue(current_position.e);
//     }
//     if (lastT!= thermalManager.degHotend(0)){
//         lastT = thermalManager.degHotend(0);
//         temperatureCharactristic.setValue(thermalManager.degHotend(0));
//     }
// }
// #endif

#endif