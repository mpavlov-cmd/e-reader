#include "HomeIntent.h"

HomeIntent::HomeIntent(
    GxEPD2_GFX& display, ESP32Time &espTime, FileManager& fm, ImageDrawer& idrawer, MenuDrawer& menuDrawer
) : AbstractDisplayIntent(display), espTime(espTime), fileManager(fm), imageDrawer(idrawer), menuDrawer(menuDrawer)
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
	menu = new Menu(menuBox, false);

	MenuItem* booksItem = new MenuItem(1, "Books");
	booksItem->setIsActive(true);

	menu->addItem(booksItem);
	menu->addItem(new MenuItem(2, "Settings"));
	menu->addItem(new MenuItem(3, "Other"));

	menuDrawer.drawMenu(*menu);
}

void HomeIntent::onFrequncy()
{
//     display.setPartialWindow(startX, startY, totalW, totalH);

// 	dt.setValue(espTime.getHour(), IDX_HOUR);
// 	dt.setValue(espTime.getMinute(), IDX_MIN);
// 	dt.setValue(espTime.getSecond(), IDX_SEC);
// 	dt.setValue(espTime.getDay(), IDX_DAY);
// 	dt.setValue(espTime.getMonth(), IDX_MON);
// 	dt.setValue(espTime.getYear()%100, IDX_YEAR);

// 	uint8_t mask = dt.getMask();

// 	display.firstPage();
// 	do
// 	{
// 		for (uint8_t i = 0; i < 6; i++)
// 		{
// 			uint8_t changed = bitRead(mask, i);
// 			if (changed == 0)
// 			{
// 				// continue;
// 			}

// 			char printVal[3];
// 			sprintf(printVal, "%02d", dt.byIndex(i));

// 			display.fillRect(clockBoxXY[i][0] + 1, clockBoxXY[i][1] + 1, framew - 2, frameh - 2, GxEPD_WHITE);
// 			display.setCursor(clockBoxXY[i][0] + 1, clockBoxXY[i][1] + tbh + 1);
// 			display.print(printVal);
// 		}
// 	} while (display.nextPage());
// 	dt.flushMask();	
}

void HomeIntent::onAction(uint16_t actionId)
{
	Serial.printf("Inside of action: %i\n", actionId);
	switchActiveMenu(true);
	menuDrawer.drawMenu(*menu);
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

// TODO: store reference to active menu item, to avoid search
void HomeIntent::switchActiveMenu(bool direction)
{
	uint16_t menuSize = menu->size();

	// Menu has at least one item
	if (menuSize == 0) {
		return;
	}
	
	MenuItem* currentActiveItem = nullptr;
	uint16_t activeItemIndex = 0;

	for (uint16_t i = 0; i < menuSize; i++)
	{
		MenuItem* item = menu->getItem(i);
		if (item->getIsActive()) {
			currentActiveItem = item;
			activeItemIndex = i;
			break;
		}
	}

	// Remove active flag from the current item
	if (currentActiveItem != nullptr) {
		currentActiveItem->setIsActive(false);
	}

	// No active items at all, or forward direction and currenyly active item islast
	if (currentActiveItem == nullptr || (direction && activeItemIndex == menuSize - 1)) {
		menu->getItem(0)->setIsActive(true);
		return;
	}

	// backward direction and currentkly active item is first
	if (!direction && activeItemIndex == 0) {
		menu->getItem(menuSize - 1)->setIsActive(true);
		return;
	}

	uint16_t newActiveIndex = direction ? activeItemIndex + 1 : activeItemIndex - 1;

	menu->getItem(newActiveIndex)->setIsActive(true);
}
