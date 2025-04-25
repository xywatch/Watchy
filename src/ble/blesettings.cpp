#include "blesettings.h"
#include "appconfig/appconfig.h"

#define BLE_SETTING_SERVICE_UUID "0000ffe0-0000-1000-8000-00805f9b34fb"
#define BLE_SETTING_CHARACTERISTIC_UUID "0000ffe1-0000-1000-8000-00805f9b34fb"

static BLEServer *pServer = nullptr;
static BLEService *pService = nullptr;
static BLECharacteristic *pCharacteristic = nullptr;

static bool deviceConnected = false;
static bool oldDeviceConnected = false;

static byte _updateStatus = -1;

// 将当前配置转换为JSON字符串
static String getSettingsJson()
{
    JSONVar doc;
    doc["wifiName"] = appConfig.wifiName;
    doc["wifiPassword"] = appConfig.wifiPassword;
    return JSON.stringify(doc);
}

// 从JSON字符串更新配置
static bool updateSettingsFromJson(const char *jsonString)
{
    JSONVar doc = JSON.parse(jsonString);

    if (JSON.typeof(doc) == "undefined")
    {
        Serial.println("JSON parsing failed!");
        _updateStatus = 5;
        return false;
    }

    // 更新配置
    if (doc.hasOwnProperty("wifiName"))
    {
        const char *fname = (const char *)doc["wifiName"];
        strlcpy(appConfig.wifiName, fname, sizeof(appConfig.wifiName));
    }
    if (doc.hasOwnProperty("wifiPassword"))
    {
        const char *lname = (const char *)doc["wifiPassword"];
        strlcpy(appConfig.wifiPassword, lname, sizeof(appConfig.wifiPassword));
    }

    _updateStatus = 2;

    appconfig_save();

    return true;
}

class BLESettingsServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
        Serial.println("Device connected");
        _updateStatus = 0;
    }

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
        Serial.println("Device disconnected");

        _updateStatus = 4;
    }
};

class BLESettingCallback : public BLECharacteristicCallbacks
{
private:
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();
        Serial.printf("Received data: %s\n", rxValue.c_str());

        // 更新配置
        if (updateSettingsFromJson(rxValue.c_str()))
        {
            Serial.println("Settings updated successfully");
            // 发送更新后的配置回客户端确认
            String jsonString = getSettingsJson();
            pCharacteristic->setValue(jsonString.c_str());
            pCharacteristic->notify();
        }
    }

    void onRead(BLECharacteristic *pCharacteristic)
    {
        // 读取请求时返回当前配置
        String jsonString = getSettingsJson();
        pCharacteristic->setValue(jsonString.c_str());
    }
};
//
// Constructor
BLESettings::BLESettings(void) {}
// Destructor
BLESettings::~BLESettings(void) {}

//
// begin
bool BLESettings::begin(const char *localName = "Watchy Settings")
{
    appconfig_init();

    // Create the BLE Device
    BLEDevice::init(localName);

    // 设置发射功率为最大
    esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);

    // 创建BLE服务器
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new BLESettingsServerCallbacks());

    // 创建OTA服务
    pService = pServer->createService(BLE_SETTING_SERVICE_UUID);

    // 创建OTA特征
    pCharacteristic = pService->createCharacteristic(
        BLE_SETTING_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setCallbacks(new BLESettingCallback());

    // 启动服务
    pService->start();

    // 开始广播
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SETTING_SERVICE_UUID);
    pAdvertising->setScanResponse(true);

    // 设置更快的广播间隔
    pAdvertising->setMinInterval(0x20); // 最小间隔
    pAdvertising->setMaxInterval(0x40); // 最大间隔

    BLEDevice::startAdvertising();

    Serial.println("BLE Settings Ready!");

    return true;
}

byte BLESettings::updateStatus() { return _updateStatus; }
