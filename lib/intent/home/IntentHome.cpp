#include "IntentHome.h"

IntentHome::IntentHome(
	GxEPD2_GFX &display,
	ESP32Time &espTime,
	FileManager &fm,
	ImageDrawer &idrawer,
	MenuWidget &menuWidget,
	ClockWidget &clockWidget) : AbstractDisplayIntent(display), espTime(espTime), fileManager(fm), imageDrawer(idrawer),
								menuWidget(menuWidget), clockWidget(clockWidget)
{
}

void IntentHome::onStartUp()
{
	// Draw Background
	File file = fileManager.openFile("/background/ninja2.bmp", FILE_READ);
	imageDrawer.drawBitmapFromSD_Buffered(file, 0, 0, false, false);

	// Define menu items
	Set<MenuItem> menuItems(10);

	// Main menu. For simplifity menu ID here MUST match intent ID
	menuItems.addItem(new MenuItem(1, "Books", true));
	menuItems.addItem(new MenuItem(2, "Settings"));
	menuItems.addItem(new MenuItem(3, "Other"));
	menuItems.addItem(new MenuItem(4, "One more item for fun"));
	menuItems.addItem(new MenuItem(INTENT_ID_SLEEP, "Sleep"));

	menu = new Menu(menuItems);

	MenuWidget menuWidget(display);
	menuWidget.upgrade(*menu);
}

void IntentHome::onFrequncy()
{
	dt.setValue(espTime.getHour(), IDX_HOUR);
	dt.setValue(espTime.getMinute(), IDX_MIN);
	dt.setValue(espTime.getSecond(), IDX_SEC);
	dt.setValue(espTime.getDay(), IDX_DAY);
	dt.setValue(espTime.getMonth(), IDX_MON);
	dt.setValue(espTime.getYear() % 100, IDX_YEAR);

	clockWidget.upgrade(dt);
}

void IntentHome::onExit()
{
	Serial.println("Home Intent on Exit");
}

ActionResult IntentHome::onAction(uint16_t actionId)
{
	Serial.printf("Inside of action: %i\n", actionId);

	// Extract hold bit for easy use
	bool held = false;
	uint8_t action = controlDirection(actionId, held);

	// Up or down
	if (action == BUTTON_ACTION_DOWN || action == BUTTON_ACTION_UP)
	{
		bool direction = action == BUTTON_ACTION_DOWN ? true : false;
		menu->moveActiveItem(direction);
		menuWidget.upgrade(*menu);

		return ActionResult::VOID;
	}

	if (action == B00001000)
	{
		Serial.println("--- Enter clicked ---");

		// Validate active item and menu id value
		if (menu->getActiveItem() == nullptr) {
			return ActionResult::VOID;
		}

		uint16_t menuItemId = menu->getActiveItem()->getId();
		if (menuItemId > UINT8_MAX) {
			return ActionResult::VOID;
		}

		switch (menuItemId)
		{
		case INTENT_ID_SLEEP :
			return {ActionRetultType::CHANGE_INTENT, INTENT_ID_SLEEP};
		
		default:
			return ActionResult::VOID;
		}	
	};

	return ActionResult::VOID;
}

uint8_t IntentHome::getId()
{
	return ID;
}