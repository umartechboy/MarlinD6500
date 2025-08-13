#ifndef __HTTP_OTA__
#define __HTTP_OTA__

#define FIRMWARE_PORT 80   // HTTP
#define HOST_PROTOCOL "http"
#define FIRMWARE_HOST "m3dcore.com"
#define FIRMWARE_JSON "d8500.json"
#define FIRMWARE_BIN "d8500.bin"
#define FILES_PATH "/ota/"

// "http://m3dcore.com/d8500.json"
#define FIRMWARE_INFO_URL HOST_PROTOCOL "://" FIRMWARE_HOST FILES_PATH FIRMWARE_JSON

// "/ota/d8500.bin"
#define FIRMWARE_PATH FILES_PATH FIRMWARE_BIN
#define FRIMWARE_VERSION 2

extern void TryOTAUpdate();

#endif