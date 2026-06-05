@echo off
setlocal enabledelayedexpansion

echo ====================================
echo   D8500s Firmware Build & Package
echo ====================================
echo.

:: Get version/date for naming
set TODAY=%DATE:~-4%-%DATE:~4,2%-%DATE:~7,2%
echo Build Date: %TODAY%
echo.

:: Step 1: Build firmware
echo [1/5] Building firmware...
pio run -e D8500s
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Firmware build failed!
    pause
    exit /b 1
)
echo.

:: Step 2: Build filesystem
echo [2/5] Building filesystem (SPIFFS)...
pio run --environment D8500s --target buildfs
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Filesystem build failed!
    pause
    exit /b 1
)
echo.

:: Step 3: Copy binaries to BuildB3
echo [3/4] Copying binaries to BuildB3...
if not exist "BuildB3" mkdir BuildB3
copy /Y ".pio\build\D8500s\bootloader.bin" "BuildB3\"
copy /Y ".pio\build\D8500s\partitions.bin" "BuildB3\"
copy /Y ".pio\build\D8500s\firmware.bin" "BuildB3\"
copy /Y ".pio\build\D8500s\spiffs.bin" "BuildB3\"
copy /Y ".pio\build\D8500s\firmware.elf" "BuildB3\"
copy /Y ".pio\build\D8500s\firmware.map" "BuildB3\"

:: Copy esptool and updater scripts to BuildB3
copy /Y "%USERPROFILE%\.platformio\penv\Scripts\esptool.exe" "BuildB3\"
copy /Y "D8500s_Firmware_Updater\Update_Firmware.bat" "BuildB3\"
copy /Y "D8500s_Firmware_Updater\Update_Firmware_Mac.sh" "BuildB3\"
copy /Y "D8500s_Firmware_Updater\README.txt" "BuildB3\"
copy /Y "D8500s_Firmware_Updater\README_Mac.txt" "BuildB3\"
echo All files copied to BuildB3.
echo.

:: Step 4: Create release ZIP
echo [4/4] Creating release package...
set ZIPNAME=D8500s_FW_%TODAY%.zip

:: Remove old zip if exists
if exist "%ZIPNAME%" del "%ZIPNAME%"

:: Create zip using PowerShell
powershell -Command "Compress-Archive -Path 'BuildB3\bootloader.bin','BuildB3\partitions.bin','BuildB3\firmware.bin','BuildB3\spiffs.bin','BuildB3\esptool.exe','BuildB3\Update_Firmware.bat','BuildB3\Update_Firmware_Mac.sh','BuildB3\README.txt','BuildB3\README_Mac.txt' -DestinationPath '%ZIPNAME%' -Force"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ====================================
    echo   Build Complete!
    echo ====================================
    echo.
    echo Release package: %ZIPNAME%
    echo Binaries location: BuildB3\
    echo Debug files: firmware.elf, firmware.map
    echo.
    echo Summary:
    dir /B BuildB3\*.bin
    echo.
    echo ZIP contents ready for distribution.
) else (
    echo.
    echo ERROR: Failed to create ZIP package!
    echo Build binaries are still available in BuildB3\
)

echo.
pause
