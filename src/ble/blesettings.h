#ifndef _BLE_SETTINGS_H_
#define _BLE_SETTINGS_H_

#include "Arduino.h"

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Arduino_JSON.h>

#include "config.h"

class BLESettings;

class BLESettings {
public:
  BLESettings(void);
  ~BLESettings(void);

  bool begin(const char *localName);
  byte updateStatus();
};

#endif
