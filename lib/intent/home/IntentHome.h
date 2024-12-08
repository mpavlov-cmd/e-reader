#ifndef INTENTHOME_H
#define INTENTHOME_H

#pragma once

#include <AbstractDisplayIntent.h>
#include <DateTimeRtc.h>
#include <ESP32Time.h>
#include <FileManager.h>
#include <ImageDrawer.h>
#include <widget/MenuWidget.h>
#include <widget/ClockWidget.h>
#include <menu/Menu.h>
#include <ButtonActions.h>
#include <IntentIdentifier.h>

struct IntentHome : public AbstractDisplayIntent
{
    private:
        ESP32Time& espTime;
        FileManager& fileManager;
        ImageDrawer& imageDrawer;

        MenuWidget& menuWidget;
        ClockWidget& clockWidget;

        // Main menu and clock models
        Menu* menu = nullptr;
        DateTimeRtc dt = DateTimeRtc();

    public:
        // Constant declaration
        static constexpr const uint8_t ID = INTENT_ID_HOME;

        IntentHome(GxEPD2_GFX& display, ESP32Time& espTime,
            FileManager& fm, ImageDrawer& idrawer, MenuWidget& menuWidget, ClockWidget& ClockWidget);

        void onStartUp() override;
        void onFrequncy() override;
        void onExit() override;

        ActionResult onAction(uint16_t actionId) override;
        uint8_t getId() override;

        ~IntentHome() {
            delete menu;
        };
};

#endif
