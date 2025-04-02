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

#include "../../platforms.h"

#ifdef ESP32

#include "../../../inc/MarlinConfig.h"

#if HAS_SPI_TFT
#include "tft_spi.h"

void TFT_SPI::Init(){
  
}
bool TFT_SPI::isBusy(){
  return false;
}
void TFT_SPI::Abort(){

}
void TFT_SPI::DataTransferBegin(uint16_t DataWidth){

}
uint32_t TFT_SPI::GetID(){
  return 0;
}
void TFT_SPI::WriteData(uint16_t Data){

}
void TFT_SPI::DataTransferEnd(){

}
void TFT_SPI::WriteReg(uint16_t Reg){

}
void TFT_SPI::WriteSequence(uint16_t *Data, uint16_t Count){

}
void TFT_SPI::WriteMultiple(uint16_t Color, uint32_t Count){

}


#endif // HAS_SPI_TFT
#endif // ESP32
