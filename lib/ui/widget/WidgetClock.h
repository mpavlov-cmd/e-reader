#ifndef WIDGETCLOCK_H
#define WIDGETCLOCK_H

#include "AbstractWidget.h"
#include "DateTimeRtc.h"

class WidgetClock : public AbstractWidget<DateTimeRtc>
{
    
public:
    WidgetClock(GxEPD2_GFX &mDisplay) : AbstractWidget(mDisplay) {}

private:

    const char CLOCK_SEGMENT[3]       = "55"; 
    const uint8_t NUM_BOXES           = 6;
    const uint8_t CLOCK_PADDING       = 6;
    const uint8_t CLOCK_EXTRA_PADDING = 6;

    uint16_t clockBoxXY[6][2] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

    int16_t tbx, tby, startX, startY, totalW, totalH;
    uint16_t tbw, tbh, framew, frameh;

     
    DBox createBox(DateTimeRtc &widgetData) override
    {
        // Get text bounds from display
        display.getTextBounds(CLOCK_SEGMENT, 0, 0, &tbx, &tby, &tbw, &tbh);

        // Calculate frames
        framew = tbw + 4;
        frameh = tbh + 4;

        totalW = framew * NUM_BOXES + CLOCK_PADDING * (NUM_BOXES - 1) + CLOCK_EXTRA_PADDING;
        totalH = frameh;

        startX = ((display.width() - totalW) / 2) - 0;
        startY = ((display.height() - totalH) / 2) - 0;

        addTillEight(startX);
        addTillEight(startY);
        addTillEight(totalW);
        addTillEight(totalH);

        return {startX, startY, (uint16_t) totalW, (uint16_t) totalH, 0, 0};
    }

    void initialize(DateTimeRtc& widgetData) override {

        uint16_t cursor = box.x;
        for (uint8_t i = 0; i < NUM_BOXES; i++)
        {
            clockBoxXY[i][0] = cursor;
            clockBoxXY[i][1] = startY;

            cursor += framew;
            cursor += CLOCK_PADDING;
            if (i == 2)
            {
                cursor += CLOCK_EXTRA_PADDING;
            }
        }
    }

    void beforePrint(DateTimeRtc& widgetData) override {

    }

    void print(DateTimeRtc& widgetData) override
    {
        for (uint8_t i = 0; i < 6; i++)
		{
			uint8_t changed = bitRead(widgetData.getMask(), i);
			if (changed == 0)
			{
				// continue;
			}

			char printVal[3];
			sprintf(printVal, "%02d", widgetData.byIndex(i));

			display.fillRect(clockBoxXY[i][0] + 1, clockBoxXY[i][1] + 4, framew - 2, frameh - 2, GxEPD_WHITE);

			display.setCursor(clockBoxXY[i][0] + 1, clockBoxXY[i][1] + tbh + 4);
			display.print(printVal);
		}
    }

    void afterPrint(DateTimeRtc& widgetData) override {
       widgetData.flushMask();
    }

    void addTillEight(int16_t& value)
    {
        while (value % 8 != 0)
        {
            value++;
        }

        Serial.printf("Adjusted value to: %i\n", value);
    }
};

#endif