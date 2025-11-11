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
#ifndef __PINS_D8500_H__
#define __PINS_D8500_H__
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

#define BOARD_INFO_NAME       "D8500"

// Limit Switches
//
#define X_STOP_PIN                            215   // Done PCB
#define Y_STOP_PIN                            35    // Done PCB
#define Z_STOP_PIN                            213   // Done PCB

//
// Steppers
//

#define X_STEP_PIN                            32    // Done PCB 
#define X_DIR_PIN                             33    // Done PCB
#define X_ENABLE_PIN                          205   // Done PCB

#define Y_STEP_PIN                            26    // Done PCB
#define Y_DIR_PIN                             25    // Done PCB
#define Y_ENABLE_PIN                          206   // Done PCB

// Swapping Z and E because the positions for the motors have changed
#define Z_STEP_PIN                           27      // Done PCB
#define Z_DIR_PIN                            204    // Done PCB
#define Z_ENABLE_PIN                         209    // Done PCB

#define E0_STEP_PIN                            16   // Done PCB
#define E0_DIR_PIN                             202  // Done PCB
#define E0_ENABLE_PIN                          207  // Done PCB

#define E1_STEP_PIN                           2    // Done PCB
#define E1_DIR_PIN                            203   // Done PCB
#define E1_ENABLE_PIN                         208   // Done PCB


//
// Temperature Sensors
//
#define TEMP_0_PIN                            34    // Back to ESP // Done PCB
#define TEMP_1_PIN                            36    // Back to ESP // Done PCB
#define TEMP_BED_PIN                            39    // Back to ESP // Done PCB
//#define TEMP_CHAMBER_PIN                          35    // Back to ESP // Done PCB

#if ENABLED(M3DPrintVueSupport)
#define TEMP_BED_PIN                          -1  // Disable Bed for PrintVue connections
#else
#define TEMP_BED_PIN                          39  // Analog Input
#endif

//
// Heaters / Fans
//
#define HEATER_0_PIN                           210 // Done
#define HEATER_1_PIN                           211 // Done
#define FAN_PIN                                212 // Done
#if ENABLED(M3DPrintVueSupport)
#define HEATER_BED_PIN                         -1 // Disable Bed for PrintVue connections
#else
#define HEATER_BED_PIN                         13
#endif

//
// MicroSD card
//
#define SD_MOSI_PIN                           23
#define SD_MISO_PIN                           19
#define SD_SCK_PIN                            18
#define SDSS                                   5
#define USES_SHARED_SPI                           // SPI is shared by SD card with TMC SPI drivers


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