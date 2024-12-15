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
#include <model/menu/Menu.h>
#include <ButtonActions.h>
#include <IntentIdentifier.h>
#include <util/MenuInputHandler.h>

struct IntentHome : public AbstractDisplayIntent
{
    private:
        ESP32Time& espTime;
        FileManager& fileManager;
        ImageDrawer& imageDrawer;

        WidgetMenu* widgetMenu = nullptr;
        WidgetClock* widgetClock = nullptr;

        // Main menu and clock models
        Menu* menu = nullptr;
        DBox* menuBox = nullptr;

        DateTimeRtc dt = DateTimeRtc();

    public:
        // Constant declaration
        static constexpr const uint8_t ID = INTENT_ID_HOME;

        IntentHome(GxEPD2_GFX& display, ESP32Time& espTime, FileManager& fm, ImageDrawer& idrawer);

        void onStartUp(IntentArgument arg) override;
        void onFrequncy() override;
        void onExit() override;

        ActionResult onAction(ActionArgument arg) override;
        uint8_t getId() override;

        ~IntentHome() {
            Serial.println("IntentHome Destructor Start");
            delete menu;
            delete menuBox;
            delete widgetMenu;
            delete widgetClock;
            Serial.println("IntentHome Destructor End");
        };
};

#endif
