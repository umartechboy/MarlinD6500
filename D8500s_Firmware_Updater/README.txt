================================================================================
                     D8500s 3D Printer Firmware Updater
================================================================================

VERSION: 1.0
DATE: June 2, 2026

================================================================================
CONTENTS
================================================================================

This package contains everything needed to update your D8500s printer:

1. Update_Firmware.bat    - Main firmware update script
2. esptool.exe            - Firmware upload utility
3. bootloader.bin         - ESP32-S3 bootloader
4. partitions.bin         - Partition table
5. firmware.bin           - D8500s firmware
6. README.txt             - This file

================================================================================
SYSTEM REQUIREMENTS
================================================================================

- Windows 7 or later (64-bit)
- USB connection to D8500s printer
- USB Serial drivers (usually installed automatically by Windows 10/11)

================================================================================
INSTALLATION INSTRUCTIONS
================================================================================

STEP 1: Prepare Your Printer
-----------------------------
1. Connect your D8500s printer to your computer via USB cable
2. Power on the printer
3. Wait for Windows to recognize the device

STEP 2: Run the Updater
-----------------------
1. Double-click "Update_Firmware.bat"
2. The updater will automatically detect your printer
3. Wait for the update to complete (1-2 minutes)
4. DO NOT disconnect the printer during the update!

STEP 3: Verify Update
---------------------
1. When "Firmware Update Successful!" appears, restart your printer
2. Check the firmware version on your printer's display
3. Verify all functions are working correctly

================================================================================
TROUBLESHOOTING
================================================================================

Problem: "Failed to connect to ESP32" or "No serial port found"
Solution:
- Check USB cable connection
- Try a different USB cable or USB port on your computer
- Restart the printer and try again
- Ensure USB drivers are properly installed

Problem: "A fatal error occurred" or "Unexpected chip ID"
Solution:
- This updater is ONLY for D8500s printers with ESP32-S3 chip
- If you have an older D8500 model with ESP32 (not S3), this updater will NOT work
- Check your printer model - older models require a different firmware updater
- Contact support to confirm your printer model and get the correct updater

Problem: "Access denied" or "Port is busy"
Solution:
- Close any software that might be using the COM port (Cura, Repetier, etc.)
- Unplug and replug the USB cable
- Try running as Administrator (right-click > Run as administrator)

Problem: "esptool.exe is not recognized"
Solution:
- Make sure all files are in the same folder
- Do not move files to different locations
- Extract all files from the ZIP before running

Problem: Driver not installed
Solution:
- Windows 10/11 usually installs USB Serial drivers automatically
- If not detected, try a different USB cable or port
- Restart your computer and reconnect the printer
- The device should appear as "USB Serial Device" in Device Manager
- Contact support if the device is still not recognized

================================================================================
IMPORTANT NOTES
================================================================================

- ⚠️ CRITICAL: This updater is ONLY for D8500s printers with ESP32-S3 chip
- ⚠️ Do NOT use this on older D8500 models with ESP32 (non-S3) chip
- If you're unsure about your printer model, contact support BEFORE updating
- Do not interrupt the update process
- The update takes approximately 1-2 minutes
- All printer settings and calibration will be preserved
- If update fails, you can safely try again
- Keep this package for future firmware updates

================================================================================
SUPPORT
================================================================================

If you encounter any issues not covered in this guide, please contact
technical support with the following information:

- Printer model (D8500s)
- Current firmware version
- Error message (if any)
- COM port used
- Windows version

================================================================================
TECHNICAL DETAILS
================================================================================

Chip: ESP32-S3
Baud Rate: 460800
Flash Addresses:
  - Bootloader: 0x0
  - Partitions: 0x8000
  - Firmware:   0x10000

================================================================================
