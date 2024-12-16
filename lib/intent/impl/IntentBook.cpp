#include "IntentBook.h"

IntentBook::IntentBook(GxEPD2_GFX &display) : AbstractDisplayIntent(display)
{
}

void IntentBook::onStartUp(IntentArgument arg)
{
    Serial.printf("Book intent started with arg: %s\n", arg.strValue);
    widgetText = new WidgetText(display);

    // TODO: Load text with actua file
    String text = "My text sample";

    ModelText modelTextObj = {textBox, CENTER_CENTER, text};
    modelText = &modelTextObj;

    // Serial.pintf does not work with Srings, it will work as text.c_str()
    // Serial.printf("--- Model text inside of statup: %s ---- \n", modelTextObj.text.c_str());
    widgetText->upgrade(*modelText);
}

void IntentBook::onFrequncy()
{
    Serial.println("IntentBook on frequency");
}

void IntentBook::onExit()
{
    Serial.println("IntentBook on exit");
}

ActionResult IntentBook::onAction(ActionArgument arg)
{
    // In case input hed -> go home
    if (arg.held) {
        return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
    }

    return ActionResult::VOID;
}

uint8_t IntentBook::getId()
{
    return ID;
}
