#ifndef INTENTFILESELECTOR_H
#define INTENTFILESELECTOR_H

#include <AbstractDisplayIntent.h>
#include <IntentIdentifier.h>

struct IntentFileSelector : public AbstractDisplayIntent
{

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_FILE_SELECTOR;

    IntentFileSelector(GxEPD2_GFX &display);

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(uint16_t actionId) override;
    uint8_t getId() override;

    ~IntentFileSelector()
    {
    }
};

#endif