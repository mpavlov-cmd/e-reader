#ifndef INTENTFILESELECTOR_H
#define INTENTFILESELECTOR_H

#include <AbstractDisplayIntent.h>
#include <IntentIdentifier.h>
#include <FileManager.h>
#include <model/menu/Menu.h>

struct IntentFileSelector : public AbstractDisplayIntent
{

private:
    static constexpr const uint16_t MAX_FILES = 1024;

    FileManager& fileManager;

    // Owned by intent
    Set<FileIndex> dirIndex; 
    Menu* menu = nullptr;


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
        delete menu;
    }
};

#endif