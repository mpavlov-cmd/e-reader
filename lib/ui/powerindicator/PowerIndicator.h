#ifndef POWERINDICATOR_H
#define POWERINDICATOR_H

#include <Arduino.h>
#include <GxEPD2_GFX.h>
#include <GxEPD2_BW.h>

#include <PowerStatus.h>
#include <box/Box.h>

struct PowerIndicator
{
    private:
        const char PERCENT_TEXT[5] = "100%";
        const uint8_t ICON_W = 20;
         
        GxEPD2_GFX& display;
        PowerStatus& powerStatus;

        // Define boxes for UI elements 
        Box* batteryBox = nullptr;
        Box* pluggedBox = nullptr;

        uint16_t widgetStartX;
        uint16_t widgetTotalWidth;

    public:
        PowerIndicator(
            GxEPD2_GFX& display,
            PowerStatus& powerStatus
        );

        void init();
        void refresh();
};

#endif