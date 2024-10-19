#ifndef HOMEINTENT_H
#define HOMEINTENT_H

#pragma once

#include <AbstractDisplayIntent.h>
#include <DateTimeRtc.h>
#include <ESP32Time.h>
#include <FileManager.h>
#include <ImageDrawer.h>
#include <menu/MenuDrawer.h>
#include <menu/Menu.h>

struct HomeIntent : public AbstractDisplayIntent
{
    private:
        ESP32Time& espTime;
        FileManager& fileManager;
        ImageDrawer& imageDrawer;
        MenuDrawer& menuDrawer;

        // Main menu
        Menu* menu = new Menu();
        Box* menuBox = new Box(48, 584, 384, 160, 0, 0);

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
        HomeIntent(GxEPD2_GFX& display, ESP32Time& espTime,
            FileManager& fm, ImageDrawer& idrawer, MenuDrawer& menuDrawer);

        String getName() override;
        void onStartUp() override;
        void onFrequncy() override;
        void onAction(uint16_t actionId) override;
        void onExit() override;

        ~HomeIntent() {
            delete menu;
            delete menuBox;
        };
};


#endif