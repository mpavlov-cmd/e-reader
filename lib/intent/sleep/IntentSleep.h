#ifndef INTENTSLEEP_H
#define INTENTSLEEP_H

#include <AbstractDisplayIntent.h>

struct IntentSleep : public AbstractDisplayIntent
{

public:
    // Constant declaration
    static constexpr const uint8_t ID = 1;

    IntentSleep(GxEPD2_GFX &display);

    void onStartUp() override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(uint16_t actionId) override;
    uint8_t getId() override;

    ~IntentSleep()
    {
    }
};

// const String const IntentSleep::NAME = "Sleep Intent";

#endif