#ifndef POWERSTATUS_H
#define POWERSTATUS_H

#pragma once

#include <Arduino.h>
#include <Battery.h>
#include <ESP32Time.h>

enum ChargeingStatus { NC, CHARGING, FULL };

struct PowerStatus
{
    private:
        const float VOLTAGE_DIVIDER_RAITO     = 2.0;
        const uint16_t VOLTAGE_REF_MILLIVOLTS = 3300;
        const uint16_t VOLTAGE_MIN_MILLIVOLTS = 3000;
        const uint16_t VOLTAGE_MAX_MILLIVOLTS = 4200;

        const uint8_t ADC_RESOLUTION = 12;

        Battery battery;
        
        uint8_t powerSensePin;
        uint8_t chargeStatusPin;

        uint8_t batteryPercentLatch = 100;
        unsigned long timeLatch = 0;
        unsigned long timeLatchMeasureDelay = 0;

        boolean connectedLatch = false;
        boolean firstRun = true;
        boolean measureingDuringCharge = false;

    public:
        PowerStatus(
            uint8_t powerSensePin,
            uint8_t chargeStatusPin, 
            uint8_t batterySensePin
        );

        boolean getConnected();
        uint8_t getBatteryLevelPercent();
        ChargeingStatus getChargingStatus();

        // TEMP
        uint16_t getBatteryVoltage();

};
 

#endif