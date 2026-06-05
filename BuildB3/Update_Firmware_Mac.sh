#!/bin/bash

echo "===================================="
echo "  D8500s Firmware Updater v1.0"
echo "===================================="
echo ""
echo "This will update your D8500s 3D printer firmware."
echo ""
echo "IMPORTANT: Before proceeding, make sure:"
echo "  1. Your printer is connected via USB"
echo "  2. No other software is using the printer's USB port"
echo ""
echo "===================================="
echo ""

# Check if esptool is installed
if ! command -v esptool.py &> /dev/null
then
    echo "esptool.py not found. Installing automatically..."
    echo ""

    # Try pip3 first
    if command -v pip3 &> /dev/null; then
        echo "Installing esptool using pip3..."
        pip3 install esptool --user
    elif command -v python3 &> /dev/null; then
        echo "Installing esptool using python3..."
        python3 -m pip install esptool --user
    else
        echo "ERROR: Python 3 is not installed!"
        echo "Please install Python 3 from https://www.python.org"
        echo ""
        read -p "Press Enter to exit..."
        exit 1
    fi

    # Check if installation succeeded
    if ! command -v esptool.py &> /dev/null; then
        echo ""
        echo "ERROR: esptool installation failed!"
        echo "Please try manually installing with:"
        echo "  pip3 install esptool"
        echo ""
        read -p "Press Enter to exit..."
        exit 1
    fi

    echo ""
    echo "esptool installed successfully!"
    echo ""
fi

# Check esptool version
ESPTOOL_VERSION=$(esptool.py version 2>&1 | head -n 1)
echo "Found: $ESPTOOL_VERSION"
echo ""

echo "Starting firmware upload..."
echo "This may take 1-2 minutes. Please do not disconnect the printer."
echo ""

# Run esptool
esptool.py --chip esp32s3 --baud 460800 write_flash 0x0 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin 0x1E0000 firmware.bin 0x3B0000 spiffs.bin

# Check exit status
if [ $? -eq 0 ]; then
    echo ""
    echo "===================================="
    echo "  Firmware Update Successful!"
    echo "===================================="
    echo ""
    echo "Your D8500s has been updated successfully."
    echo "Please restart your printer to apply the changes."
else
    echo ""
    echo "===================================="
    echo "  Firmware Update Failed!"
    echo "===================================="
    echo ""
    echo "Please check:"
    echo "  - USB cable is connected properly"
    echo "  - Printer is powered on"
    echo "  - You have the CORRECT printer model (D8500s with ESP32-S3)"
    echo ""
    echo "IMPORTANT:"
    echo "  This updater is ONLY for D8500s V3.1 printers with ESP32-S3 chip."
    echo "  If you have an older D8500 model with ESP32 (non-S3),"
    echo "  you need a different firmware updater."
    echo ""
    echo "If problems persist, contact support at info@markhor3d.com with error details above."
fi

echo ""
read -p "Press Enter to exit..."
