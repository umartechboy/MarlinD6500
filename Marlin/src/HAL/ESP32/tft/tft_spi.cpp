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
#include "tft_spi.h"
#include "../../platforms.h"


#include "../../../pins/esp32/pins_D8500.h"


#ifdef ESP32
#if HAS_SPI_TFT
#include "tft_spi.h"
#include "..\..\..\lcd\tft_io\tft_ids.h"

TFT_SPI* TFTObj;

// Initialize the display

void TFT_SPI::Init(){
}
bool TFT_SPI::isBusy(){
  return false;
}
void TFT_SPI::Abort(){

}
void TFT_SPI::setAddrWindowClipped(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
  TFTObj->skipX = 0;
  TFTObj->skipY = 0;
  TFTObj->writtenInX = 0;
  TFTObj->writtenInY = 0;
  TFTObj->addressWindowWidth = x2 - x1 + 1;
  TFTObj->addressWindowHeight = y2 - y1 + 1;

  if (x2 >= TFT_WIDTH) {
    TFTObj->skipX = x2 - TFT_WIDTH + 1;
    x2 = TFT_WIDTH - 1;
  }
  if (y2 >= TFT_HEIGHT) {
    TFTObj->skipY = y2 - TFT_HEIGHT + 1;
    y2 = TFT_HEIGHT - 1;
  }
  if (x1 < TFT_WIDTH && y1 < TFT_WIDTH) {
    setAddrWindow(x1, y1, TFTObj->addressWindowWidth, TFTObj->addressWindowHeight);
  }
  else{    
    TFTObj->skipX = TFTObj->addressWindowWidth;
    TFTObj->skipY = TFTObj->addressWindowHeight;
  }
}
void TFT_SPI::DataTransferBegin(uint16_t DataWidth){
  SERIAL_IMPL.print("DataTransferBegin: ");
  SERIAL_IMPL.println(DataWidth);
  //TFTObj->startWrite();
}
void TFT_SPI::DataTransferEnd(){
  SERIAL_IMPL.println("DataTransferEnd");
  //TFTObj->endWrite();
}
uint32_t TFT_SPI::GetID(){
  return ST7735_144;
}
void TFT_SPI::WriteData(uint16_t Data){
  SERIAL_IMPL.print("WriteData: ");
  SERIAL_IMPL.println(Data);
  //TFTObj->write16(Data);
}
void TFT_SPI::WriteReg(uint16_t Reg){
  SERIAL_IMPL.print("WriteReg: ");
  SERIAL_IMPL.println(Reg);
  //TFTObj->writeCommand16(Reg);
}
void TFT_SPI::WriteSequence(uint16_t *Data, uint16_t Count){
  
  SERIAL_IMPL.print("WriteSequence: ");
  SERIAL_IMPL.println(Count);
  for (int yi = 0; yi < TFTObj->addressWindowHeight - TFTObj->skipY; yi++) {
      TFTObj->writePixels(Data + yi * TFTObj->addressWindowWidth, TFTObj->addressWindowWidth - TFTObj->skipX);
  }
  //TFTObj->writePixels(Data, Count);
  TFTObj->endWrite();
}
void TFT_SPI::WriteMultiple(uint16_t Color, uint32_t Count){
  SERIAL_IMPL.print("WriteMultiple Color: ");
  SERIAL_IMPL.print(Color);
  SERIAL_IMPL.print(", count: ");
  SERIAL_IMPL.println(Count);
  
  for (int yi = 0; yi < TFTObj->addressWindowHeight - TFTObj->skipY; yi++) {
      TFTObj->writeColor(Color, TFTObj->addressWindowWidth - TFTObj->skipX);
  }
  //TFTObj->writeColor(Color, Count);
  TFTObj->endWrite();
}


#endif // HAS_SPI_TFT
#endif // ESP32
