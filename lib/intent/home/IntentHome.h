#ifndef INTENTHOME_H
#define INTENTHOME_H

#pragma once

#include <AbstractDisplayIntent.h>
#include <DateTimeRtc.h>
#include <ESP32Time.h>
#include <FileManager.h>
#include <ImageDrawer.h>
#include <widget/WidgetMenu.h>
#include <widget/WidgetClock.h>
#include <menu/Menu.h>
#include <ButtonActions.h>
#include <IntentIdentifier.h>

struct IntentHome : public AbstractDisplayIntent
{
    private:
        ESP32Time& espTime;
        FileManager& fileManager;
        ImageDrawer& imageDrawer;

        WidgetMenu& widgetMenu;
        WidgetClock& widgetClock;

        // Main menu and clock models
        Menu* menu = nullptr;
        DateTimeRtc dt = DateTimeRtc();

    public:
        // Constant declaration
        static constexpr const uint8_t ID = INTENT_ID_HOME;

        IntentHome(GxEPD2_GFX& display, ESP32Time& espTime,
            FileManager& fm, ImageDrawer& idrawer, WidgetMenu& menuWidget, WidgetClock& widgetClock);

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
