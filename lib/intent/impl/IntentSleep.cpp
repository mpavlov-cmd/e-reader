#include "IntentSleep.h"

IntentSleep::IntentSleep(GxEPD2_GFX &display, SleepControl& sleepControl, WidgetImage& widgetImage) 
    : AbstractDisplayIntent(display), widgetImage(widgetImage), sleepControl(sleepControl) {}

void IntentSleep::onStartUp(IntentArgument arg) {
    Serial.println("Sleep intenet started");

    imgModel = {"/.system/img/sleep.bmp", CENTER_CEMNTER};
    widgetImage.upgrade(imgModel);
    
    // TODO: Write LED to LOW, move pin definitions to a separate conf library
    display.hibernate();
    sleepControl.sleepNow();
}

void IntentSleep::onFrequncy() {
    Serial.println("Sleep intenet on frequency");
}

void IntentSleep::onExit()
{
}

ActionResult IntentSleep::onAction(uint16_t actionId)
{
    return ActionResult::VOID;
}

uint8_t IntentSleep::getId()
{
    return ID;
}
