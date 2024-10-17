#include "Menu.h"

Menu::Menu(const Box mBox, const bool mFullWindow): box(mBox), fullWindow(mFullWindow)
{
    // Init array and allocate memory
    menuItemsArray = new MenuItem*[CAPACITY];
    index = 0;
    for (int i = 0; i < CAPACITY; ++i) {
        menuItemsArray[i] = nullptr;
    }
}

bool Menu::addItem(MenuItem *menuItem)
{
    if (index >= CAPACITY)
    {
        return false;
    }

    menuItemsArray[index++] = menuItem;

    return true;
}

MenuItem* Menu::getItem(uint16_t idx)
{
    if (idx >= CAPACITY) {
        Serial.println("Error, index out of capacity");
        return nullptr;
    }

    return menuItemsArray[idx];
}

bool Menu::removeLastItem()
{
    delete menuItemsArray[index];
    if (index == 0)
    {
        return false;
    }

    index--;
    return true;
}

uint16_t Menu::size()
{
    return index;
}

Box Menu::getBox()
{
    return box;
}

bool Menu::getFullWindow()
{
    return fullWindow;
}
