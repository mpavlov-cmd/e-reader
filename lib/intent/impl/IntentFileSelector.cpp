#include "IntentFileSelector.h"

IntentFileSelector::IntentFileSelector(GxEPD2_GFX &display, FileManager& fileManager) 
: AbstractDisplayIntent(display), fileManager(fileManager), dirIndex(MAX_FILES)
{
}

void IntentFileSelector::onStartUp(IntentArgument arg)
{
    Serial.printf("Intent FileSelector On Startup Called with argument: %s\n", arg.strValue);
    const char* rootPath = arg.strValue == nullptr ? "/" : arg.strValue;

    // TODO: Check if argument is file or directory. If file, potentially fire action immedeately
    dirIndex.clear();
    bool indexed = fileManager.indexDirectory(arg.strValue, DirIndexConf::FULL, dirIndex);

    Set<MenuItem> menuItems(MAX_FILES);
    for (uint16_t i = 0; i < dirIndex.size(); i++) {

        FileIndex* fi = dirIndex.getItem(i);
        bool isActive = i==0;
        Serial.printf("File: %s isDirectory: %i\n", fi->getName(), fi->getIsDirectry());

        MenuItem* menuItem = nullptr;
        if (fi->getIsDirectry()) {
            size_t newLength = strlen(DIR) + strlen(fi->getName()) + 1; // +1 for null terminator
            char* newValue = new char[newLength];

            // Combine the strings
            strcpy(newValue, DIR);
            strcat(newValue, fi->getName());

            menuItem = new MenuItem(i, newValue, fi->getPath(), isActive);
            delete newValue;
        } else {
            menuItem = new MenuItem(i, fi->getName(), fi->getPath(), isActive);
        }

        // Add item to the menu
        menuItems.addItem(menuItem);
    }

    // (480 - 432) / 2 = 24
    // (800 - 704) / 2 = 48    
    menuBox = new DBox{24, 48, 432, 704, 0, 0};
    menu = new Menu(*menuBox, menuItems);

    widgetMenu = new WidgetMenu(display);
    widgetMenu->upgrade(*menu);
}

void IntentFileSelector::onFrequncy()
{
    Serial.println("Intent FileSelector On Frequency Called");
}

void IntentFileSelector::onExit()
{
    Serial.println("Intent FileSelector On Frequency Called");
}

ActionResult IntentFileSelector::onAction(uint16_t actionId)
{
    // TODO: Implemeng button navigation, when menu item is selected: either call change intent to self, or go back
    // TODO: Allow to go back to the previous dir level
    Serial.printf("Inside of action: %i\n", actionId);

    // TODO: Duplicate codel see IntnetHome
	// Extract hold bit for easy use
	bool held = false;
	uint8_t action = controlDirection(actionId, held);

	// Up or down
	if (action == BUTTON_ACTION_DOWN || action == BUTTON_ACTION_UP)
	{
		bool direction = action == BUTTON_ACTION_DOWN ? true : false;
		menu->moveActiveItem(direction);
		widgetMenu->upgrade(*menu);

		return ActionResult::VOID;
	}

	if (action == B00001000)
	{
		Serial.println("--- Enter clicked ---");
    }

    return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
}

uint8_t IntentFileSelector::getId()
{
    return ID;
}
