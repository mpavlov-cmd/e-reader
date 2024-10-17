#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <menu/MenuItem.h>
#include <box/Box.h>

struct Menu
{
    private:
        const uint16_t CAPACITY = 2048;
        const Box box;
        const bool fullWindow;

        uint16_t index;
        MenuItem** menuItemsArray;
        
    public:
        Menu(const Box mBox, const bool mFullWindow);

        bool addItem(MenuItem* menuItem);
        bool removeLastItem();

        /**
         * Returns reference to the menu item, so it can be modified
         * 
         * @param idx of the target menu item
         * @returns MenuItem pointer 
         */
        MenuItem* getItem(uint16_t idx);
        uint16_t size();

        Box getBox();
        bool getFullWindow();

        ~Menu()
        {
            // Destructor to free allocated memory
            for (uint16_t i = 0; i < index; i++)
            {
                delete menuItemsArray[i];
            }
        }
};



#endif