#include "unity_config.h"
#include <Arduino.h>

void unityOutputStart()
{
    Serial.begin(115200);
}

void unityOutputChar(char c)
{
    Serial.print(c);
}

void unityOutputFlush()
{
    Serial.flush();
}

void unityOutputComplete()
{
    Serial.end();
}
