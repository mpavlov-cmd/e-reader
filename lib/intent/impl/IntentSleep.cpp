#include "IntentSleep.h"

IntentSleep::IntentSleep(GxEPD2_GFX &display, SleepControl& sleepControl, ImageDrawer& imageDrawer, FileManager& fileManager) 
    : AbstractDisplayIntent(display), sleepControl(sleepControl), imageDrawer(imageDrawer), fileManager(fileManager) {}

void IntentSleep::onStartUp(IntentArgument arg) {
    Serial.println("Sleep intenet started");

    widgetImage = new WidgetImage(display, imageDrawer, fileManager);

    imgModel = {"/.system/img/sleep.bmp", CENTER_CENTER};
    widgetImage->upgrade(imgModel);
    
	digitalWrite(PIN_LED, LOW);
    display.hibernate();
    sleepControl.sleepNow();
}

void IntentSleep::onFrequncy() {
    Serial.println("IntentSleep on frequency");
}

void IntentSleep::onExit()
{
}

ActionResult IntentSleep::onAction(ActionArgument arg)
{
    return ActionResult::VOID;
}

uint8_t IntentSleep::getId()
{
    return ID;
}
