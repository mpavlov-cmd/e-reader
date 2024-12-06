#ifndef POWERWIDGET_H
#define POWERWIDGET_H

#include "AbstractWidget.h"
#include "PowerStatus.h"
#include <bolt.c> 
#include <plug.c> 

class PowerWidget : public AbstractWidget<PowerMetrics>
{

public:
    PowerWidget(GxEPD2_GFX &mDisplay) : AbstractWidget(mDisplay) {}

private: 

        const char PERCENT_TEXT[5] = "100%";
        const uint8_t ICON_W = 20;
         
        // Define boxes for UI elements 
        DBox batteryBox;
        DBox pluggedBox;

        uint16_t widgetStartX;
        uint16_t widgetTotalWidth;

    DBox createBox(PowerMetrics& widgetData) override
    {
        return {0, 0, (uint16_t) display.width(), 24, 0, 0};
    }

    void initialize(PowerMetrics& widgetData) override {
        
        int16_t x, y;
        uint16_t width, height, batteryStart;
        
        // Init battery percentage box in the top right corner
        display.getTextBounds(PERCENT_TEXT, 0, 0, &x, &y, &width, &height);
        height++;

        int16_t batteryBoxStartX = display.width() - width;
        batteryBox = { batteryBoxStartX, 6, width, height, 0, 0 };

        // Init Power Icon 
        int16_t pluggedBoxStartX = batteryBoxStartX - ICON_W - 1;
        pluggedBox = { pluggedBoxStartX, 6, ICON_W, height, 0, 0 };

        widgetTotalWidth = display.width() - pluggedBoxStartX;
        widgetStartX = pluggedBoxStartX;
    }

    void beforePrint(PowerMetrics& widgetData) override {

    }

    void print(PowerMetrics& widgetData) override {

        // Draew Icon If connected
        if (widgetData.isConnected) {
            
            // Only show charging when connected, chargeAllowed and charging
            const unsigned char* icon = gImage_plug;
            if (widgetData.chargingStatus == CHARGING) {
                icon = gImage_bolt;
            }

            display.drawBitmap(pluggedBox.x, pluggedBox.y, icon, pluggedBox.width, 12, GxEPD_BLACK);
        }

        // TODO: Figure out whu 
        char resultBuffer[5]; // 3 for the number, 1 for %, and 1 for null terminator
        uint8_t levelPercent = widgetData.battLevelPercent;
        snprintf(resultBuffer, sizeof(resultBuffer), "%3d%%", levelPercent);

        display.setCursor(batteryBox.x, batteryBox.height + 2);
        display.print(resultBuffer);

        display.setCursor(5, batteryBox.height + 2);
        display.print(widgetData.battVoltageMillivolts);

       // display.setCursor(50, 0 + batteryBox->getHeight() - 1);
       // display.print(powerStatus.getVoltageDuringMeasuring());
    }

    void afterPrint(PowerMetrics& widgetData) override {
    
    }

};

#endif