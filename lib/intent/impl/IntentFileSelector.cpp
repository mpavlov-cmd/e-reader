#include "IntentFileSelector.h"

void IntentFileSelector::prepareAndRnderDir(const char *path)
{
    // Save current path 
    currentPath = path;
    
    const char* levelUpPath = getParentDir(path);

    // Init menu items
    Set<MenuItem> menuItems(MAX_FILES);

    // Add top level item
    String dirString = String(DIR) + "..";
    MenuItem* parentLevelitem = new MenuItem(0, dirString.c_str(), levelUpPath, false);
    menuItems.addItem(parentLevelitem);

    // Build dir index
    dirIndex.clear();
    bool indexed = fileManager.indexDirectory(path, DirIndexConf::NO_HIDDEN, dirIndex);

    for (uint16_t i = 0; i < dirIndex.size(); i++) {

        FileIndex* fi = dirIndex.getItem(i);
        bool isActive = i==0;
        uint16_t id = i + 1;

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

IntentFileSelector::IntentFileSelector(GxEPD2_GFX &display, FileManager &fileManager)
    : AbstractDisplayIntent(display), fileManager(fileManager), dirIndex(MAX_FILES)
{
}

void IntentFileSelector::onStartUp(IntentArgument arg)
{
    Serial.printf("Intent FileSelector On Startup Called with argument: %s\n", arg.strValue);
    const char* rootPath = arg.strValue == nullptr ? "/" : arg.strValue;

    prepareAndRnderDir(rootPath);

}

void IntentFileSelector::onFrequncy()
{
    Serial.println("Intent FileSelector On Frequency Called");
}

void IntentFileSelector::onExit()
{
    Serial.println("Intent FileSelector On Frequency Called");
}

ActionResult IntentFileSelector::onAction(ActionArgument arg)
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

    Serial.println("--- Enter clicked ---");
    MenuItem* activeItem = menu->getActiveItem();
    if (activeItem == nullptr) {
        return ActionResult::VOID;
    }

    const char* newFilePath = activeItem->getValue();
    Serial.printf("Selected file: %s\n", newFilePath);

    File selectedFile = fileManager.openFile(newFilePath, FILE_READ);
    bool isDirectory = selectedFile.isDirectory();

    // Make sure the value is copied, otherwise after file is closed pointer would be deleted
    static char storedPacthCa[256]; 
    strncpy(storedPacthCa, selectedFile.path(), sizeof(storedPacthCa) - 1);
    storedPacthCa[sizeof(storedPacthCa) - 1] = '\0'; 

    selectedFile.close();

    // Get char array for the stored path 
    if (!isDirectory) {
        const char* fileExt = getFileExtension(storedPacthCa);

        // Choose new intent based on the extention
        if (strcmp("txt", fileExt) == 0) {
            IntentArgument bookArg(storedPacthCa);
            return {ActionRetultType::CHANGE_INTENT, INTENT_ID_BOOK, bookArg};
        }

        return ActionResult::VOID;
    }


    if (currentPath == "/" && strcmp("/", storedPacthCa) == 0) {
        return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
    }

    // File is directory
    delete menu;
    delete widgetMenu;

    prepareAndRnderDir(storedPacthCa);
    
    return ActionResult::VOID;
}

uint8_t IntentFileSelector::getId()
{
    return ID;
}
