#include "IntentHome.h"

IntentHome::IntentHome(
	GxEPD2_GFX &display,
	ESP32Time &espTime,
	FileManager &fm,
	ImageDrawer &idrawer) : AbstractDisplayIntent(display), espTime(espTime), fileManager(fm), imageDrawer(idrawer)
{
}

void IntentHome::onStartUp(IntentArgument arg)
{

	// Create widgets
	widgetMenu = new WidgetMenu(display);
	widgetClock = new WidgetClock(display);

	// Draw Background
	File file = fileManager.openFile("/background/ninja2.bmp", FILE_READ);
	imageDrawer.drawBitmapFromSD_Buffered(file, 0, 0, false, false);

	// Define menu items
	Set<MenuItem> menuItems(10);

	// Main menu. For simplifity menu ID here MUST match intent ID
	menuItems.addItem(new MenuItem(INTENT_ID_FILE_SELECTOR, "Select Book", true));
	menuItems.addItem(new MenuItem(2, "Settings"));
	menuItems.addItem(new MenuItem(3, "Other"));
	menuItems.addItem(new MenuItem(4, "One more item for fun"));
	menuItems.addItem(new MenuItem(INTENT_ID_SLEEP, "Sleep"));

	// Define Box and Menu
	// TODO: Avoid allocat
	menuBox = new DBox{48, 584, 384, 160, 0, 0};
	menu = new Menu(*menuBox, menuItems);

	widgetMenu->upgrade(*menu);
}

void IntentHome::onFrequncy()
{
	dt.setValue(espTime.getHour(), IDX_HOUR);
	dt.setValue(espTime.getMinute(), IDX_MIN);
	dt.setValue(espTime.getSecond(), IDX_SEC);
	dt.setValue(espTime.getDay(), IDX_DAY);
	dt.setValue(espTime.getMonth(), IDX_MON);
	dt.setValue(espTime.getYear() % 100, IDX_YEAR);

	widgetClock->upgrade(dt);
}

void IntentHome::onExit()
{
	Serial.println("Home Intent on Exit");
}

ActionResult IntentHome::onAction(ActionArgument arg)
{
	// Handle up or down
	if (handleMenuNavigation(*menu, *widgetMenu, arg.actionBit)) {
		return ActionResult::VOID;
	}

	// Anyting else
	if (arg.actionBit =! B00001000)
	{
		return ActionResult::VOID;
	}

	Serial.println("--- Home Intent Enter Clicked ---");

	// Validate active item and menu id value
	if (menu->getActiveItem() == nullptr) {
		return ActionResult::VOID;
	}

	uint16_t menuItemId = menu->getActiveItem()->getId();
	if (menuItemId > UINT8_MAX) {
		return ActionResult::VOID;
	}

	// From main menu go to FS root
	IntentArgument fileSelectorFsRoot("/");

	switch (menuItemId)
	{
	case INTENT_ID_SLEEP:
		return {ActionRetultType::CHANGE_INTENT, INTENT_ID_SLEEP, IntentArgument::NO_ARG};
	case INTENT_ID_FILE_SELECTOR:
		return {ActionRetultType::CHANGE_INTENT, INTENT_ID_FILE_SELECTOR, fileSelectorFsRoot};
	default:
		return ActionResult::VOID;
	}	
}

uint8_t IntentHome::getId()
{
	return ID;
}