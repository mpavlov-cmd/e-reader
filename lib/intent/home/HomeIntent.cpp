#include "HomeIntent.h"
#include <ButtonActions.h>

HomeIntent::HomeIntent(
    GxEPD2_GFX& display,
	ESP32Time &espTime,
	FileManager& fm, 
	ImageDrawer& idrawer, 
	MenuWidget& menuWidget,
	ClockWidget& clockWidget
) : AbstractDisplayIntent(display), espTime(espTime), fileManager(fm), imageDrawer(idrawer), menuWidget(menuWidget), clockWidget(clockWidget)
{
}

String HomeIntent::getName()
{
    return NAME;
}

void HomeIntent::onStartUp()
{
	// Draw Background
	File file = fileManager.openFile("/background/ninja2.bmp", FILE_READ);
	imageDrawer.drawBitmapFromSD_Buffered(file, 0, 0, false, false);

	// Define menu items
	Set<MenuItem> menuItems(10);

	// Fill menu
	MenuItem* booksItem = new MenuItem(1, "Books", true);

	menuItems.addItem(booksItem);
	menuItems.addItem(new MenuItem(2, "Settings"));
	menuItems.addItem(new MenuItem(3, "Other"));
	menuItems.addItem(new MenuItem(4, "One more item for fun"));
	menuItems.addItem(new MenuItem(5, "Sleep"));

	menu = new Menu(menuItems);
	
	MenuWidget menuWidget(display);
	menuWidget.upgrade(*menu);
}

void HomeIntent::onFrequncy()
{
	dt.setValue(espTime.getHour(), IDX_HOUR);
	dt.setValue(espTime.getMinute(), IDX_MIN);
	dt.setValue(espTime.getSecond(), IDX_SEC);
	dt.setValue(espTime.getDay(), IDX_DAY);
	dt.setValue(espTime.getMonth(), IDX_MON);
	dt.setValue(espTime.getYear()%100, IDX_YEAR);

	clockWidget.upgrade(dt);
}

void HomeIntent::onAction(uint16_t actionId)
{
	Serial.printf("Inside of action: %i\n", actionId);

	// Extract hold bit for easy use
	bool held      = false;
	uint8_t action = controlDirection(actionId, held);

	// Up or nown
	if (action == BUTTON_ACTION_DOWN || action == BUTTON_ACTION_UP) {
		bool direction = action == BUTTON_ACTION_DOWN ? true : false;
		menu->moveActiveItem(direction);
		menuWidget.upgrade(*menu);
		return;
	}

	if (action == B00001000) {
		Serial.println("Enter clicked");
	};
}

void HomeIntent::onExit()
{
}
