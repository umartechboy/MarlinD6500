================================================================================
                D8500s 3D Printer Firmware Updater - macOS
================================================================================

VERSION: 1.0
DATE: June 2, 2026

================================================================================
CONTENTS
================================================================================

This package contains everything needed to update your D8500s printer:

1. Update_Firmware_Mac.sh  - Main firmware update script for macOS
2. bootloader.bin          - ESP32-S3 bootloader
3. partitions.bin          - Partition table
4. firmware.bin            - D8500s firmware
5. spiffs.bin              - File system (settings, configs, web files)
6. README_Mac.txt          - This file

================================================================================
SYSTEM REQUIREMENTS
================================================================================

- macOS 10.12 (Sierra) or later
- Python 3 (pre-installed on macOS 10.15+)
- USB connection to D8500s printer
- esptool.py (installation instructions below)

================================================================================
INSTALLATION INSTRUCTIONS
================================================================================

STEP 1: Install esptool (One-time setup)
-----------------------------------------
Before running the updater for the first time, you need to install esptool:

1. Open Terminal (Applications > Utilities > Terminal)
2. Run ONE of these commands:

   pip3 install esptool

   or if that doesn't work:

   python3 -m pip install esptool

3. Wait for installation to complete
4. You only need to do this once - esptool will remain installed

STEP 2: Prepare Your Printer
-----------------------------
1. Connect your D8500s printer to your Mac via USB cable
2. Power on the printer
3. Wait for macOS to recognize the device

STEP 3: Make Script Executable (One-time)
------------------------------------------
1. Open Terminal
2. Navigate to the updater folder:
   cd /path/to/D8500s_Firmware_Updater
   (Tip: Type "cd " then drag the folder into Terminal)
3. Make the script executable:
   chmod +x Update_Firmware_Mac.sh

STEP 4: Run the Updater
-----------------------
Option A - Double-click method:
1. Right-click "Update_Firmware_Mac.sh"
2. Select "Open With > Terminal"
3. Follow the on-screen instructions

Option B - Terminal method:
1. Open Terminal
2. Navigate to the updater folder (if not already there)
3. Run: ./Update_Firmware_Mac.sh
4. Wait for the update to complete (1-2 minutes)
5. DO NOT disconnect the printer during the update!

STEP 5: Verify Update
---------------------
1. When "Firmware Update Successful!" appears, restart your printer
2. Check the firmware version on your printer's display
3. Verify all functions are working correctly

================================================================================
TROUBLESHOOTING
================================================================================

Problem: "esptool.py: command not found"
Solution:
- esptool is not installed yet
- Follow STEP 1 above to install esptool
- Make sure to use pip3 or python3 -m pip install esptool
- After installation, try running the update script again

Problem: "Permission denied" when running the script
Solution:
- The script is not executable yet
- Follow STEP 3 to make it executable with: chmod +x Update_Firmware_Mac.sh
- Then try running it again

Problem: "Failed to connect to ESP32" or "No serial port found"
Solution:
- Check USB cable connection
- Try a different USB cable or USB port on your Mac
- Restart the printer and try again
- Check if the device appears in System Information:
  Apple menu > About This Mac > System Report > USB
  Look for "USB Serial Device" or similar

Problem: "Access denied" or "Port is busy"
Solution:
- Close any software that might be using the USB port (Cura, Repetier, etc.)
- Unplug and replug the USB cable
- Restart your Mac if the problem persists

Problem: "A fatal error occurred" or "Unexpected chip ID"
Solution:
- This updater is ONLY for D8500s printers with ESP32-S3 chip
- If you have an older D8500 model with ESP32 (not S3), this updater will NOT work
- Check your printer model - older models require a different firmware updater
- Contact support to confirm your printer model and get the correct updater

Problem: macOS Security warning about unidentified developer
Solution:
- Right-click the script and select "Open" (instead of double-clicking)
- Click "Open" in the security dialog
- Or go to System Preferences > Security & Privacy and click "Open Anyway"

================================================================================
FINDING YOUR PRINTER'S USB PORT (Optional)
================================================================================

If you need to troubleshoot connection issues:

1. Open Terminal
2. Run: ls /dev/cu.*
3. Look for entries like /dev/cu.usbserial-XXXX or /dev/cu.SLAB_USBtoUART
4. This shows your printer's port (esptool auto-detects this)

================================================================================
IMPORTANT NOTES
================================================================================

- ⚠️ CRITICAL: This updater is ONLY for D8500s V3.1 printers with ESP32-S3 chip
- ⚠️ Do NOT use this on older D8500 models with ESP32 (non-S3) chip
- If you're unsure about your printer model, contact support BEFORE updating
- Do not interrupt the update process
- The update takes approximately 1-2 minutes
- All printer settings and calibration will be preserved
- If update fails, you can safely try again
- Keep this package for future firmware updates
- You only need to install esptool once - it stays installed for future updates

================================================================================
SUPPORT
================================================================================

If you encounter any issues not covered in this guide, please contact
technical support at info@markhor3d.com with the following information:

- Printer model (D8500s V3.1)
- Current firmware version
- Error message (if any)
- macOS version
- Terminal output/error messages

================================================================================
TECHNICAL DETAILS
================================================================================

Chip: ESP32-S3
Baud Rate: 460800
Flash Addresses:
  - Bootloader: 0x0
  - Partitions: 0x8000
  - Firmware (app0):   0x10000
  - Firmware (app1):   0x1E0000
  - SPIFFS:            0x3B0000

Tool: esptool.py (Python-based)
Minimum esptool version: 4.0

================================================================================
