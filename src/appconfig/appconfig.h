#ifndef APPCONFIG_H_
#define APPCONFIG_H_

#include "nvs_flash.h"
#include "Arduino.h"

typedef struct
{
	char wifiName[32];
	char wifiPassword[32];
} appconfig_s;

extern appconfig_s appConfig;

void appconfig_init(void);
void appconfig_save(void);

#endif /* APPCONFIG_H_ */
