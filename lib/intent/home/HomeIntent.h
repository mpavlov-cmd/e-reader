#ifndef HOMEINTENT_H
#define HOMEINTENT_H

#pragma once

#include <AbstractDisplayIntent.h>
#include <DateTimeRtc.h>
#include <ESP32Time.h>
#include <FileManager.h>
#include <ImageDrawer.h>
#include <widget/MenuWidget.h>
#include <widget/ClockWidget.h>
#include <menu/Menu.h>

struct HomeIntent : public AbstractDisplayIntent
{
    private:
        ESP32Time& espTime;
        FileManager& fileManager;
        ImageDrawer& imageDrawer;

        // TODO: Intent can own widgets
        MenuWidget& menuWidget;
        ClockWidget& clockWidget;

        // Main menu and clock models
        Menu* menu = nullptr;
        DateTimeRtc dt = DateTimeRtc();

        // Constant declaration
        const String NAME = "Home Intent";

    public:
        HomeIntent(GxEPD2_GFX& display, ESP32Time& espTime,
            FileManager& fm, ImageDrawer& idrawer, MenuWidget& menuWidget, ClockWidget& ClockWidget);

        String getName() override;
        void onStartUp() override;
        void onFrequncy() override;
        void onAction(uint16_t actionId) override;
        void onExit() override;

        ~HomeIntent() {
            delete menu;
        };
};

#endif
