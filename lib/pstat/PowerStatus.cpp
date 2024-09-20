#include "PowerStatus.h"


PowerStatus::PowerStatus(
    uint8_t powerSensePin,
    uint8_t chargeStatusPin,
    uint8_t batterySensePin,
    uint16_t minVoltage,
    uint16_t maxVoltage) : 
        battery(minVoltage, maxVoltage, batterySensePin),
        powerSensePin(powerSensePin), 
        chargeStatusPin(chargeStatusPin)
{
    pinMode(powerSensePin, INPUT);
    pinMode(chargeStatusPin, INPUT);

    battery.begin(REF_VOLTAGE_MILLIVOLTS, VOLTAGE_DIVIDER_RAITO, &sigmoidal);
}

boolean PowerStatus::getConnected()
{
    return digitalRead(powerSensePin) == HIGH;
}

uint8_t PowerStatus::getBatteryLevelPercent()
{
    return battery.level();
}

// TODO: Implement logic to indicate battery not connected
ChargeingStatus PowerStatus::getChargingStatus()
{
    return digitalRead(chargeStatusPin) == LOW ? CHARGING : FULL;
}
