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
#pragma once
#ifndef __PINS_D8500s_H__
#define __PINS_D8500s_H__
/**
 * FYSETC E4 pin assignments
 *
 * 3D printer control board based on the ESP32 microcontroller.
 * Supports 4 stepper drivers, heated bed, single hotend.
 */

#include "env_validate.h"

#if EXTRUDERS > 2 || E_STEPPERS > 2
  #error "D8500 only supports 2 E stepper."
#endif

#define BOARD_INFO_NAME       "D8500s"

// D8500s has no PCF8574 expander pins (200-215) in use.
// (Leave USE_ESP32_PCF8574 undefined.)

// Limit Switches
//
#define X_STOP_PIN                            41   // Done PCB
#define Y_STOP_PIN                            42    // Done PCB
#define Z_STOP_PIN                            48   // Done PCB

//
// Steppers
//

#define X_STEP_PIN                            4    // Done PCB 
#define X_DIR_PIN                             5    // Done PCB
#define X_ENABLE_PIN                          6   // Done PCB

#define Y_STEP_PIN                            43    // Done PCB
#define Y_DIR_PIN                             44     // Done PCB
#define Y_ENABLE_PIN                          9   // Done PCB

// Swapping Z and E because the positions for the motors have changed
#define Z_STEP_PIN                           10      // Done PCB
#define Z_DIR_PIN                            8    // Done PCB
#define Z_ENABLE_PIN                         11    // Done PCB

#define E0_STEP_PIN                            17   // Done PCB
#define E0_DIR_PIN                             16  // Done PCB
#define E0_ENABLE_PIN                          18  // Done PCB

#define E1_STEP_PIN                           13    // Done PCB
#define E1_DIR_PIN                            12   // Done PCB
#define E1_ENABLE_PIN                         14   // Done PCB


//
// Temperature Sensors
//
#define TEMP_0_PIN                            1    // Back to ESP // Done PCB
#define TEMP_1_PIN                            2    // Back to ESP // Done PCB
#define TEMP_BED_PIN                            -1    // Back to ESP // Done PCB
//#define TEMP_CHAMBER_PIN                          35    // Back to ESP // Done PCB

//
// Heaters / Fans
//
#define HEATER_0_PIN                           46 // Done
#define HEATER_1_PIN                           45 // Done
#define FAN_PIN                                21 // Done
#define HEATER_BED_PIN                         -1

//
// MicroSD card
//
#define SD_MOSI_PIN                           35
#define SD_MISO_PIN                           36
#define SD_SCK_PIN                            37
#define SDSS                                  38
#define USES_SHARED_SPI                           // SPI is shared by SD card with TMC SPI drivers


#define TFT_CS         39
#define TFT_DC         40
#define TFT_RST        -1
#define TFT_MOSI         35
#define TFT_SCK         37
#define TFT_BACKLITE   -1


/**
 * Hardware serial pins
 *
 * Override these pins in Configuration.h or Configuration_adv.h if needed.
 *
 * Note: Serial2 can be defined using HARDWARE_SERIAL2_RX and HARDWARE_SERIAL2_TX
 *       but MRR ESPA does not have enough spare pins for such reassignment.
 */
#ifndef HARDWARE_SERIAL1_RX
  #define HARDWARE_SERIAL1_RX                 -1
#endif
#ifndef HARDWARE_SERIAL1_TX
  #define HARDWARE_SERIAL1_TX                 -1
#endif

#endif