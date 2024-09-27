#include "PowerIndicator.h"
#include <bolt.c> 
#include <plug.c> 

PowerIndicator::PowerIndicator(
    GxEPD2_BW<GxEPD2_290_T94_V2, (uint16_t)296U> &display,
    PowerStatus &powerStatus
) : display(display), powerStatus(powerStatus)
{
    int16_t x, y;
    uint16_t width, height, batteryStart;
    
    // Init battery percentage box in the top right corner
    display.getTextBounds(PERCENT_TEXT, 0, 0, &x, &y, &width, &height);
    height++;

    uint16_t batteryBoxStartX = display.width() - width;
    batteryBox = new Box(batteryBoxStartX, 0, width, height, 0, 0);

    // Init Power Icon 
    uint16_t pluggedBoxStartX = batteryBoxStartX - ICON_W - 1;
    pluggedBox = new Box(pluggedBoxStartX, 1, ICON_W, height, 0, 0);

    widgetTotalWidth = display.width() - pluggedBoxStartX;
    widgetStartX = pluggedBoxStartX;
}

void PowerIndicator::refresh()
{

    // display.setPartialWindow(widgetStartX, 0, widgetTotalWidth, batteryBox->hght());
    display.setPartialWindow(0, 0, display.width(), batteryBox->hght());
    do {
        // Clear display
        // display.fillRect(widgetStartX, 0, widgetTotalWidth, batteryBox->hght(), GxEPD_WHITE);
        display.fillRect(0, 0, display.width(), batteryBox->hght(), GxEPD_WHITE);

        // Draew Icon If connected
        if (powerStatus.getConnected()) {
            
            // Only show charging when connected, chargeAllowed and charging
            const unsigned char* icon = gImage_plug;
            if (powerStatus.getChageEnabled() && powerStatus.getChargingStatus() == CHARGING) {
                icon = gImage_bolt;
            }

            Serial.println("---------IN CONNECTED ----------");
            display.drawBitmap(
                pluggedBox->x(),
                pluggedBox->y(),
                icon,
                pluggedBox->wdth(),
                12,
                GxEPD_BLACK
            );
        }

        char resultBuffer[5]; // 3 for the number, 1 for %, and 1 for null terminator
        uint8_t levelPercent = powerStatus.getBatteryLevelPercent();
        snprintf(resultBuffer, sizeof(resultBuffer), "%3d%%", levelPercent);

        display.setCursor(batteryBox->x(), 0 + batteryBox->hght() - 1);
        display.print(resultBuffer);

        display.setCursor(0, 0 + batteryBox->hght() - 1);
        display.print(powerStatus.getBatteryVoltage());

        display.setCursor(50, 0 + batteryBox->hght() - 1);
        display.print(powerStatus.getVoltageDuringMeasuring());

    } while (display.nextPage());
}
