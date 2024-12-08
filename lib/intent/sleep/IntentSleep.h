#ifndef INTENTSLEEP_H
#define INTENTSLEEP_H

#include <AbstractDisplayIntent.h>
#include <IntentIdentifier.h>
#include <widget/WidgetImage.h>
#include <SleepControl.h>

struct IntentSleep : public AbstractDisplayIntent
{

private: 
    SleepControl& sleepControl;
    WidgetImage& widgetImage;

    ImageModel imgModel;

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_SLEEP;

    IntentSleep(GxEPD2_GFX &display, SleepControl &sleepControl, WidgetImage &widgetImage);

    void onStartUp() override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(uint16_t actionId) override;
    uint8_t getId() override;

    ~IntentSleep()
    {
    }
};

#endif