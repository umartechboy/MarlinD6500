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

/**
 * FYSETC E4 pin assignments
 *
 * 3D printer control board based on the ESP32 microcontroller.
 * Supports 4 stepper drivers, heated bed, single hotend.
 */

#include "env_validate.h"

#if EXTRUDERS > 1 || E_STEPPERS > 1
  #error "D6500 E4 only supports 1 E stepper."
#elif HAS_MULTI_HOTEND
  #error "D6500 E4 only supports 1 hotend / E stepper."
#endif

#define BOARD_INFO_NAME       "D6500"
//#define D6500_RouterPCB   // Make this 1 

// Limit Switches
//
#define X_STOP_PIN                            34 // Done
#define Y_STOP_PIN                            35 // Done
#define Z_STOP_PIN                            15 

//
// Steppers
//

#define X_STEP_PIN                            32 // Done
#define X_DIR_PIN                             33 // Done
#define X_ENABLE_PIN                          25

#define Y_STEP_PIN                            26 // Done
#define Y_DIR_PIN                             25 // Done
#define Y_ENABLE_PIN                          14

#if ENABLED(D6500_RouterPCB)
#define Z_STEP_PIN                            21
#define Z_DIR_PIN                             12
#define Z_ENABLE_PIN                          4
#else
#define Z_STEP_PIN                            27 // Done
#define Z_DIR_PIN                             12
#define Z_ENABLE_PIN                          22
#endif

#if ENABLED(D6500_RouterPCB)
#define E0_STEP_PIN                           22
#define E0_DIR_PIN                            17
#define E0_ENABLE_PIN                         21
#else
#define E0_STEP_PIN                           16 // Done
#define E0_DIR_PIN                            4
#define E0_ENABLE_PIN                         17


#define E1_STEP_PIN                           2 // Done
#define E1_DIR_PIN                            4
#define E1_ENABLE_PIN                         17
#endif

//
// Temperature Sensors
//
#define TEMP_0_PIN                            36  // Analog Input
#if ENABLED(M3DPrintVueSupport)
#define TEMP_BED_PIN                          -1  // Disable Bed for PrintVue connections
#else
#define TEMP_BED_PIN                          39  // Analog Input
#endif

//
// Heaters / Fans
//
#define HEATER_0_PIN                           2
#define FAN_PIN                                -1 // Can't manually control the fan
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
