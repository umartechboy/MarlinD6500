// #include "Keypad.h"
// #include "..\Components\M3DUI.h"
// #include "..\App\MenuApp.h"

// int pinMap [] = {14, 13, 0, 12};
// int baseValues [4];
// #define readPad(i) (min(100, (min(100, (max(0, (baseValues[(((uint32_t)i) % 4)] - touchRead(pinMap[(((uint32_t)i) % 4)]))) * 100 / baseValues[(((uint32_t)i) % 4)]) * 2))) / 100.0F)

// bool first = true;
// Keys touchOnADCKeyPad_getKey(){
//     if (first){
//         for (int i = 0; i < 4; i++){
//           for (int ri = 0; ri < 10; ri ++){
//             baseValues [i] += touchRead(pinMap[i]);
//             delay(1);
//           }
//           baseValues[i] /= 10;
//         }
//       first = false;
//     }
//     // Make a cache
//     float all [4];
//     for (int i = 0; i < 4; i++) {
//       all[i] = readPad(i);
//     }
// #if DebugKeys
//     SERIAL_IMPL.printf("Cache values: %f %f %f %f", all[0], all[1], all[2], all[3]);
// #endif
//     // Check if its the middle button
//     int aboveZero = 0;
//     for (int i = 0; i < 4; i++) {
//       if (all[i] > 0.05){
//           aboveZero++;
//         }
//     }
    
// #if DebugKeys
//     SERIAL_IMPL.printf(", Above zero 1: %d", aboveZero);
// #endif
//     if (aboveZero >= 4){  
// #if DebugKeys    
//       SERIAL_IMPL.println();
// #endif
//       return Keys::KEYPAD_MIDDLE;
//     }
//     // Check if its a diagonal button
//     aboveZero = 0;
//     for (int i = 0; i < 4; i++) {
//       if (all[i] > 0.1){
//           aboveZero++;
//         }
//     }
    
// #if DebugKeys
//     SERIAL_IMPL.printf(", Above zero 2: %d", aboveZero);
// #endif
//     if (aboveZero == 2){
//       // find the first ind
      
//       for (int i = 0; i < 4; i++) {
//         if (all[i] > 0.1){
//           if (i == 0) {
//             if (all[3] > 0.1) { 
//               continue;
//             }
//           }
// #if DebugKeys
//           SERIAL_IMPL.printf(", %d\n", i * 2 + 2 );
// #endif
//           return (Keys)(i * 2 + 2);
//         }
//       }
//     }
//     // Single Button Detection
//     int maxI = 0;
//     for (int i = 0; i < 4; i++) {
//       if (all[i] > all[maxI])
//         maxI = i;
//     }
    
//     if (all[maxI] < 0.5){
      
// #if DebugKeys
//       SERIAL_IMPL.printf(", No key\n");
// #endif
//       return Keys::KEYPAD_NONE;
//     }
//     else{
      
// #if DebugKeys
//       SERIAL_IMPL.printf(", One Key %d\n", maxI * 2 + 1 );
// #endif
//       return (Keys)(maxI * 2 + 1);
//     }
// }

// #define AddKey(k, i) (Keys)(((k) + (i) > 8) ? ((k) + (i) - 8):(((k) + (i) < 1) ? ((k) + (i) + 8):((k) + (i))))

// bool unknwonSwipe = false;
// bool swipeInProcess = false;
// void KeyPad::Loop(MenuHost* host){
//   if (millis() - lastKeyCheck > 20){
//     lastKeyCheck = millis();
//     touchOnADCKeyPad_getKey();
//   }
//   return;
//   if (millis() - lastKeyCheck > (lastKeyDown == Keys::KEYPAD_NONE) ? 30:50){
//     lastKeyCheck = millis();
//     Keys key = touchOnADCKeyPad_getKey();

//     if (key == KEYPAD_NONE && lastKeyDown == KEYPAD_NONE){
//       // Idle
//       lastKeyDown = key;
//       return;
//     }
//     // Its a change. Detect which one    
//     if (key == KEYPAD_NONE){ // lastKey cannot be NONE too
//       // Its a key Up
//       if (swipeInProcess){ 
//         // End of swipe doesn't mean anything. At least not yet
//         swipeInProcess = false;
//         SERIAL_IMPL.println("End of Swipe");
//       }
//       else{
//         // Send Key up
//         SERIAL_IMPL.printf("Key Up: %d\n", lastKeyDown);
//         //host->HandleKeyUp(lastKeyDown);
//       }
//     }
//     else if (lastKeyDown == KEYPAD_NONE) {// Its a Key down.
//         SERIAL_IMPL.printf("Key Down: %d\n", key);
//       // We can't send key down because it might turn into a swipe
//       swipeInProcess = false; // Not needed, but still reset things
//       unknwonSwipe = false;
//     }
//     else {
//       if (lastKeyDown == key) {// None None Key Hold
//         if (swipeInProcess){
//           // Just skip
//         }
//         else { // We can process a hold or press here
//           // skip for now
//         }
//       }
//       else {
//         // Key to Key swipe
//         swipeInProcess = true;
//         // Detect the gesture

//         // Test for Dial rotate
//         if (key == AddKey(lastKeyDown, 1) && !unknwonSwipe){
//           SERIAL_IMPL.printf("Dial Inc: %d > %d\n", lastKeyDown, key);
//           host->HandleDialIncrement();
//           if (millis() - lastDialRotateSentAt < 30) 
//               host->HandleDialIncrement(); // accelerate
//           lastDialRotateSentAt = millis(); 
//         }
//         else if (key == AddKey(lastKeyDown, -1) && !unknwonSwipe){
//           SERIAL_IMPL.printf("Dial Dec: %d > %d\n", lastKeyDown, key);
//           host->HandleDialDecrement();
//           if (millis() - lastDialRotateSentAt < 30) 
//               host->HandleDialDecrement(); // accelerate
//           lastDialRotateSentAt = millis(); 
//         }
//         else if (key == KEYPAD_MIDDLE && lastKeyDown == KEYPAD_DOWN || key == KEYPAD_UP && lastKeyDown == KEYPAD_MIDDLE){
//           SERIAL_IMPL.printf("Swipe Up: %d > %d\n", lastKeyDown, key);
//           host->HandleDialIncrement();
//         }
//         else if (key == KEYPAD_MIDDLE && lastKeyDown == KEYPAD_UP || key == KEYPAD_DOWN && lastKeyDown == KEYPAD_MIDDLE){
//           SERIAL_IMPL.printf("Swipe Down: %d > %d\n", lastKeyDown, key);
//           host->HandleDialDecrement();
//         }
//         else { // Swipe but not a dial rotate
//           SERIAL_IMPL.printf("Unknown Swipe: %d > %d\n", lastKeyDown, key);
//           unknwonSwipe = true;
//         }
//       }
//     }
//     lastKeyDown = key;
//   }
// }