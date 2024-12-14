#include "IntentFileSelector.h"

IntentFileSelector::IntentFileSelector(GxEPD2_GFX &display, FileManager& fileManager) 
: AbstractDisplayIntent(display), fileManager(fileManager), dirIndex(MAX_FILES)
{
}

void IntentFileSelector::onStartUp(IntentArgument arg)
{
    Serial.printf("Intent FileSelector On Startup Called with argument: %s\n", arg.strValue);
    const char* rootPath = arg.strValue == nullptr ? "/" : arg.strValue;
    const char* levelUpPath = fileManager.getPreviousLevel(rootPath);

    Serial.printf("rootPath: %s\n", rootPath);
    Serial.printf("levelUpPath: %s\n", levelUpPath);

    // Init menu items
    Set<MenuItem> menuItems(MAX_FILES);

    // Add top level item
    MenuItem* parentLevelitem = new MenuItem(0, "<DIR>..", levelUpPath, false);
    menuItems.addItem(parentLevelitem);

    // Build dir index
    dirIndex.clear();
    Serial.printf("rootPath2: %s\n", rootPath);
    Serial.printf("arg.strValue: %s\n", arg.strValue);
    
    bool indexed = fileManager.indexDirectory(rootPath, DirIndexConf::FULL, dirIndex);

    for (uint16_t i = 0; i < dirIndex.size(); i++) {

        FileIndex* fi = dirIndex.getItem(i);
        bool isActive = i==0;
        uint16_t id = i + 1;

        Serial.printf("File: %s isDirectory: %i\n", fi->getName(), fi->getIsDirectry());

        MenuItem* menuItem = nullptr;
        if (fi->getIsDirectry()) {
            size_t newLength = strlen(DIR) + strlen(fi->getName()) + 1; // +1 for null terminator
            char* newValue = new char[newLength];

            // Combine the strings
            strcpy(newValue, DIR);
            strcat(newValue, fi->getName());

            menuItem = new MenuItem(id, newValue, fi->getPath(), isActive);
            delete newValue;
        } else {
            menuItem = new MenuItem(id, fi->getName(), fi->getPath(), isActive);
        }

        // Add item to the menu
        menuItems.addItem(menuItem);
    }
  
    menu = new Menu(menuBox, menuItems);
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
    Serial.printf("Inside of action: %i\n", actionId);

    // TODO: Duplicate code see IntnetHome
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
        MenuItem* activeItem = menu->getActiveItem();
        if (activeItem == nullptr) {
            return ActionResult::VOID;
        }

        const char* newFilePath = activeItem->getValue();
        Serial.printf("Selected file: %s\n", newFilePath);

        File selectedFile = fileManager.openFile(newFilePath, FILE_READ);
        bool isDirectory = selectedFile.isDirectory();

        // Mae sure the value is copied, otherwise after file is closed pointer would break
        String storedPath(selectedFile.path());
        selectedFile.close();

        if (!isDirectory) {
            // TODO: Fire intend ased on the fie extention
            return ActionResult::VOID;
        }

        // File is directory
        delete menu;
        delete widgetMenu;

        IntentArgument newDirArgument(storedPath.c_str());
        onStartUp(newDirArgument);
        
        return ActionResult::VOID;
    }

    return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
}

uint8_t IntentFileSelector::getId()
{
    return ID;
}
