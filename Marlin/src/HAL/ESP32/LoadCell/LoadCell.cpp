#include "../../../inc/MarlinConfig.h"
 #include "HX711/HX711.h"
 #include "LoadCell.h"
 #define HxData 201
 #define HxClk  15

 HX711_ADC load(HxData, HxClk); 
 
 void LoadCellSetup(){
  Serial.println("Starting HX711");
  
  digitalWrite(HxData, 1); // make sure its floating to let us read
  digitalWrite(200, 1); // default to high Z coz now we are using pin 15 for testing.
  delay(1);
  load.begin();
  load.tareAsync();
 }
 float floatingAverage = 0;
 float firstAfterTare = true;
  
 bool ProbeEnable = true; // on by default for tarring
 float lastReading = 0;
 int exampleReadingCount = 0;
 float threshold = 1;
 float rawValueFilterFactor = 0.1F;
 float FloatingFactor = 0.008F;
 #define FilterOutSeriesOfErraticValue 5
 int lastReturn  = 0;
 float lastAnalogReturn = 0;
 long lastProbe = 1000; // will force a tare
 float readProbeAnalog(){
    return lastAnalogReturn;
 }
 int LoadCellProbe(){
    lastProbe = millis();
    return lastReturn;
 }
 bool debugOn = false;
 #define debug(x) {if (debugOn)SERIAL_IMPL.print((x));}
 long lastUpdateAt = 0;
 void LoadCellLoop(){

    //debugOn = (millis() - lastProbe) < 1000;
    debugOn = ProbeEnable;
    if (load.getTareAsyncStatus()){
        //SERIAL_IMPL.println("Taring.");
        load.tareAsyncLoop();
        firstAfterTare = true;
        return;
    }

    // digitalWrite(HxData, HIGH);
    // delay(1000);
    // digitalWrite(HxData, LOW);
    // delay(1000);
    //if (ProbeEnable){
        if (millis() - lastUpdateAt < 12)
            return;
        lastUpdateAt = millis();
        if (load.update()){
            // Serial.print(millis());
            // Serial.print(": ");
            float reading = load.getData();
            //reading = lastReading * (1 - rawValueFilterFactor) + reading * rawValueFilterFactor;
            // SERIAL_IMPL.print(reading);
            // SERIAL_IMPL.print("\t");
            // Remove chances of false triggers. 
            if (abs(reading - lastReading) > 1){
                exampleReadingCount--;
                lastUpdateAt = millis() + 50; // Skip the next few cycles
                    if (exampleReadingCount < 0)
                exampleReadingCount = 0;
                    
                if (exampleReadingCount <= 0)
                    lastReading = reading; // change the ref this this for the next time, accepting this data
                else
                    reading = lastReading; // use previous data
            }
            else
            {
                lastReading = reading;	
                exampleReadingCount++;
                    if (exampleReadingCount > FilterOutSeriesOfErraticValue)
                exampleReadingCount = FilterOutSeriesOfErraticValue;
            }

            // SERIAL_IMPL.print(reading);
            // SERIAL_IMPL.print("\t");
            
            debug("PE: ");
            debug(ProbeEnable);
            debug(", Pr: ");
            debug(exampleReadingCount);
            debug(", ");
            float floatingFactor = FloatingFactor;
            if (firstAfterTare){
            floatingFactor = 1; 
            firstAfterTare = false;
            }

            floatingAverage = reading * floatingFactor + floatingAverage * (1 - floatingFactor);
            float offsetCorrected = floatingAverage - reading;
            lastAnalogReturn = offsetCorrected;
            debug(floatingAverage);
            debug("(fAvg) -");
            debug(reading);
            debug("(reading) = ");
            debug(offsetCorrected);
            debug("(used) ");
            debug(offsetCorrected > threshold ? ">":(offsetCorrected < -threshold ? "<":"~="));
            debug(threshold);
            debug(" => ");
            debug(offsetCorrected > threshold ? 1:0);
            
            // SERIAL_IMPL.print("\t");
            // SERIAL_IMPL.print(10);
            // SERIAL_IMPL.print("\t");
            // SERIAL_IMPL.print(-10);
            // Serial.print("\t");
            // Serial.print(offsetCorrected);
            // SERIAL_IMPL.println();
            debug("\r\n");

            lastReturn = offsetCorrected > threshold ? 1:0;
        }
        // }
    // else if (!ProbeEnable){
    //     //debug("Probe Disabled\r\n");
    // }
 }