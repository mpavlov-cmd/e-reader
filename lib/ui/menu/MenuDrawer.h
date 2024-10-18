#ifndef MENUDRAWER_H
#define MENUDRAWER_H

#include <Arduino.h>
#include <menu\Menu.h>
#include <GxEPD2_GFX.h>
#include <GxEPD2_BW.h>

struct MenuDrawer
{
    private:
        GxEPD2_GFX& display;

        int16_t x = 0, y = 0;
        uint16_t width = 0, height = 0;

    public:
        MenuDrawer(GxEPD2_GFX& mDisplay);

        /**
         * Current implementation would re-draw the entire menu every time method is called
         * 
         * @param menu menu object to draw
         * @param box to draw menu at
         * @param fullWindow weather full or partial window should be used
         */
        void drawMenu(Menu& menu, Box& box, bool fullWindow = false);
};


#endif