#include "HomeIntent.h"


HomeIntent::HomeIntent(
    GxEPD2_BW<GxEPD2_290_T94_V2, (uint16_t)296U> &display, ESP32Time &espTime
) : AbstractDisplayIntent(display), espTime(espTime)
{
    // Fill clock coordinates array on startup
    initClockCoordinates();
}

String HomeIntent::getName()
{
    return NAME;
}

void HomeIntent::onStartUp()
{
    do {
		for (uint8_t i = 0; i < 6; i++) {
			display.drawRect(clockBoxXY[i][0], clockBoxXY[i][1], framew, frameh, GxEPD_BLACK);
		}
	} while (display.nextPage());
   
}

void HomeIntent::onFrequncy(uint32_t freqMills)
{
    display.setPartialWindow(0, 0, display.width(), display.height());

	dt.setValue(espTime.getHour(), IDX_HOUR);
	dt.setValue(espTime.getMinute(), IDX_MIN);
	dt.setValue(espTime.getSecond(), IDX_SEC);
	dt.setValue(espTime.getDay(), IDX_DAY);
	dt.setValue(espTime.getMonth(), IDX_MON);
	dt.setValue(espTime.getYear()%100, IDX_YEAR);

	uint8_t mask = dt.getMask();
	for (uint8_t i = 0; i < 6; i++)
    {
		uint8_t changed = bitRead(mask, i);
		if (changed == 0) {
			continue;
		}

		char printVal[3];
        sprintf(printVal, "%02d", dt.byIndex(i));

		do
		{
			display.fillRect(clockBoxXY[i][0] + 1, clockBoxXY[i][1] + 1, framew - 2, frameh - 2, GxEPD_WHITE);
			display.setCursor(clockBoxXY[i][0] + 1, clockBoxXY[i][1] + tbh + 1);
			display.print(printVal);
		} while (display.nextPage());	
	}

	dt.flushMask();	

}

void HomeIntent::onAction(uint16_t actionId)
{
}

void HomeIntent::onExit()
{
}


void HomeIntent::initClockCoordinates()
{
    // Get text bounds from display 
    display.getTextBounds(CLOCK_SEGMENT, 0, 0, &tbx, &tby, &tbw, &tbh);

    // Calculate frames
    framew = tbw + 4;
	frameh = tbh + 4;

    uint8_t numBoxes = 6;

	uint16_t totalW = framew * numBoxes + CLOCK_PADDING * (numBoxes - 1) + CLOCK_EXTRA_PADDING;
	uint16_t totalH = frameh;

	uint16_t startX = ((display.width() - totalW) / 2) - 0;
	uint16_t startY = ((display.height() - totalH) / 2) - 0;

	uint16_t cursor = startX;
	for (uint8_t i = 0; i < numBoxes; i++)
    {
		clockBoxXY[i][0] = cursor;
		clockBoxXY[i][1] = startY;
		
		cursor += framew;
		cursor += CLOCK_PADDING;
		if (i == 2) {
			cursor += CLOCK_EXTRA_PADDING;
		}
	}

}