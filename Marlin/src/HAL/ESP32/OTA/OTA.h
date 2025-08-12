#ifndef __HTTP_OTA__
#define __HTTP_OTA__
#define FIRMWARE_INFO_URL "https://updates.markhor3d.com/d8500.json"
#define FIRMWARE_HOST "https://updates.markhor3d.com/"
#define FIRMWARE_BIN "d8500.bin"
#define FRIMWARE_VERSION 1
extern void TryOTAUpdate(void*);

#endif