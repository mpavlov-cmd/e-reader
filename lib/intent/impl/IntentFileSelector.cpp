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
        Serial.printf("File: %s isDirectory: %i\n", fi->getName(), fi->getIsDirectry());

        // TODO: Extend menu with value and icon
        MenuItem* menuItem = new MenuItem(i, fi->getName(), i==0);
        menuItems.addItem(menuItem);
    }

    // (480 - 432) / 2 = 24
    // (800 - 704) / 2 = 48    
    // TODO: move box and menu to private class variable;
    DBox box{24, 48, 432, 704, 0, 0};
    Menu menu(box, menuItems);

    widgetMenu = new WidgetMenu(display);
    widgetMenu->upgrade(menu);
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

    // Temp, allows to go back home
    return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
}

uint8_t IntentFileSelector::getId()
{
    return ID;
}
