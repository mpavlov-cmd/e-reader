#include "PowerStatus.h"


PowerStatus::PowerStatus(
    uint8_t powerSensePin,
    uint8_t chargeStatusPin,
    uint8_t batterySensePin,
    uint8_t chargeEnablePin
) : 
        battery(VOLTAGE_MIN_MILLIVOLTS, VOLTAGE_MAX_MILLIVOLTS, batterySensePin, ADC_RESOLUTION),
        powerSensePin(powerSensePin), 
        chargeStatusPin(chargeStatusPin),
        chargeEnablePin(chargeEnablePin)
{
    pinMode(powerSensePin, INPUT);
    pinMode(chargeStatusPin, INPUT);
    pinMode(chargeEnablePin, OUTPUT);
    digitalWrite(chargeEnablePin, HIGH);

    // Set 12 bit ADC resolution
    analogReadResolution(ADC_RESOLUTION);
    battery.begin(VOLTAGE_REF_MILLIVOLTS, VOLTAGE_DIVIDER_RAITO, &asigmoidal);
}

boolean PowerStatus::getChageEnabled()
{
    return digitalRead(chargeEnablePin) == HIGH;
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

void PowerStatus::allowCharge(boolean chargeAllowed)
{
    uint8_t highOrLow = chargeAllowed == true ? HIGH : LOW;
    digitalWrite(chargeEnablePin, highOrLow);
}
