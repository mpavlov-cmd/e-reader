#ifndef POWERINDICATOR_H
#define POWERINDICATOR_H

#pragma once

#include <Arduino.h>
#include <PowerStatus.h>
#include <GxEPD2_BW.h>
#include <box/Box.h>

struct PowerIndicator
{
    private:
        const char PERCENT_LENGTH[5] = "100%";
        const uint8_t ICON_W = 13;
         

        GxEPD2_BW<GxEPD2_290_T94_V2, (uint16_t)296U> &display;
        PowerStatus powerStatus;

        // Define boxes for UI elements 
        Box* batteryBox = nullptr;
        Box* pluggedBox = nullptr;
        Box* chargeingBox = nullptr;

        uint16_t widgetStartX;
        uint16_t widgetTotalWidth;

    public:
        PowerIndicator(
            GxEPD2_BW<GxEPD2_290_T94_V2, (uint16_t)296U> &display,
            PowerStatus &powerStatus
        );

        void refresh();
};

#endif