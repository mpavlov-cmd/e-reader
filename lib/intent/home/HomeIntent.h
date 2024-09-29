#ifndef HOMEINTENT_H
#define HOMEINTENT_H

#pragma once

#include <AbstractDisplayIntent.h>
#include <DateTimeRtc.h>
#include <ESP32Time.h>

struct HomeIntent : public AbstractDisplayIntent
{
    private:
        // ESP Time should be passed through comnstructor
        ESP32Time espTime;

        // Constant declaration
        const String NAME = "Home Intent";
        const char CLOCK_SEGMENT[3] = "55"; 
        const uint8_t CLOCK_PADDING = 6;
        const uint8_t CLOCK_EXTRA_PADDING = 6;

        // Stateful properties
        DateTimeRtc dt = DateTimeRtc();
        uint16_t clockBoxXY[6][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
        int16_t tbx, tby;
        uint16_t tbw, tbh, framew, frameh, startX, startY, totalW, totalH;
        

        void initClockCoordinates();

    public:
        HomeIntent(GxEPD2_GFX& display, ESP32Time &espTime);

        String getName() override;
        void onStartUp() override;
        void onFrequncy(uint32_t freqMills) override;
        void onAction(uint16_t actionId) override;
        void onExit() override;
};


#endif