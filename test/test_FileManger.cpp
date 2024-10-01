#include <unity.h>
#include <Arduino.h>
#include <SPI.h>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();

    UNITY_END(); // stop unit testing
}

void loop()
{
}