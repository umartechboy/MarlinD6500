#include <Arduino.h>
#include <PCF8574.h>
#include <Wire.h>
PCF8574 pcf1(0x20);
PCF8574 pcf2(0x21); // Closer to ESP32
extern "C" {
    esp_err_t esp_task_wdt_reset();
  
    // Declare original weak functions from the ESP32 core
    extern void __digitalWrite(uint8_t pin, uint8_t val);
    extern int  __digitalRead(uint8_t pin);
    extern uint16_t __analogRead(uint8_t pin);
  uint16_t pcfMap = 0xFFFF;
  // Override digitalWrite
  void digitalWrite(uint8_t pin, uint8_t val) {
    
    if (pin >= 200 && pin < 216){    
      
      if ((pcfMap >> (pin - 200)) & 1 == val)
        return;
      if (val)
        pcfMap |=  ((uint16_t)(1 << (pin - 200)));
      else
        pcfMap &= ~((uint16_t)(1 << (pin - 200)));
          
      if (pin >= 200 && pin < 208) {
        pcf1.write(pin - 200, val & 1);
        // Serial.print("digitalWrite on PCF1 (");
        // Serial.print(pin - 200);
        // Serial.print(", ");
        // Serial.print(val);
        // Serial.println(")");
      }
      else {//if (pin >= 208 && pin < 216) {
          pcf2.write(pin - 208, val & 1);
          // Serial.print("digitalWrite on PCF2 (");
          // Serial.print(pin - 208);
          // Serial.print(", ");
          // Serial.print(val);
          // Serial.println(")");
      }
    }
    else 
      __digitalWrite(pin, val);       // Call the original
  }
    
  // Override digitalRead
  int digitalRead(uint8_t pin) {
    // Serial.print("digitalRead(");
    // Serial.print(pin);
    // Serial.print(") = ");
    // Serial.println(__digitalRead(pin));
    if (pin >= 200 && pin < 208){
    //   // Serial.print("digitalRead on PCF1 (");
    //   // Serial.print(pin - 200);`
    //   // Serial.print(") = ");
       int val = pcf1.read(pin - 200);
    //   // Serial.print(val);
    //   // Serial.println();
       return val;
    }
     else if (pin >= 208 && pin < 216){
    //   if (pin == 213) { // Z Stop       
    //     int16_t rawADS = ads.readADC_SingleEnded(0);
    //     Serial.print("analogRead on ADS = "); 
    //     Serial.print(rawADS);
    //     float V = ads.computeVolts(rawADS);
    //     rawADS = V / 3.3F * 1023;
    //     Serial.print(", V = "); 
    //     Serial.print(V);
    //     Serial.print(", V to ADC = "); 
    //     Serial.print(rawADS);
    //     Serial.print(", mapped 1023 = ");
    //     Serial.print(map(rawADS, 0, 32767, 0, 1023));
    //     Serial.print(", mapped 4095 = ");
    //     Serial.print(map(rawADS, 0, 32767, 0, 4095));
    //     Serial.println();
    //   }
    //   // Serial.print("digitalRead on PCF2 (");
    //   // Serial.print(pin - 208);
    //   // Serial.print(") = ");
       int val = pcf2.read(pin - 208);
    //   // Serial.print(val);
    //   // Serial.println();
       return val;
    }
    else
      return __digitalRead(pin);       // Call the original
  }
  
  } // extern "C"


  void beginDigitalIO() {
    Wire.begin();
    SERIAL_IMPL.println("Starting Wire and IO Expander");
    if (pcf1.begin()){
      SERIAL_IMPL.println("PCF1 Started");
    }
    else  
      SERIAL_IMPL.println("PCF1 Failed");
      
    if (pcf2.begin()){
      SERIAL_IMPL.println("PCF2 Started");
    }
    else  
      SERIAL_IMPL.println("PCF2 Failed");
      
    SERIAL_IMPL.println("Expanders On.");
    pcf1.write8(0);
    pcf2.write8(0);
    pcfMap = 0;
  }