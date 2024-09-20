#ifndef POWERSTATUS_H
#define POWERSTATUS_H

#pragma once

#include <Arduino.h>
#include <Battery.h>

enum ChargeingStatus { NC, CHARGING, FULL };

struct PowerStatus
{
    private:
        const uint8_t VOLTAGE_DIVIDER_RAITO = 2;
        const uint16_t REF_VOLTAGE_MILLIVOLTS = 3300;

        Battery battery;
        uint8_t powerSensePin;
        uint8_t chargeStatusPin;

    public:
        PowerStatus(
            uint8_t powerSensePin,
            uint8_t chargeStatusPin, 
            uint8_t batterySensePin,
            uint16_t minVoltage,
            uint16_t maxVoltage
        );

        boolean getConnected();
        uint8_t getBatteryLevelPercent();
        ChargeingStatus getChargingStatus();
};
 

#endif