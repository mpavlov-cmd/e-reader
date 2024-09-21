#include "PowerStatus.h"


PowerStatus::PowerStatus(
    uint8_t powerSensePin,
    uint8_t chargeStatusPin,
    uint8_t batterySensePin,
    uint16_t minVoltage,
    uint16_t maxVoltage) : 
        battery(minVoltage, maxVoltage, batterySensePin, ADC_RESOLUTION),
        powerSensePin(powerSensePin), 
        chargeStatusPin(chargeStatusPin)
{
    pinMode(powerSensePin, INPUT);
    pinMode(chargeStatusPin, INPUT);

    delay(1000);
    Serial.println("---------CONFIG------------ MIN, MAX, REF");
    Serial.println(minVoltage);
    Serial.println(maxVoltage);
    Serial.println(REF_VOLTAGE_MILLIVOLTS);

    // Set 12 bit ADC resolution
    analogReadResolution(ADC_RESOLUTION);
    battery.begin(REF_VOLTAGE_MILLIVOLTS, VOLTAGE_DIVIDER_RAITO, &asigmoidal);
}

boolean PowerStatus::getConnected()
{
    return digitalRead(powerSensePin) == HIGH;
}

uint8_t PowerStatus::getBatteryLevelPercent()
{
    Serial.println("---------Voltage and Level--------");
    Serial.println(analogRead(GPIO_NUM_36));
    Serial.println(battery.voltage());
    Serial.println(battery.level());

    return battery.level();
}

// TODO: Implement logic to indicate battery not connected
ChargeingStatus PowerStatus::getChargingStatus()
{
    return digitalRead(chargeStatusPin) == LOW ? CHARGING : FULL;
}
