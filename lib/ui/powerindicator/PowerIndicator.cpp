#include "PowerIndicator.h"

PowerIndicator::PowerIndicator(
    GxEPD2_BW<GxEPD2_290_T94_V2, (uint16_t)296U> &display,
    PowerStatus &powerStatus
) : display(display), powerStatus(powerStatus)
{
    int16_t x, y;
    uint16_t width, height, batteryStart;
    
    // Init battery percentage box in the top right corner
    display.getTextBounds(PERCENT_LENGTH, 0, 0, &x, &y, &width, &height);
    uint16_t batteryBoxStartX = display.width() - width;
    batteryBox = new Box(batteryBoxStartX, 0, width, height, 0, 0);

    // Init Power Icon 
    uint16_t pluggedBoxStartX = batteryBoxStartX - ICON_W - 1;
    pluggedBox = new Box(pluggedBoxStartX, 0, ICON_W, height, 0, 0);

    // Init Charging Icon
    uint16_t chargingBoxStartX = pluggedBoxStartX - ICON_W - 1;
    chargeingBox = new Box(chargingBoxStartX, 0, ICON_W, height, 0, 0);

    widgetTotalWidth = display.width() - chargingBoxStartX;
    widgetStartX = chargingBoxStartX;
}

void PowerIndicator::refresh()
{

    // Serial.println("-------------");
    // Serial.println(batteryBox->x());
    // Serial.println(batteryBox->y());
    // Serial.println(batteryBox->wdth());
    // Serial.println(batteryBox->hght());

    // Serial.println("------- Widget -------");
    // Serial.println(widgetStartX);
    // Serial.println(widgetTotalWidth);


    display.setPartialWindow(widgetStartX, 0, widgetTotalWidth, batteryBox->hght());
    do {
        // Clear display
        display.fillRect(widgetStartX, 0, widgetTotalWidth, batteryBox->hght(), GxEPD_WHITE);

        // Draew Icon If charging
        if (powerStatus.getChargingStatus() == CHARGING) {
            Serial.println("---------IN CHARGING ----------");
            display.fillRect(
                chargeingBox->x(),
                chargeingBox->y(),
                chargeingBox->wdth(),
                chargeingBox->hght(),
                GxEPD_BLACK
            );
        }

        // Draew Icon If connected
        if (powerStatus.getConnected()) {
            Serial.println("---------IN CONNECTED ----------");
            display.fillRect(
                pluggedBox->x(),
                pluggedBox->y(),
                pluggedBox->wdth(),
                pluggedBox->hght(),
                GxEPD_BLACK
            );
        }

        display.setCursor(batteryBox->x(), 0 + batteryBox->hght());
        display.print(powerStatus.getBatteryLevelPercent());

    } while (display.nextPage());
}
