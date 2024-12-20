#ifndef INTENTBOOK_H
#define INTENTBOOK_H

#include <AbstractDisplayIntent.h>
#include <IntentIdentifier.h>
#include <widget/WidgetText.h>
#include <text/TextIndex.h>
#include <cache/DirectoryCache.h>

struct IntentBook : public AbstractDisplayIntent
{
private:
    TextIndex& textIndex;
    FileManager& fileManager;

    DBox textBox{24, 48, 432, 704, 2, 0};

    ModelText* modelText = nullptr;
    WidgetText* widgetText = nullptr;

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_BOOK;

    IntentBook(GxEPD2_GFX &display, TextIndex &textIndex, FileManager &fileManager);

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;

    ~IntentBook()
    {
        delete widgetText;
    }
};

#endif