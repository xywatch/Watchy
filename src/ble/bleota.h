#ifndef _BLE_OTA_H_
#define _BLE_OTA_H_

#include "Arduino.h"

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "esp_ota_ops.h"

#include "config.h"

#define STATUS_CONNECTED    0
#define STATUS_DISCONNECTED 4
#define STATUS_UPDATING     1
#define STATUS_SUCCESS      2
#define STATUS_FAILED       5

class BLE;

class BLE {
public:
  BLE(void);
  ~BLE(void);

  bool begin(const char *localName);
  int updateStatus();
  int howManyBytes();

private:
  String local_name;

  BLEServer *pServer = NULL;

  BLEService *pESPOTAService                 = NULL;
  BLECharacteristic *pESPOTAIdCharacteristic = NULL;

  BLEService *pService                            = NULL;
  BLECharacteristic *pVersionCharacteristic       = NULL;
  BLECharacteristic *pOtaCharacteristic           = NULL;
  BLECharacteristic *pWatchFaceNameCharacteristic = NULL;
};

#endif
