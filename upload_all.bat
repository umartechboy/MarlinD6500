@echo off
setlocal

:: Check if a COM port was passed as the first argument (%1)
if "%~1"=="" (
    echo [INFO] No COM port provided. Letting esptool auto-detect...
    set PORT_ARG=
) else (
    echo [INFO] Using specified COM port: %~1
    set PORT_ARG=--port %~1
)

:: Run the esptool command with the dynamic port argument
esptool --chip esp32s3 %PORT_ARG% --baud 460800 write_flash 0x0 BuildB3/bootloader.bin 0x8000 BuildB3/partitions.bin 0x10000 BuildB3/firmware.bin 0x1E0000 BuildB3/firmware.bin 0x3B0000 BuildB3/spiffs.bin
