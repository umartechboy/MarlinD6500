/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#ifdef ARDUINO_ARCH_ESP32

#include "../../inc/MarlinConfig.h"
#include "Music.h"
#include <rom/rtc.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <HardwareSerial.h>
#include <soc/adc_channel.h>
#include <ESP32_SoftWire.h>
#include "SoftWireLibs/PCF8574/PCF8574.h"
#include "LoadCell/LoadCell.h"
#include "M3DUI/App/MenuApp.h"
#include "..\..\feature\powerloss.h"
//#include "SoftWireLibs/ADS1x15/Adafruit_ADS1X15.h"
#include <map>
#include "LoadCell/LoadCell.h"
#include <Preferences.h>
extern bool hasJoyStick;
extern float PerMachineProbePressureCompensation;
extern float FloatingFactor;
std::map<int, uint16_t> adcMap;
#if USE_ESP32_PCF8574
SoftWire sWire;
PCF8574 pcf1(0x20, &sWire);
PCF8574 pcf2(0x21, &sWire); // Closer to ESP32 (U6)
SemaphoreHandle_t xPCFIOMutex;
TaskHandle_t pcfTaskHandle = NULL;
volatile uint16_t pcfReadCache = 0xFFFF;
volatile bool pcf1_write_pending = false;
volatile bool pcf2_write_pending = false;
volatile bool pcf1_read_pending = false;
volatile bool pcf2_read_pending = false;
volatile uint16_t pcfMapToSync = 0xFFFF;
volatile uint16_t pcfWriteMapConfirmed = 0xFFFF;

#endif
//Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
#if ENABLED(USE_ESP32_TASK_WDT)
  #include <esp_task_wdt.h>
#endif

#if ENABLED(WIFISUPPORT)
  #include <ESPAsyncWebServer.h>
  #include "wifi.h"
  #if ENABLED(OTASUPPORT)
    #include "ota.h"
  #endif
  #if ENABLED(WEBSUPPORT)
    #include "spiffs.h"
    #include "web.h"
  #endif
#endif

#if ENABLED(ESP3D_WIFISUPPORT)
  DefaultSerial1 MSerial0(false, Serial2Socket);
#endif

// ------------------------
// Externs
// ------------------------

portMUX_TYPE MarlinHAL::spinlock = portMUX_INITIALIZER_UNLOCKED;

// ------------------------
// Local defines
// ------------------------

#define V_REF 1100

// ------------------------
// Public Variables
// ------------------------

uint16_t MarlinHAL::adc_result;
pwm_pin_t MarlinHAL::pwm_pin_data[MAX_EXPANDER_BITS];

// ------------------------
// Private Variables
// ------------------------

esp_adc_cal_characteristics_t characteristics[ADC_ATTEN_MAX];
adc_atten_t attenuations[ADC1_CHANNEL_MAX] = {};
uint32_t thresholds[ADC_ATTEN_MAX];

volatile int numPWMUsed = 0;
volatile struct { pin_t pin; int value; } pwmState[MAX_PWM_PINS];

pin_t chan_pin[CHANNEL_MAX_NUM + 1] = { 0 }; // PWM capable IOpins - not 0 or >33 on ESP32

struct {
  uint32_t freq; // ledcReadFreq doesn't work if a duty hasn't been set yet!
  uint16_t res;
} pwmInfo[(CHANNEL_MAX_NUM + 1) / 2];

// ------------------------
// Public functions
// ------------------------

#if ENABLED(WIFI_CUSTOM_COMMAND)

  bool wifi_custom_command(char * const command_ptr) {
    #if ENABLED(ESP3D_WIFISUPPORT)
      return esp3dlib.parse(command_ptr);
    #else
      UNUSED(command_ptr);
      return false;
    #endif
  }

#endif

#if ENABLED(USE_ESP32_EXIO)

  HardwareSerial YSerial2(2);

  void Write_EXIO(uint8_t IO, uint8_t v) {
    if (hal.isr_state()) {
      hal.isr_off();
      YSerial2.write(0x80 | (((char)v) << 5) | (IO - 100));
      hal.isr_on();
    }
    else
      YSerial2.write(0x80 | (((char)v) << 5) | (IO - 100));
  }

#endif

#if ENABLED(M3DPrintVueSupport)
  extern void M3DPrintVueSetup();
  extern void M3DPrintVueLoop();
#endif
#if USE_ESP32_PCF8574
bool pcfIsSyncing = false;
bool PCFIsBusy = false;
uint16_t lockedBits = 0;
bool PCFSync(bool force = false);
bool LockPCF(uint16_t lockedPins){
  if (pcfIsSyncing)
  // Can't get a lock because the PCF are already busy.
    return false;
    
  lockedBits = lockedPins;
  PCFIsBusy = true;
  return true;
}
void ReleasePCF(){
  PCFIsBusy = false;
  lockedBits = 0;
  PCFSync();
}
bool PCFSync(bool force){
  if (PCFIsBusy && !force)
    return false;
  if (pcfIsSyncing)
    return false;
  pcfIsSyncing = true;
  if (pcf1_write_pending) {
    pcf1.write8(pcfMapToSync & 0xFF);
    pcf1_write_pending = false;
  }

  if (pcf2_write_pending) {
    pcf2.write8((pcfMapToSync >> 8) & 0xFF);
    pcf2_write_pending = false;
  }

  pcfWriteMapConfirmed = pcfMapToSync;

  if (pcf1_read_pending) {
    pcfReadCache &= 0xFF00;
    pcfReadCache |= pcf1.read8();
    pcf1_read_pending = false;
  }

  if (pcf2_read_pending) {
    pcfReadCache &= 0x00FF;
    pcfReadCache |= pcf2.read8() << 8;
    pcf2_read_pending = false;
  }
  pcfIsSyncing = false;
  return true;
}
void pcfServiceTask(void *param) {
  for (;;) {
    // Block until notified
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // clears notification automatically
    if (xSemaphoreTake(xPCFIOMutex, portMAX_DELAY)) {
      PCFSync();
      xSemaphoreGive(xPCFIOMutex);
    }
  }
}
#else
bool LockPCF(uint16_t lockedPins){ return true; } // lock always succeeds (no PCF)
void ReleasePCF(){}
bool PCFSync(bool force){ return true; } // always succeeds (no PCF)
#endif

void InitIOExpanders(){
  // disabled all Steppers
  pinMode(X_ENABLE_PIN, OUTPUT);
  pinMode(Y_ENABLE_PIN, OUTPUT);
  pinMode(Z_ENABLE_PIN, OUTPUT);
  pinMode(E0_ENABLE_PIN, OUTPUT);
  pinMode(E1_ENABLE_PIN, OUTPUT);
  pinMode(HEATER_0_PIN, OUTPUT);
  pinMode(HEATER_1_PIN, OUTPUT);
  pinMode(FAN0_PIN, OUTPUT);
  digitalWrite(X_ENABLE_PIN, HIGH);
  digitalWrite(Y_ENABLE_PIN, HIGH);
  digitalWrite(Z_ENABLE_PIN, HIGH);
  digitalWrite(E0_ENABLE_PIN, HIGH);
  digitalWrite(E1_ENABLE_PIN, HIGH);
  digitalWrite(HEATER_0_PIN, LOW);
  digitalWrite(HEATER_1_PIN, LOW);
  digitalWrite(FAN0_PIN, LOW);
#if USE_ESP32_PCF8574
  xPCFIOMutex = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(pcfServiceTask, "PCFService", 2048, NULL, 1, &pcfTaskHandle, 1);
  //SERIAL_IMPL.println("Starting Wire and IO Expander");
  sWire.begin(21, 22, 400000);
  if (pcf1.begin()){
    //SERIAL_IMPL.println("PCF1 Started");
  }
  else  
    //SERIAL_IMPL.println("PCF1 Failed");
    
  if (pcf2.begin()){
    //SERIAL_IMPL.println("PCF2 Started");
  }
  else  
    //SERIAL_IMPL.println("PCF2 Failed");
    
  // if (ads.begin(0x48, &sWire)){
  //   SERIAL_IMPL.println("ADS1115 Started");
  // }
  // else  
  //   SERIAL_IMPL.println("ADS1115 Failed");

  //SERIAL_IMPL.println("Expanders On.");
  delay(20);
  #define pcfBitOne(pin) (1 << (pin - 200))
  pcfMapToSync = 0 | 
    pcfBitOne(X_ENABLE_PIN) |
    pcfBitOne(Y_ENABLE_PIN) |
    pcfBitOne(E0_ENABLE_PIN) |
    pcfBitOne(E1_ENABLE_PIN) |
    pcfBitOne(X_STOP_PIN) |
    pcfBitOne(Z_STOP_PIN) |
    pcfBitOne(HxData);
  pcf1.write8(pcfMapToSync); // 200-207
  pcf2.write8(pcfMapToSync >> 8); // 208-215, 1 for X and Z stops
#endif
}

void MarlinHAL::init_board() {
  UISetup();
  LoadCellSetup();
  InitMusic(Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN);

  Preferences prefs;
  prefs.begin("machine");
  hasJoyStick = prefs.getBool("joystick", true);
  PerMachineProbePressureCompensation = prefs.getFloat("probe_p", 0);
  FloatingFactor = prefs.getFloat("probe_f", 0.002F);
  prefs.end();
  SERIAL_IMPL.print("Control set to: ");
  SERIAL_IMPL.println(hasJoyStick?"Joystick":"Touchpad");
  SERIAL_IMPL.println("Starting SD to look for PLR");
  if(!card.isMounted()){
    card.mount();
  }

  if (recovery.exists()){
    SERIAL_IMPL.println("Loading recovery");
    recovery.load();
    if (recovery.valid()){
      SERIAL_IMPL.println("Valid Recovery loaded");
      mainScreenStep.printStatus = PrintStatus::PrintToRecover;      
      SERIAL_IMPL.printf("File Name: %s\n", recovery.info.sd_filename);
      SERIAL_IMPL.printf("SD Pos: %d\n", recovery.info.sdpos);
      SERIAL_IMPL.printf("X offset: %f\n", recovery.info.home_offset.x);
      SERIAL_IMPL.printf("Y offset: %f\n", recovery.info.home_offset.y);
      SERIAL_IMPL.printf("Z offset: %f\n", recovery.info.home_offset.z);
      SERIAL_IMPL.printf("X shift: %f\n", recovery.info.position_shift.x);
      SERIAL_IMPL.printf("Y shift: %f\n", recovery.info.position_shift.y);
      SERIAL_IMPL.printf("Z shift: %f\n", recovery.info.position_shift.z);
      SERIAL_IMPL.printf("Active Extrder: %d\n", recovery.info.active_extruder);
      SERIAL_IMPL.printf("Elapsed: %d\n", recovery.info.print_job_elapsed);
    }
    else {
      SERIAL_IMPL.println("Recovery not valid");
    }
  }
  else {
    SERIAL_IMPL.println("No Recovery Data");
  }
  
  #if ENABLED(USE_ESP32_TASK_WDT)
    esp_task_wdt_init(10, true);
  #endif
  #if ENABLED(M3DPrintVueSupport)
    M3DPrintVueSetup();
  #endif
  #if ENABLED(ESP3D_WIFISUPPORT)
    //esp3dlib.init();
  #elif ENABLED(WIFISUPPORT)
    wifi_init();
    TERN_(OTASUPPORT, OTA_init());
    #if ENABLED(WEBSUPPORT)
      spiffs_init();
      web_init();
    #endif
    server.begin();
  #endif

  #if CONFIG_IDF_TARGET_ESP32
  #else
  // ESP32 uses a GPIO matrix that allows pins to be assigned to hardware serial ports.
  // The following code initializes hardware Serial1 and Serial2 to use user-defined pins
  // if they have been defined.
  // On S3, This interferes with ADC read on channel3. Don't begin seria, there is no need.
  #if defined(HARDWARE_SERIAL1_RX) && defined(HARDWARE_SERIAL1_TX) && (HARDWARE_SERIAL1_RX != -1) && (HARDWARE_SERIAL1_TX != -1)
    HardwareSerial Serial1(1);
    #ifdef TMC_BAUD_RATE  // use TMC_BAUD_RATE for Serial1 if defined
      Serial1.begin(TMC_BAUD_RATE, SERIAL_8N1, HARDWARE_SERIAL1_RX, HARDWARE_SERIAL1_TX);
    #else  // use default BAUDRATE if TMC_BAUD_RATE not defined
      Serial1.begin(BAUDRATE, SERIAL_8N1, HARDWARE_SERIAL1_RX, HARDWARE_SERIAL1_TX);
    #endif
  #endif
  #if defined(HARDWARE_SERIAL2_RX) && defined(HARDWARE_SERIAL2_TX)
    HardwareSerial Serial2(2);
    #ifdef TMC_BAUD_RATE  // use TMC_BAUD_RATE for Serial1 if defined
      Serial2.begin(TMC_BAUD_RATE, SERIAL_8N1, HARDWARE_SERIAL2_RX, HARDWARE_SERIAL2_TX);
    #else  // use default BAUDRATE if TMC_BAUD_RATE not defined
      Serial2.begin(BAUDRATE, SERIAL_8N1, HARDWARE_SERIAL2_RX, HARDWARE_SERIAL2_TX);
    #endif
  #endif

  // Initialize the i2s peripheral only if the I2S stepper stream is enabled.
  // The following initialization is performed after Serial1 and Serial2 are defined as
  // their native pins might conflict with the i2s stream even when they are remapped.
  #if ENABLED(USE_ESP32_EXIO)
    YSerial2.begin(460800 * 3, SERIAL_8N1, 16, 17);
  #elif ENABLED(I2S_STEPPER_STREAM)
    i2s_init();
  #endif
  #endif
  SERIAL_IMPL.println("Hal init done");
}

void GcodeSuite::M39() {
  if (parser.seen('P')){
    PerMachineProbePressureCompensation = parser.value_float();
    Preferences prefs;
    prefs.begin("machine");
    prefs.putFloat("probe_p", PerMachineProbePressureCompensation);
    prefs.end();
  }
  else if (parser.seen('F')){
    FloatingFactor = parser.value_float();
    Preferences prefs;
    prefs.begin("machine");
    prefs.putFloat("probe_f", FloatingFactor);
    prefs.end();
  }
  SERIAL_IMPL.printf("Using: PComp = %f, FFac = %f\n", PerMachineProbePressureCompensation, FloatingFactor);
}
volatile int16_t adcCache[4] = {26000, 26000, 26000, 26000};
volatile bool needsConversion[4] = {0, 0, 0, 0};

 // All displays share the MarlinUI class
 #include "..\..\lcd\marlinui.h"
 extern MarlinUI ui;
 

long lastLoadCellLoop = 0;
long lastPCFSync = 0;
void MarlinHAL::idletask() {
  //SERIAL_IMPL.println("update_buttons Idle()");
  //ui.update_buttons();
  if (millis() - lastLoadCellLoop > 13) {// < 80hz
    lastLoadCellLoop = millis();
    LoadCellLoop();
  }
#if USE_ESP32_PCF8574
  if (millis() - lastPCFSync >= 1) {// at most 1khz
    lastPCFSync = millis();
    if (xSemaphoreTake(xPCFIOMutex, 0)) {
      pcf1_write_pending = true;
      pcf2_write_pending = true; // this is to refresh PCF in case it has reset due to a brown out.
      PCFSync();
      xSemaphoreGive(xPCFIOMutex);
    }
  }
#endif
  UILoop();
  #if BOTH(WIFISUPPORT, OTASUPPORT)
    OTA_handle();
  #endif
  //TERN_(ESP3D_WIFISUPPORT, esp3dlib.idletask());

  #if ENABLED(M3DPrintVueSupport)
    M3DPrintVueLoop();
  #endif
  // for (int i = 0; i < 4; i++){
  //   if (needsConversion[i]){
  //     int16_t newVal = ads.readADC_SingleEnded(i) * 0.03875;
  //     if (abs(newVal - adcCache[i]) > 50){
  //       adcCache[i] = ((long)(newVal * 5 + adcCache[i] * 95)) / 100;  
  //     }
  //     else{
  //       adcCache[i] = newVal;
  //       needsConversion[i] = false;
  //     }
  //     //SERIAL_IMPL.println(adcCache[i]);
  //   }
  // }
  
  ui.update_buttons();
  
}

uint8_t MarlinHAL::get_reset_source() { return rtc_get_reset_reason(1); }

void MarlinHAL::reboot() { ESP.restart(); }

void _delay_ms(int delay_ms) { delay(delay_ms); }

// return free memory between end of heap (or end bss) and whatever is current
int MarlinHAL::freeMemory() { return ESP.getFreeHeap(); }

// ------------------------
// Watchdog Timer
// ------------------------
#if ENABLED(USE_WATCHDOG)

  #define WDT_TIMEOUT_US TERN(WATCHDOG_DURATION_8S, 8000000, 4000000) // 4 or 8 second timeout

  extern "C" {
    bool heaterPinsEnabled = true;
    void disableHeaterPins(){
      heaterPinsEnabled = false;
      delay(1); // confirms we are outside pcf writes that takes 200us
    }
    void enableHeaterPins(){
      heaterPinsEnabled = true;
    }
    esp_err_t esp_task_wdt_reset();
  
    // Declare original weak functions from the ESP32 core
    extern void __digitalWrite(uint8_t pin, uint8_t val);
    extern int  __digitalRead(uint8_t pin);
    //extern uint16_t __analogRead(uint8_t pin);
  // Override digitalWrite
void digitalWrite(uint8_t pin, uint8_t val) {
#if USE_ESP32_PCF8574
  if (pin >= 200 && pin < 216) {
    uint8_t bit = pin - 200;

    if (((pcfWriteMapConfirmed >> bit) & 1) == val) return; // already up to date

    if (val) {
      pcfMapToSync |= (1 << bit);
    }
    else {
      pcfMapToSync &= ~(1 << bit);
    }
    if (bit < 8)
      pcf1_write_pending = true;
    else
      pcf2_write_pending = true;

    if (PCFIsBusy){ // PCF is busy. We need to see if this is a high priority request
      // we can let it go through only if we have a lock on the bits
      if ((1 << bit) & lockedBits){        
        PCFSync(true);
      }
      // can't sync in any way
      // We have already set the flags, priority user will sync when the lock is released
      return;
    }
    if (pin == Z_DIR_PIN) {// 2nd highest priority by default
      if (PCFSync()) // Done! Return
        return;
      else { // Give it one more try. Most probably will fail too. Its a soft failure, not a hard one.
        if (PCFSync())
          return;
        //else
          // At least let us queue it up.
      }
    }
    if (xPortInIsrContext()) {
      // We can trigger a sync in the io task
      if (pcfTaskHandle != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(pcfTaskHandle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
      return;
    }

    // Not in ISR, not busy, sync now
    if (xSemaphoreTake(xPCFIOMutex, portMAX_DELAY)) {
     PCFSync();
      xSemaphoreGive(xPCFIOMutex);
    }
    // This is double write. Can only happen in task from another core.
    return;
  }
  else 
#endif
  if (pin == 217){ // Probe enable disable
    ProbeEnable = val;
  }
  else if (pin == HEATER_BED_PIN){ // Probe enable disable
    // we actually done have a heater. Skip it.
  }
  else
    __digitalWrite(pin, val);
}

    
  // Override digitalRead
int digitalRead(uint8_t pin) {
#if USE_ESP32_PCF8574
  if (pin >= 200 && pin < 216) {
    uint8_t bit = pin - 200;

    if (bit < 8)
      pcf1_read_pending = true;
    else
      pcf2_read_pending = true;
    if (PCFIsBusy){ // can't sync now. We have set the flags, priority user will sync when the lock is released      
      if ((1 << bit) & lockedBits){        
        PCFSync(true);
      }
      return (pcfReadCache >> bit) & 1; // can't sync just return the last
    }
    if (xPortInIsrContext()) {
      if (!PCFSync()){ // try to sync now. If not possible, que the task    
        //SERIAL_IMPL.printf("ri%d\n", pin);
        if (pcfTaskHandle != NULL) { // trigger sync in parallel
          BaseType_t xHigherPriorityTaskWoken = pdFALSE;
          vTaskNotifyGiveFromISR(pcfTaskHandle, &xHigherPriorityTaskWoken);
          portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
      }
    } else if (xSemaphoreTake(xPCFIOMutex, portMAX_DELAY)) { // we can sync now      
      //SERIAL_IMPL.printf("rs%d\n", pin);
      PCFSync();
      xSemaphoreGive(xPCFIOMutex);
    }
    else {      
      //SERIAL_IMPL.printf("rn%d\n", pin);
    }
    return (pcfReadCache >> bit) & 1;
  }
  else
#endif
  if (pin == 216) {
    // Loadcell
    return LoadCellProbe();
  }
  else
    return __digitalRead(pin);
}
} // extern "C"
  

  void watchdogSetup() {
    // do whatever. don't remove this function.
  }

  void MarlinHAL::watchdog_init() {
    // TODO
  }

  // Reset watchdog.
  void MarlinHAL::watchdog_refresh() { esp_task_wdt_reset(); }

#endif

// ------------------------
// ADC
// ------------------------


#define ADC1_CHANNEL(pin) ADC1_GPIO ## pin ## _CHANNEL
#include "soc/adc_channel.h"
adc1_channel_t get_channel(int pin) {
  switch (pin) {
    case 1: return ADC1_CHANNEL(1);
    case 2: return ADC1_CHANNEL(2);
    case 3: return ADC1_CHANNEL(3);
    case 4: return ADC1_CHANNEL(4);
    case 5: return ADC1_CHANNEL(5);
    case 6: return ADC1_CHANNEL(6);
    case 7: return ADC1_CHANNEL(7);
    case 8: return ADC1_CHANNEL(8);
    case 9: return ADC1_CHANNEL(9);
    case 10: return ADC1_CHANNEL(10);
  }
  return ADC1_CHANNEL_MAX;
}

void adc1_set_attenuation(adc1_channel_t chan, adc_atten_t atten) {
  if (attenuations[chan] != atten) {
    adc1_config_channel_atten(chan, atten);
    attenuations[chan] = atten;
  }
}

void MarlinHAL::adc_init() {
  // Configure ADC
  // ads.setGain(GAIN_ONE);
  // ads.setDataRate(RATE_ADS1015_128SPS);
  //ads.setDataRate(RATE_ADS1015_3300SPS);  
  //ads.startADCReading(ADS1X15_REG_CONFIG_MUX_DIFF_0_1, /*continuous=*/true);
  adc1_config_width(ADC_WIDTH_12Bit);

  // Configure channels only if used as (re-)configuring a pin for ADC that is used elsewhere might have adverse effects
  TERN_(HAS_TEMP_ADC_0,        adc1_set_attenuation(get_channel(TEMP_0_PIN), ADC_ATTEN_11db));
  TERN_(HAS_TEMP_ADC_1,        adc1_set_attenuation(get_channel(TEMP_1_PIN), ADC_ATTEN_11db));
  TERN_(HAS_TEMP_ADC_2,        adc1_set_attenuation(get_channel(TEMP_2_PIN), ADC_ATTEN_11db));
  TERN_(HAS_TEMP_ADC_3,        adc1_set_attenuation(get_channel(TEMP_3_PIN), ADC_ATTEN_11db));
  TERN_(HAS_TEMP_ADC_4,        adc1_set_attenuation(get_channel(TEMP_4_PIN), ADC_ATTEN_11db));
  TERN_(HAS_TEMP_ADC_5,        adc1_set_attenuation(get_channel(TEMP_5_PIN), ADC_ATTEN_11db));
  TERN_(HAS_TEMP_ADC_6,        adc2_set_attenuation(get_channel(TEMP_6_PIN), ADC_ATTEN_11db));
  TERN_(HAS_TEMP_ADC_7,        adc3_set_attenuation(get_channel(TEMP_7_PIN), ADC_ATTEN_11db));
  TERN_(HAS_HEATED_BED,        adc1_set_attenuation(get_channel(TEMP_BED_PIN), ADC_ATTEN_11db));
  TERN_(HAS_TEMP_CHAMBER,      adc1_set_attenuation(get_channel(TEMP_CHAMBER_PIN), ADC_ATTEN_11db));
  TERN_(HAS_TEMP_PROBE,        adc1_set_attenuation(get_channel(TEMP_PROBE_PIN), ADC_ATTEN_11db));
  TERN_(HAS_TEMP_COOLER,       adc1_set_attenuation(get_channel(TEMP_COOLER_PIN), ADC_ATTEN_11db));
  TERN_(HAS_TEMP_BOARD,        adc1_set_attenuation(get_channel(TEMP_BOARD_PIN), ADC_ATTEN_11db));
  TERN_(FILAMENT_WIDTH_SENSOR, adc1_set_attenuation(get_channel(FILWIDTH_PIN), ADC_ATTEN_11db));

  // Note that adc2 is shared with the WiFi module, which has higher priority, so the conversion may fail.
  // That's why we're not setting it up here.

  // Calculate ADC characteristics (i.e., gain and offset factors for each attenuation level)
  for (int i = 0; i < ADC_ATTEN_MAX; i++) {
    esp_adc_cal_characterize(ADC_UNIT_1, (adc_atten_t)i, ADC_WIDTH_BIT_12, V_REF, &characteristics[i]);

    // Change attenuation 100mV below the calibrated threshold
    thresholds[i] = esp_adc_cal_raw_to_voltage(4095, &characteristics[i]);
  }
}

#ifndef ADC_REFERENCE_VOLTAGE
  #define ADC_REFERENCE_VOLTAGE 3.3
#endif

uint32_t kp0Read = 0;
uint32_t kp1Read = 0;
uint32_t readADCMV(const pin_t pin){
  // if (pin == TEMP_CHAMBER_PIN)
  //   return kp0Read;
  // else 
  if (pin == TEMP_BED_PIN)
    return kp1Read;
  else
    return 0;
}
void MarlinHAL::adc_start(const pin_t pin) {
  
  // if (pin >= 216 && pin < 220){
  //   adc_result = analogRead(pin);
  //   return; // Its on ADS
  // }
  const adc1_channel_t chan = get_channel(pin);
  // if (pin == TEMP_BED_PIN){
  //   kp1Read = adc1_get_raw(chan);
  //   return;
  // }
  uint32_t mv;
  esp_adc_cal_get_voltage((adc_channel_t)chan, &characteristics[attenuations[chan]], &mv);

  // Lets try to compensate for the voltage drop in the presence of the heater.
  // Th1:
  // Without Heater | With Heater 1
  // 3198          | 3195
  // 370          | 505

  uint32_t mvToReturn = mv;
#if USE_ESP32_PCF8574
  if ((pcfMapToSync >> 10) & 0b1 && pin == 34) {// Heater 1
    // Do the linearization
    mvToReturn = map(mv, 505, 3195, 370, 3197);
  }
#endif 
  uint16_t this_adc_result = mvToReturn * isr_float_t(1023) / isr_float_t(ADC_REFERENCE_VOLTAGE) / isr_float_t(1000);
  if (pin == TEMP_0_PIN || pin == TEMP_1_PIN){
    // Check if pin already has an entry
    auto it = adcMap.find(pin);

    if (it == adcMap.end()) {
        // First time seeing this pin — insert as-is
        adcMap[pin] = this_adc_result;
    } else {
        // Average with previous value
        uint16_t oldValue = it->second;
        uint16_t averaged = (oldValue * 96 + this_adc_result * 4) / 100;
        adcMap[pin] = averaged;
    }
    adc_result = adcMap[pin];
  }
  else{
    adc_result = this_adc_result;
    // if (pin == TEMP_CHAMBER_PIN)
    //   kp0Read = this_adc_result;
    // else 
    if (pin == TEMP_BED_PIN){
      kp1Read = this_adc_result;
    }
  }
  // Change the attenuation level based on the new reading
  adc_atten_t atten;
  if (mv < thresholds[ADC_ATTEN_DB_0] - 100)
    atten = ADC_ATTEN_DB_0;
  else if (mv > thresholds[ADC_ATTEN_DB_0] - 50 && mv < thresholds[ADC_ATTEN_DB_2_5] - 100)
    atten = ADC_ATTEN_DB_2_5;
  else if (mv > thresholds[ADC_ATTEN_DB_2_5] - 50 && mv < thresholds[ADC_ATTEN_DB_6] - 100)
    atten = ADC_ATTEN_DB_6;
  else if (mv > thresholds[ADC_ATTEN_DB_6] - 50)
    atten = ADC_ATTEN_DB_11;
  else return;

  adc1_set_attenuation(chan, atten);
}

// ------------------------
// PWM
// ------------------------

int8_t channel_for_pin(const uint8_t pin) {
  for (int i = 0; i <= CHANNEL_MAX_NUM; i++)
    if (chan_pin[i] == pin) return i;
  return -1;
}

// get PWM channel for pin - if none then attach a new one
// return -1 if fail or invalid pin#, channel # (0-15) if success
int8_t get_pwm_channel(const pin_t pin, const uint32_t freq, const uint16_t res) {
  if (!WITHIN(pin, 1, MAX_PWM_IOPIN)) return -1; // Not a hardware PWM pin!
  int8_t cid = channel_for_pin(pin);
  if (cid >= 0) return cid;

  // Find an empty adjacent channel (same timer & freq/res)
  for (int i = 0; i <= CHANNEL_MAX_NUM; i++) {
    if (chan_pin[i] == 0) {
      if (chan_pin[i ^ 0x1] != 0) {
        if (pwmInfo[i / 2].freq == freq && pwmInfo[i / 2].res == res) {
          chan_pin[i] = pin; // Allocate PWM to this channel
          ledcAttachPin(pin, i);
          return i;
        }
      }
      else if (cid == -1)    // Pair of empty channels?
        cid = i & 0xFE;      // Save lower channel number
    }
  }
  // not attached, is an empty timer slot avail?
  if (cid >= 0) {
    chan_pin[cid] = pin;
    pwmInfo[cid / 2].freq = freq;
    pwmInfo[cid / 2].res = res;
    ledcSetup(cid, freq, res);
    ledcAttachPin(pin, cid);
  }
  return cid; // -1 if no channel avail
}

void MarlinHAL::set_pwm_duty(const pin_t pin, const uint16_t v, const uint16_t v_size/*=_BV(PWM_RESOLUTION)-1*/, const bool invert/*=false*/) {
  #if ENABLED(I2S_STEPPER_STREAM)
    if (pin > 127) {
      const uint8_t pinlo = pin & 0x7F;
      pwm_pin_t &pindata = pwm_pin_data[pinlo];
      const uint32_t duty = map(invert ? v_size - v : v, 0, v_size, 0, pindata.pwm_cycle_ticks);
      if (duty == 0 || duty == pindata.pwm_cycle_ticks) { // max or min (i.e., on/off)
        pindata.pwm_duty_ticks = 0;  // turn off PWM for this pin
        duty ? SBI32(i2s_port_data, pinlo) : CBI32(i2s_port_data, pinlo); // set pin level
      }
      else
        pindata.pwm_duty_ticks = duty; // PWM duty count = # of 4µs ticks per full PWM cycle
    }
    else
  #endif
    {
      const int8_t cid = get_pwm_channel(pin, PWM_FREQUENCY, PWM_RESOLUTION);
      if (cid >= 0) {
        const uint32_t duty = map(invert ? v_size - v : v, 0, v_size, 0, _BV(PWM_RESOLUTION)-1);
        ledcWrite(cid, duty);
      }
    }
}

void removePWMOnPin(const int16_t pin){  
      const int8_t cid = channel_for_pin(pin);
      if (cid >= 0) { // has an assignment
        ledcDetachPin(chan_pin[cid]);
        chan_pin[cid] = 0;              // remove old freq channel
      }
}
int8_t MarlinHAL::set_pwm_frequency(const pin_t pin, const uint32_t f_desired) {
  #if ENABLED(I2S_STEPPER_STREAM)
    if (pin > 127) {
      pwm_pin_data[pin & 0x7F].pwm_cycle_ticks = 1000000UL / f_desired / 4; // # of 4µs ticks per full PWM cycle
      return 0;
    }
    else
  #endif
    {
      const int8_t cid = channel_for_pin(pin);
      if (cid >= 0) {
        if (f_desired == ledcReadFreq(cid)) return cid; // no freq change
        ledcDetachPin(chan_pin[cid]);
        chan_pin[cid] = 0;              // remove old freq channel
      }
      return get_pwm_channel(pin, f_desired, PWM_RESOLUTION); // try for new one
    }
}

// use hardware PWM if avail, if not then ISR
void analogWrite(const pin_t pin, const uint16_t value, const uint32_t freq/*=PWM_FREQUENCY*/, const uint16_t res/*=8*/) { // always 8 bit resolution!
  // Use ledc hardware for internal pins
  const int8_t cid = get_pwm_channel(pin, freq, res);
  if (cid >= 0) {
    ledcWrite(cid, value); // set duty value
    return;
  }

  // not a hardware PWM pin OR no PWM channels available
  int idx = -1;

  // Search Pin
  for (int i = 0; i < numPWMUsed; ++i)
    if (pwmState[i].pin == pin) { idx = i; break; }

  // not found ?
  if (idx < 0) {
    // No slots remaining
    if (numPWMUsed >= MAX_PWM_PINS) return;

    // Take new slot for pin
    idx = numPWMUsed;
    pwmState[idx].pin = pin;
    // Start timer on first use
    if (idx == 0) HAL_timer_start(MF_TIMER_PWM, PWM_TIMER_FREQUENCY);

    ++numPWMUsed;
  }

  // Use 7bit internal value - add 1 to have 100% high at 255
  pwmState[idx].value = (value + 1) / 2;
}

// Handle PWM timer interrupt
HAL_PWM_TIMER_ISR() {
  HAL_timer_isr_prologue(MF_TIMER_PWM);

  static uint8_t count = 0;

  for (int i = 0; i < numPWMUsed; ++i) {
    if (count == 0)                   // Start of interval
      digitalWrite(pwmState[i].pin, pwmState[i].value ? HIGH : LOW);
    else if (pwmState[i].value == count)   // End of duration
      digitalWrite(pwmState[i].pin, LOW);
  }

  // 128 for 7 Bit resolution
  count = (count + 1) & 0x7F;

  HAL_timer_isr_epilogue(MF_TIMER_PWM);
}

#endif // ARDUINO_ARCH_ESP32
