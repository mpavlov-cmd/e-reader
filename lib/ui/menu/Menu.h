#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <menu/MenuItem.h>
#include <box/Box.h>

struct Menu
{
    private:
        const uint16_t CAPACITY = 2048;

        uint16_t index;
        MenuItem** menuItemsArray;
        
    public:
        Menu();

        /**
         * Add item to the menu
         * 
         * @param pointer to the new MenuItem
         * @return true if added, false if menu at full capacity
         */
        bool addItem(MenuItem* menuItem);

        /**
         * Removes last elemnt from the menu
         * 
         * @returns true of remeoved, false if menu is empty
         */
        bool removeLastItem();

        /**
         * Returns reference to the menu item, so it can be modified
         * 
         * @param idx of the target menu item
         * @returns MenuItem pointer 
         */
        MenuItem* getItem(uint16_t idx);

        /**
         * @returns index of curerntly active item or -1 if no active items in the menu
         */
        int16_t getActiveItemIdx();

        /** 
         * Marks either next or previous menu item as active depending on a direction
         * removes active mark from previously acttive one
         * 
         * @param direction active item will be moved to the bottom if direction 
         */
        void moveActiveItem(uint8_t direction);

        /**
         * Size of menu
         * 
         * @returns number of elements in mens
         */
        uint16_t size();

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