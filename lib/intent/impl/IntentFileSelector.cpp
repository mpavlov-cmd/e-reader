#include "IntentFileSelector.h"

IntentFileSelector::IntentFileSelector(GxEPD2_GFX &display, FileManager& fileManager) 
: AbstractDisplayIntent(display), fileManager(fileManager), dirIndex(MAX_FILES)
{
}

void IntentFileSelector::onStartUp(IntentArgument arg)
{
    Serial.printf("Intent FileSelector On Startup Called with argument: %s\n", arg.strValue);
    const char* rootPath = arg.strValue == nullptr ? "/" : arg.strValue;

    dirIndex.clear();
    bool indexed = fileManager.indexDirectory(arg.strValue, DirIndexConf::FULL, dirIndex);

    for (uint16_t i = 0; i < dirIndex.size(); i++) {
        FileIndex* fi = dirIndex.getItem(i);
        Serial.printf("File: %s isDirectory: %i\n", fi->getName(), fi->getIsDirectry());
    }

    // TODO: Convert file index to menu and show menu
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
    // Temp, allows to go back home
    return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
}

uint8_t IntentFileSelector::getId()
{
    return ID;
}
