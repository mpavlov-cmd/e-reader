#ifndef INTENTBOOK_H
#define INTENTBOOK_H

#include <AbstractDisplayIntent.h>
#include <IntentIdentifier.h>

struct IntentBook : public AbstractDisplayIntent
{

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_BOOK;

    IntentBook(GxEPD2_GFX &display);

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;

    ~IntentBook()
    {
    }
};

#endif