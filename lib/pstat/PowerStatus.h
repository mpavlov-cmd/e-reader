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
        const uint16_t VOLTAGE_REF_MILLIVOLTS = 4200;
        const uint16_t VOLTAGE_MIN_MILLIVOLTS = 3000;
        const uint16_t VOLTAGE_MAX_MILLIVOLTS = 4200;

        const uint8_t ADC_RESOLUTION = 12;

        Battery battery;
        uint8_t powerSensePin;
        uint8_t chargeStatusPin;
        uint8_t chargeEnablePin;

        uint8_t batteryPercentLatch;

    public:
        PowerStatus(
            uint8_t powerSensePin,
            uint8_t chargeStatusPin, 
            uint8_t batterySensePin,
            uint8_t chargeEnablePin
        );

        boolean getChageEnabled();
        boolean getConnected();
        uint8_t getBatteryLevelPercent();
        ChargeingStatus getChargingStatus();

        void allowCharge(boolean chargeAllowed);
};
 

#endif