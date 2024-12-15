#ifndef MENUINPUTUTIL_H
#define MENUINPUTUTIL_H

#include <Arduino.h>
#include <ButtonActions.h>
#include <widget/WidgetMenu.h>
#include <model/menu/Menu.h>

/**
 * Takes menu and menu widget and handles up / down menu navigation to avoid duplicate code in menu based intents
 * 
 * @param menu menu model
 * @param widgetMenu widget to upadte
 * @param controlBit input direction 
 * 
 */
bool handleMenuNavigation(Menu& menu, WidgetMenu& widgetMenu, uint8_t controlBit);

#endif

