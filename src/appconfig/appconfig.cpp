#include "appconfig.h"

RTC_DATA_ATTR appconfig_s appConfig;
RTC_DATA_ATTR bool appconfig_inited;

// 写到nvs中
void appconfig_save()
{
    nvs_handle_t handle;
    // 打开命名空间
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &handle);
    if (err != ESP_OK) return;

    // 写入
    nvs_set_str(handle, "wifiName", appConfig.wifiName);
    nvs_set_str(handle, "wifiPassword", appConfig.wifiPassword);

    // nvs_set_u8(handle, "sleepTimeout", appConfig.sleepTimeout);
   
    // 提交更改
    nvs_commit(handle);
    
    // 关闭句柄
    nvs_close(handle);

    Serial.printf("nvs save ok\n");
}

void appconfig_init()
{
    if (appconfig_inited) {
        return;
    }

    // 初始化 NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS 分区被损坏或版本不匹配时擦除重新初始化
        nvs_flash_erase();
        err = nvs_flash_init();
        Serial.printf("nvs_flash_init error: %d\n", err);
    }

    nvs_handle_t handle;
    err = nvs_open("storage", NVS_READONLY, &handle);
    if (err != ESP_OK) {
        Serial.printf("nvs_open error: %d, %s\n", err, esp_err_to_name(err)); // 4354, ESP_ERR_NVS_NOT_FOUND, 第一次没有这个命名空间, 所以要先写一次
        // 第一次运行时，创建命名空间并写入默认值
        // strlcpy(appConfig.wifiName, "", sizeof(appConfig.wifiName));
        // strlcpy(appConfig.wifiPassword, "", sizeof(appConfig.wifiPassword));
        appconfig_save();
        appconfig_inited = true;
        return;
    }

    size_t len = sizeof(appConfig.wifiName);
    err = nvs_get_str(handle, "wifiName", appConfig.wifiName, &len);
    if (err == ESP_OK) {
        Serial.printf("nvs wifiName: %s\n", appConfig.wifiName);
    }

    err = nvs_get_str(handle, "wifiPassword", appConfig.wifiPassword, &len);
    if (err == ESP_OK) {
        Serial.printf("nvs wifiPassword: %s\n", appConfig.wifiPassword);
    }

    appconfig_inited = true;
    // 读取整数
    // u8 sleepTimeout = 0;
    // err = nvs_get_u8(handle, "sleepTimeout", &sleepTimeout);
    // if (err == ESP_OK) {
    //     Serial.printf("nvs sleepTimeout: %d\n", sleepTimeout);
    // }
}
