#include "IntentBook.h"

IntentBook::IntentBook(GxEPD2_GFX &display) : AbstractDisplayIntent(display)
{
}

void IntentBook::onStartUp(IntentArgument arg)
{
}

void IntentBook::onFrequncy()
{
}

void IntentBook::onExit()
{
}

ActionResult IntentBook::onAction(ActionArgument arg)
{
    return ActionResult::VOID;
}

uint8_t IntentBook::getId()
{
    return ID;
}
