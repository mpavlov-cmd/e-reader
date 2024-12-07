#include "IntentSleep.h"

IntentSleep::IntentSleep(GxEPD2_GFX &display) : AbstractDisplayIntent(display)
{
}

void IntentSleep::onStartUp() {
    Serial.println("Sleep intenet started");
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
