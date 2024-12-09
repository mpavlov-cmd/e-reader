#include "IntentFileSelector.h"

IntentFileSelector::IntentFileSelector(GxEPD2_GFX &display) : AbstractDisplayIntent(display)
{
}

void IntentFileSelector::onStartUp(IntentArgument arg)
{
    Serial.println("Intent FileSelector On Startup Called");
}

void IntentFileSelector::onFrequncy()
{
    Serial.println("Intent FileSelector On Frequency Called");
}

void IntentFileSelector::onExit()
{
    Serial.println("Intent FileSelector On Frequency Called");
}

ActionResult IntentFileSelector::onAction(uint16_t actionId)
{
    return ActionResult::VOID;
}

uint8_t IntentFileSelector::getId()
{
    return ID;
}
