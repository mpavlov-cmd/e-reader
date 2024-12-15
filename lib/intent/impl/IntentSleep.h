#ifndef INTENTSLEEP_H
#define INTENTSLEEP_H

#include <AbstractDisplayIntent.h>
#include <IntentIdentifier.h>
#include <widget/WidgetImage.h>
#include <SleepControl.h>
#include <PinDefinitions.h>

struct IntentSleep : public AbstractDisplayIntent
{

private: 
    SleepControl& sleepControl;
    ImageDrawer& imageDrawer;
    FileManager& fileManager;

    WidgetImage* widgetImage = nullptr;
    ImageModel imgModel;

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_SLEEP;

    IntentSleep(GxEPD2_GFX &display, SleepControl &sleepControl, ImageDrawer& imageDrawer, FileManager& fileManager);

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;

    ~IntentSleep()
    {
        delete widgetImage;
    }
};

#endif