#include <Watchy.h>
#include "settings.h"

Watchy watchy(settings);

void setup()
{
    Serial.begin(115200);
    Serial.println("start");
    watchy.init();
}

void loop() {}