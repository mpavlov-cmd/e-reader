#include "HomeIntent.h"
#include <menu/Menu.h>
#include <menu/MenuDrawer.h>

HomeIntent::HomeIntent(
    GxEPD2_GFX& display, ESP32Time &espTime, FileManager& fm, ImageDrawer& idrawer
) : AbstractDisplayIntent(display), espTime(espTime), fileManager(fm), imageDrawer(idrawer)
{

}

String HomeIntent::getName()
{
    return NAME;
}

void HomeIntent::onStartUp()
{
	// Fill clock coordinates array on startup
    initClockCoordinates();

	// Draw Background
	File file = fileManager.openFile("/background/ninja2.bmp", FILE_READ);
	imageDrawer.drawBitmapFromSD_Buffered(file, 0, 0, false, false);

	// Draw Main Menu
	Box menuBox(48, 512, 384, 256, 0, 0);
	Menu menu(menuBox, false);

	MenuItem* booksItem = new MenuItem(1, "Books");
	booksItem->setIsActive(true);

	menu.addItem(booksItem);
	menu.addItem(new MenuItem(2, "Settings"));
	menu.addItem(new MenuItem(3, "Other"));

	// TODO: move to dependency  
	MenuDrawer menuDrawer(display);
	menuDrawer.drawMenu(menu);
}

void HomeIntent::onFrequncy()
{
    display.setPartialWindow(startX, startY, totalW, totalH);

	dt.setValue(espTime.getHour(), IDX_HOUR);
	dt.setValue(espTime.getMinute(), IDX_MIN);
	dt.setValue(espTime.getSecond(), IDX_SEC);
	dt.setValue(espTime.getDay(), IDX_DAY);
	dt.setValue(espTime.getMonth(), IDX_MON);
	dt.setValue(espTime.getYear()%100, IDX_YEAR);

	uint8_t mask = dt.getMask();

	display.firstPage();
	do
	{
		for (uint8_t i = 0; i < 6; i++)
		{
			uint8_t changed = bitRead(mask, i);
			if (changed == 0)
			{
				// continue;
			}

			char printVal[3];
			sprintf(printVal, "%02d", dt.byIndex(i));

			display.fillRect(clockBoxXY[i][0] + 1, clockBoxXY[i][1] + 1, framew - 2, frameh - 2, GxEPD_WHITE);
			display.setCursor(clockBoxXY[i][0] + 1, clockBoxXY[i][1] + tbh + 1);
			display.print(printVal);
		}
	} while (display.nextPage());
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

	totalW = framew * numBoxes + CLOCK_PADDING * (numBoxes - 1) + CLOCK_EXTRA_PADDING;
	totalH = frameh;

	startX = ((display.width() - totalW) / 2) - 0;
	startY = ((display.height() - totalH) / 2) - 0;

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