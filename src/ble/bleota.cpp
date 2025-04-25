#include "bleota.h"

#define SERVICE_UUID_ESPOTA    "cd77498e-1ac8-48b6-aba8-4161c7342fce"
#define CHARACTERISTIC_UUID_ID "cd77498f-1ac8-48b6-aba8-4161c7342fce"

#define SERVICE_UUID_OTA               "86b12865-4b70-4893-8ce6-9864fc00374d"
#define CHARACTERISTIC_UUID_FW         "86b12866-4b70-4893-8ce6-9864fc00374d"
#define CHARACTERISTIC_UUID_HW_VERSION "86b12867-4b70-4893-8ce6-9864fc00374d"
#define CHARACTERISTIC_UUID_WATCHFACE_NAME                                     \
  "86b12868-4b70-4893-8ce6-9864fc00374d"

#define FULL_PACKET         512
#define CHARPOS_UPDATE_FLAG 5

static esp_ota_handle_t otaHandler = 0;

int status        = -1;
bool updateFlag   = false;

// OTA 更新相关变量
int content_length = 0;
int content_index = 0;
bool isOTARunning = false;

class BLECustomServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    status = STATUS_CONNECTED;
  }

  void onDisconnect(BLEServer *pServer) {
    status = STATUS_DISCONNECTED;
  }
};

class otaCallback : public BLECharacteristicCallbacks
{
private:
    esp_ota_handle_t update_handle = 0;
    const esp_partition_t *update_partition = NULL;

    /*
    Running partition: app0
    OTA Partition size: 1966080 bytes
    Firmware size: 1775952 bytes
    */
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();

        if (!isOTARunning) // 开始OTA
        {
            content_length = *((int *)rxValue.c_str());
            Serial.printf("OTA Start, size: %d\n", content_length);

            const esp_partition_t *running = esp_ota_get_running_partition();
            Serial.printf("Running partition: %s\n", running->label);

            // 获取更新分区
            update_partition = esp_ota_get_next_update_partition(NULL);
            if (update_partition == NULL)
            {
                Serial.println("No OTA partition found");
                status = STATUS_FAILED;
                return;
            }

            // 3. 确保不是同一个分区
            if (update_partition == running)
            {
                Serial.println("Error: Update partition is same as running partition");
                status = STATUS_FAILED;
                return;
            }

            // 开始OTA，获取handle
            // OTA_WITH_SEQUENTIAL_WRITES
            // OTA_SIZE_UNKNOWN
            esp_err_t err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle);
            if (err != ESP_OK)
            {
                Serial.printf("esp_ota_begin failed %d\n", err);

                // 打印具体错误
                switch (err)
                {
                case ESP_ERR_NO_MEM:
                    Serial.println("Not enough memory");
                    break;
                case ESP_ERR_INVALID_ARG:
                    Serial.println("Invalid argument");
                    break;
                case ESP_ERR_OTA_PARTITION_CONFLICT:
                    Serial.println("Partition conflict");
                    break;
                default:
                    Serial.println("Unknown error");
                }
                status = STATUS_FAILED;
                return;
            }

            Serial.printf("OTA Partition: %s\n", update_partition->label);
            Serial.printf("OTA Partition size: %d bytes\n", update_partition->size);
            Serial.printf("Firmware size: %d bytes\n", content_length);

            isOTARunning = true;
            status = STATUS_UPDATING;
            content_index = 0;

            // 处理第一块数据
            if (rxValue.length() > 4)
            {
                err = esp_ota_write(update_handle, rxValue.c_str() + 4, rxValue.length() - 4);
                if (err != ESP_OK)
                {
                    Serial.println("esp_ota_write failed!");
                    esp_ota_abort(update_handle);
                    isOTARunning = false;
                    status = STATUS_FAILED;
                    return;
                }
                content_index += rxValue.length() - 4;
            }
        }
        else
        { // OTA进行中
            if (content_index < content_length)
            {
                // 使用已存储的 update_handle 继续写入
                esp_err_t err = esp_ota_write(update_handle, rxValue.c_str(), rxValue.length());
                if (err != ESP_OK)
                {
                    Serial.println("esp_ota_write failed!");
                    esp_ota_abort(update_handle);
                    isOTARunning = false;
                    status = STATUS_FAILED;
                    return;
                }
                content_index += rxValue.length();

                Serial.printf("OTA Progress: %0.2f%%\n", ((float)content_index * 100) / content_length);

                // 完成
                if (content_index >= content_length)
                {
                    err = esp_ota_end(update_handle);
                    if (err != ESP_OK)
                    {
                        Serial.println("esp_ota_end failed!");
                        isOTARunning = false;
                        status = STATUS_FAILED;
                        return;
                    }

                    err = esp_ota_set_boot_partition(update_partition);
                    if (err != ESP_OK)
                    {
                        Serial.println("esp_ota_set_boot_partition failed!");
                        isOTARunning = false;
                        status = STATUS_FAILED;
                        return;
                    }
                    // success
                    status = STATUS_SUCCESS;
                }
            }
        }
    }
};
//
// Constructor
BLE::BLE(void) {}

//
// Destructor
BLE::~BLE(void) {}

//
// begin
bool BLE::begin(const char *localName = "Watchy BLE OTA") {
  // Create the BLE Device
  BLEDevice::init(localName);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLECustomServerCallbacks());

  // Create the BLE Service
  pESPOTAService = pServer->createService(SERVICE_UUID_ESPOTA);
  pService       = pServer->createService(SERVICE_UUID_OTA);

  // Create a BLE Characteristic
  pESPOTAIdCharacteristic = pESPOTAService->createCharacteristic(
      CHARACTERISTIC_UUID_ID, BLECharacteristic::PROPERTY_READ);

  pVersionCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_HW_VERSION, BLECharacteristic::PROPERTY_READ);

  pWatchFaceNameCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_WATCHFACE_NAME, BLECharacteristic::PROPERTY_READ);

  pOtaCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_FW,
      BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE);

  pOtaCharacteristic->addDescriptor(new BLE2902());
  pOtaCharacteristic->setCallbacks(new otaCallback());

  // Start the service(s)
  pESPOTAService->start();
  pService->start();

  // Start advertising
  pServer->getAdvertising()->addServiceUUID(SERVICE_UUID_ESPOTA);
  pServer->getAdvertising()->start();

  uint8_t hardwareVersion[5] = {HARDWARE_VERSION_MAJOR, HARDWARE_VERSION_MINOR,
                                SOFTWARE_VERSION_MAJOR, SOFTWARE_VERSION_MINOR,
                                SOFTWARE_VERSION_PATCH};
  pVersionCharacteristic->setValue((uint8_t *)hardwareVersion, 5);
  pWatchFaceNameCharacteristic->setValue("Watchy 7 Segment");

  return true;
}

int BLE::updateStatus() { return status; }

int BLE::howManyBytes() { return content_index; }
