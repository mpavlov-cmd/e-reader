#include "MenuInputHandler.h"

bool handleMenuNavigation(Menu &menu, WidgetMenu &widgetMenu, uint8_t controlBit)
{
    bool upOrDown = controlBit == BUTTON_ACTION_DOWN || controlBit == BUTTON_ACTION_UP;
    if (!upOrDown) {
        return false;
    }

    bool direction = controlBit == BUTTON_ACTION_DOWN ? true : false;
		menu.moveActiveItem(direction);
		widgetMenu.upgrade(menu);

    return true;
}