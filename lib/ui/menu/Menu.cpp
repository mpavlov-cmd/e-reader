#include "Menu.h"

Menu::Menu()
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

int16_t Menu::getActiveItemIdx()
{
    for (uint16_t i = 0; i < index; i++)
	{
		MenuItem* item = getItem(i);
		if (item->getIsActive()) {
			return i;
		}
	}

    return -1;
}

void Menu::moveActiveItem(uint8_t direction)
{
    // Menu has at least one element
    if (size() == 0) {
		return;
	}

    int16_t activeItemIdx = getActiveItemIdx();
    // If no active item activate the existing one
    if (activeItemIdx == -1) {
        getItem(0)->setIsActive(true);
        return;
    }

    // Active item non-null
    // Deactivate current item
    getItem(activeItemIdx)->setIsActive(false);

    // forward direction and currenyly active item is last
	if (direction && activeItemIdx == size() - 1) {
		getItem(0)->setIsActive(true);
		return;
	}

	// backward direction and currentkly active item is first
	if (!direction && activeItemIdx == 0) {
		getItem(size() - 1)->setIsActive(true);
		return;
	}

	uint16_t newActiveIndex = direction ? activeItemIdx + 1 : activeItemIdx - 1;
	getItem(newActiveIndex)->setIsActive(true);
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
