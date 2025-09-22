#include "../../../inc/MarlinConfig.h"
 #include "HX711/HX711.h"
 #include "LoadCell.h"

 HX711_ADC load(HxData, HxClk); 
 
 void LoadCellSetup(){
  //Serial.println("Starting HX711");
  
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
 float threshold = 3.0;
 float fluctuationTolerance = 0.2F; // how much fluctuation in the reading to ignore
 float rawValueFilterFactor = 0.1F;
 float FloatingFactor = 0.002F;
 #define FilterOutSeriesOfErraticValue 5
 int lastReturn  = 0;
 float lastAnalogReturn = 0;
 float lastRawReturn = 0;
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
    debugOn = true;
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
    if (ProbeEnable){
            
        // if (millis() - lastUpdateAt < 100)
        //     return;
        // lastUpdateAt = millis();
        // digitalWrite(HxClk, (millis() % 4000) > 2000);
        // SERIAL_IMPL.print("Clk: ");
        // SERIAL_IMPL.print((millis() % 4000) > 2000);
        // SERIAL_IMPL.print("Data: ");
        // SERIAL_IMPL.print(digitalRead(HxData));
        // SERIAL_IMPL.println();
        // return;

        if (millis() - lastUpdateAt < 12)
            return;
        if (load.update()) { // reset the timer only in case the update was successful, otherwise, retry as soon as possible
            lastUpdateAt = millis();
            // Serial.print(millis());
            // Serial.print(": ");
            lastRawReturn = load.getData();
            //reading = lastReading * (1 - rawValueFilterFactor) + reading * rawValueFilterFactor;
            // SERIAL_IMPL.print(reading);
            // SERIAL_IMPL.print("\t");

            // Remove chances of false triggers. 
            if (abs(lastRawReturn - lastReading) > 3){ // too big a jump in one iteration
                exampleReadingCount--;
                lastUpdateAt = millis() + 50; // Skip the next few cycles
                    if (exampleReadingCount < 0)
                exampleReadingCount = 0;
                    
                if (exampleReadingCount <= 0)
                    lastReading = lastRawReturn; // change the ref this this for the next time, accepting this data
                else
                    lastRawReturn = lastReading; // use previous data
            }
            else
            {
                lastReading = lastRawReturn;	
                exampleReadingCount++;
                    if (exampleReadingCount > FilterOutSeriesOfErraticValue)
                exampleReadingCount = FilterOutSeriesOfErraticValue;
            }

            // SERIAL_IMPL.print(reading);
            // SERIAL_IMPL.print("\t");
            
            // SERIAL_IMPL.print("PE: ");
            // SERIAL_IMPL.print(ProbeEnable);
            SERIAL_IMPL.print(", Pr: ");
            SERIAL_IMPL.print(exampleReadingCount);
            SERIAL_IMPL.print(", ");
            float floatingFactor = FloatingFactor;
            if (firstAfterTare){
                floatingFactor = 1; 
                firstAfterTare = false;
            }

            floatingAverage = lastRawReturn * floatingFactor + floatingAverage * (1 - floatingFactor);
            lastAnalogReturn = floatingAverage - lastRawReturn;
            SERIAL_IMPL.print(floatingAverage);
            SERIAL_IMPL.print("(fAvg) -");
            SERIAL_IMPL.print(lastRawReturn);
            SERIAL_IMPL.print("(reading) = ");
            SERIAL_IMPL.print(lastAnalogReturn);
            SERIAL_IMPL.print("(used) ");
            if (lastAnalogReturn > threshold + fluctuationTolerance){
                SERIAL_IMPL.print(" > ");
                SERIAL_IMPL.print(threshold);
                SERIAL_IMPL.print(" + ");
                SERIAL_IMPL.print(fluctuationTolerance);                
                SERIAL_IMPL.print("(");
                SERIAL_IMPL.print(threshold + fluctuationTolerance);
                SERIAL_IMPL.print(")");
            }
            else if (lastAnalogReturn < threshold - fluctuationTolerance) {
                SERIAL_IMPL.print(" < ");                
                SERIAL_IMPL.print(threshold);
                SERIAL_IMPL.print(" - ");
                SERIAL_IMPL.print(fluctuationTolerance);                
                SERIAL_IMPL.print("(");
                SERIAL_IMPL.print(threshold - fluctuationTolerance);
                SERIAL_IMPL.print(")");
            }
            else{                
                SERIAL_IMPL.print(" <> ");
                SERIAL_IMPL.print(threshold);
            }
            //SERIAL_IMPL.print(lastAnalogReturn > threshold ? ">":(lastAnalogReturn < -threshold ? "<":"~="));

            // SERIAL_IMPL.print(threshold);
            // SERIAL_IMPL.print(" => ");
            // SERIAL_IMPL.print(lastAnalogReturn > threshold ? 1:0);
            
            // SERIAL_IMPL.print("\t");
            // SERIAL_IMPL.print(10);
            // SERIAL_IMPL.print("\t");
            // SERIAL_IMPL.print(-10);
            // Serial.print("\t");
            // Serial.print(offsetCorrected);
            // SERIAL_IMPL.println();
            SERIAL_IMPL.print("\r\n");
            if (lastReturn == 0) {
                if (lastAnalogReturn > threshold + fluctuationTolerance){ // Be convinced only if we are way above the fluctionation zone
                    lastReturn = 1;
                }
            }
            else { // Be convinced only if we are way below the fluctionation zone
                if (lastAnalogReturn < threshold - fluctuationTolerance){
                    lastReturn = 0;
                }
            }
        }
    }
    // else if (!ProbeEnable){
    //     //debug("Probe Disabled\r\n");
    // }
 }
 
 #define steps_per_mm DEFAULT_AXIS_STEPS_PER_UNIT_Z // 6.0596F exactly
 void move_mm(float mm, float speed){
    int dir = mm > 0;
    if (mm < 0)
        mm *= -1.0F;
    digitalWrite(Z_DIR_PIN, INVERT_Z_DIR ? (1 - dir):dir);

    long stepsToMove = round((float)mm * steps_per_mm);
    int usPerStep = round(1.0F / speed / steps_per_mm * 1000000.0F); // int can be a bit inaccurate but the distance will still be accurate
    //SERIAL_IMPL.printf("Move: steps = %f, delay = %d\n", stepsToMove, usPerStep);
    for (long i = 0; i < stepsToMove; i++){
        
        digitalWrite(Z_STEP_PIN, 1);
        digitalWrite(Z_STEP_PIN, 0);
        delayMicroseconds(usPerStep);
    }
 }
 void removeLoadCellOffset(){
    SERIAL_IMPL.println("Averaging");
    for (int i = 0; i < 5; i++){
        LoadCellLoop();
        safe_delay(12);
    }
    floatingAverage = lastRawReturn;
    SERIAL_IMPL.println("RemovedOffset");
    for (int i = 0; i < 2; i++){
        LoadCellLoop();
        safe_delay(12);
    }
 }
extern void removePWMOnPin(const pin_t pin);
extern void disableESPMarlinTimers();
extern void enableESPMarlinTimers();
extern void enableHeaterPins();
extern void disableHeaterPins();
float do_blocking_move_to_dz_D8500(float dz, float fr_mm_s){
    
    SERIAL_IMPL.printf("Probe Test: dz = %f, fr_mm_s = %f\n", dz, fr_mm_s);
    removePWMOnPin(Z_STEP_PIN);
    SERIAL_IMPL.println("pin pwm removed"); delay(1);
    disableESPMarlinTimers();
    //disableHeaterPins();
    SERIAL_IMPL.println("timer disabled"); delay(1);
    pinMode(Z_STEP_PIN, OUTPUT);
    // Test
    removeLoadCellOffset();
    SERIAL_IMPL.println("offset removed"); delay(1);
    float distanceGone = 0;
    float stepSize = -0.05F;
    while(distanceGone > dz){
        move_mm(stepSize, 1);
        distanceGone += stepSize;
        LoadCellLoop();
        if (lastAnalogReturn > threshold){
            SERIAL_IMPL.println("Bed Touch");
            break;
        }
    }
    SERIAL_IMPL.printf("Distance Gone: %f\n", distanceGone);
    // delay(1000);
    // move_mm(-distanceGone, 5);

    SERIAL_IMPL.println("Move Done");
    // No need to attach the pwm again. It will automatically get set on next timer setting
    //enableHeaterPins();
    enableESPMarlinTimers();
    return distanceGone;
 }