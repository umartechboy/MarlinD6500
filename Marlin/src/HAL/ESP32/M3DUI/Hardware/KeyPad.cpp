#include "Keypad.h"
#include "..\Components\M3DUI.h"

int pinMap [] = {14, 13, 0, 12};
int baseValues [4];
#define readPad(i) (min(100, (min(100, (max(0, (baseValues[(((uint32_t)i) % 4)] - touchRead(pinMap[(((uint32_t)i) % 4)]))) * 100 / baseValues[(((uint32_t)i) % 4)]) * 2))) / 100.0F)

bool first = true;
Keys touchOnADCKeyPad_getKey(){
    if (first){
        for (int i = 0; i < 4; i++){
          for (int ri = 0; ri < 10; ri ++){
            baseValues [i] += touchRead(pinMap[i]);
            delay(1);
          }
          baseValues[i] /= 10;
        }
      first = false;
    }
    // Make a cache
    float all [4];
    for (int i = 0; i < 4; i++) {
      all[i] = readPad(i);
    }
#if DebugKeys
    SERIAL_IMPL.printf("Cache values: %f %f %f %f", all[0], all[1], all[2], all[3]);
#endif
    // Check if its the middle button
    int aboveZero = 0;
    for (int i = 0; i < 4; i++) {
      if (all[i] > 0.05){
          aboveZero++;
        }
    }
    
#if DebugKeys
    SERIAL_IMPL.printf(", Above zero 1: %d", aboveZero);
#endif
    if (aboveZero >= 4){      
      SERIAL_IMPL.println();
      return Keys::KEYPAD_MIDDLE;
    }
    // Check if its a diagonal button
    aboveZero = 0;
    for (int i = 0; i < 4; i++) {
      if (all[i] > 0.1){
          aboveZero++;
        }
    }
    
#if DebugKeys
    SERIAL_IMPL.printf(", Above zero 2: %d", aboveZero);
#endif
    if (aboveZero == 2){
      // find the first ind
      
      for (int i = 0; i < 4; i++) {
        if (all[i] > 0.1){
          if (i == 0) {
            if (all[3] > 0.1) { 
              continue;
            }
          }
#if DebugKeys
          SERIAL_IMPL.printf(", %d\n", i * 2 + 2 );
#endif
          return (Keys)(i * 2 + 2);
        }
      }
    }
    // Single Button Detection
    int maxI = 0;
    for (int i = 0; i < 4; i++) {
      if (all[i] > all[maxI])
        maxI = i;
    }
    
    if (all[maxI] < 0.5){
      
#if DebugKeys
      SERIAL_IMPL.printf(", No key\n");
#endif
      return Keys::KEYPAD_NONE;
    }
    else{
      
#if DebugKeys
      SERIAL_IMPL.printf(", One Key %d\n", maxI * 2 + 1 );
#endif
      return (Keys)(maxI * 2 + 1);
    }
}

#define AddKey(k, i) (Keys)(((k) + (i) > 8) ? ((k) + (i) - 8):(((k) + (i) < 1) ? ((k) + (i) + 8):((k) + (i))))

void KeyPad::Loop(MenuHost* host){
  if (millis() - lastKeyCheck > 30){
    lastKeyCheck = millis();
    Keys key = touchOnADCKeyPad_getKey();
    if (key){ // we just need to wait for it to go up
        if (key != lastKeyDown && lastKeyDown != Keys::KEYPAD_NONE){ // dial rotate or swipe
        SERIAL_IMPL.printf("Dial Rotate: %d > %d\n", lastKeyDown, key);

        if (key == AddKey(lastKeyDown, 1)) {
            if (host->CurrentStep){ 
            host->CurrentStep->IncrementValue(); 
            if (millis() - lastIncrementSendAt < 30) 
                host->CurrentStep->IncrementValue(); // accelerate
            lastIncrementSendAt = millis(); 
            }
        }
        else if (key ==  AddKey(lastKeyDown, -1)) {
            if (host->CurrentStep) { 
            host->CurrentStep->DecrementValue(); 
            if (millis() - lastIncrementSendAt < 30) 
                host->CurrentStep->DecrementValue(); // accelerate
            lastIncrementSendAt = millis(); 
            }
        }
        else {
            lastKeyDown = Keys::KEYPAD_NONE;
        }
        }
        else
        keyToSend = key;
        lastKeyDown = key;
    }
    else {
      //  cannot be a dial rotate
      lastKeyDown = Keys::KEYPAD_NONE;
      possibleSwipFrom = Keys::KEYPAD_NONE;
      if(keyToSend){
        SERIAL_IMPL.printf("Key proessed: %d\n", keyToSend);
        if (host->TargetStep == 0){ // No step transitions in process
          if (host->Retro){
            // Complete Retro navigation
            if (host->CurrentNotification){
              SERIAL_IMPL.println("Middle or back key pressed");
              host->CurrentNotification->HandleKeyUp(keyToSend);
            }
            else if (keyToSend == KEYPAD_OPTIONS){
              if (host->CurrentStep)
                host->GotoRetroOptionsStep();
            }
            else if (keyToSend == KEYPAD_BACK){
              if (host->CurrentStep)
                if (host->CurrentStep->PreviousStep)
                  host->GotoPreviousStep();
            }
            else { // Send all the keys to the step
              if (host->CurrentStep)
                host->CurrentStep->HandleKeyUp(keyToSend);
            }
          }
          else{ 
            // Normal mode
            if (keyToSend == KEYPAD_MIDDLE){
              SERIAL_IMPL.println("Middle key pressed");
              // begin buttons overlay
              if(host->stepAnimationStage == StepAnimationStage::MainStep && keyToSend == KEYPAD_MIDDLE){
                  host->stepAnimationStage = StepAnimationStage::GoingToOverLay;
                  if (host->CurrentStep)
                    host->CurrentStep->FocusChanged(StepAnimationStage::GoingToOverLay);
                  host->moveToMenuAfterStepAnim = 0;
                  host->ResetAnimationProgress(250);
              }
              else if (host->stepAnimationStage == StepAnimationStage::InOverlay){
                  host->stepAnimationStage = StepAnimationStage::GoingToStep;
                  if (host->CurrentStep)
                    host->CurrentStep->FocusChanged(StepAnimationStage::GoingToStep);
                  host->moveToMenuAfterStepAnim = 0;
                  host->ResetAnimationProgress(250);
              }
            }
            else if (keyToSend == KEYPAD_UP_LEFT && host->CurrentStep->PreviousStep && host->stepAnimationStage == StepAnimationStage::InOverlay){
              host->GotoPreviousStep();
            }
            else if (keyToSend == KEYPAD_DOWN_RIGHT && host->CurrentStep->NextStep && host->stepAnimationStage == StepAnimationStage::InOverlay){
              host->GotoNextStep();
            }
            else if (keyToSend == KEYPAD_UP_RIGHT && host->CurrentStep->NextStep && host->stepAnimationStage == StepAnimationStage::InOverlay){                  
              host->stepAnimationStage = StepAnimationStage::GoingToStep;
              host->ResetAnimationProgress(250);
            }
            else { // Let the step handle this key if not in overlay
              if (host->stepAnimationStage == StepAnimationStage::MainStep)
                host->CurrentStep->HandleKeyUp(keyToSend);
            }
          }
        }
        // else just discard this button
        keyToSend = Keys::KEYPAD_NONE;
      }
    }
  }
}