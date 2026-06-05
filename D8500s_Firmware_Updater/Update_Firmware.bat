@echo off
echo ====================================
echo   D8500s Firmware Updater v1.0
echo ====================================
echo.
echo This will update your D8500s 3D printer firmware.
echo.
echo IMPORTANT: Before proceeding, make sure:
echo   1. Your printer is connected via USB
echo   2. USB drivers are installed
echo   3. No other software is using the printer's COM port
echo.
echo ====================================
echo.
echo Opening Device Manager...
echo Please verify your printer appears under "Ports (COM ^& LPT)"
echo as "USB Serial Device" or "USB-SERIAL" (ESP32-S3).
echo.
echo Close Device Manager window when ready to continue.
echo.

start /wait devmgmt.msc

echo.
echo Starting firmware upload...
echo This may take 1-2 minutes. Please do not disconnect the printer.
echo.

esptool.exe --chip esp32s3 --baud 460800 write_flash 0x0 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin 0x1E0000 firmware.bin

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ====================================
    echo   Firmware Update Successful!
    echo ====================================
    echo.
    echo Your D8500s has been updated successfully.
    echo Please restart your printer to apply the changes.
) else (
    echo.
    echo ====================================
    echo   Firmware Update Failed!
    echo ====================================
    echo.
    echo Please check:
    echo   - USB cable is connected properly
    echo   - Printer is powered on
    echo   - USB drivers are installed
    echo   - You have the CORRECT printer model (D8500s with ESP32-S3)
    echo.
    echo IMPORTANT:
    echo   This updater is ONLY for D8500s V3.1 printers with ESP32-S3 chip.
    echo   If you have an older D8500 model with ESP32 (non-S3),
    echo   you need a different firmware updater.
    echo.
    echo If problems persist, contact support at info@markhor3d.com with error details above.
)

echo.
pause
