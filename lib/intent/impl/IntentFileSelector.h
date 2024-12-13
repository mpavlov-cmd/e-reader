#ifndef INTENTFILESELECTOR_H
#define INTENTFILESELECTOR_H

#include <AbstractDisplayIntent.h>
#include <IntentIdentifier.h>
#include <FileManager.h>
#include <model/menu/Menu.h>
#include <widget/WidgetMenu.h>
#include <ButtonActions.h>

struct IntentFileSelector : public AbstractDisplayIntent
{

private:
    static constexpr const uint16_t MAX_FILES = 1024;
    static constexpr const char* DIR = "<DIR>";

    FileManager& fileManager;

    Set<FileIndex> dirIndex; 
    Menu* menu = nullptr;
    DBox* menuBox = nullptr;
    WidgetMenu* widgetMenu = nullptr;


public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_FILE_SELECTOR;

    IntentFileSelector(GxEPD2_GFX &display, FileManager& fileManager);

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(uint16_t actionId) override;
    uint8_t getId() override;

    ~IntentFileSelector()
    {
        Serial.println("IntentFileSelector Destructor Start");
        delete menu;
        delete menuBox;
        delete widgetMenu;
        Serial.println("IntentFileSelector Destructor End");
    }
};

#endif