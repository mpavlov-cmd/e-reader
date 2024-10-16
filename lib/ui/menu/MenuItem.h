#ifndef MENUITEM_H
#define MENUITEM_H

#include <Arduino.h>

struct MenuItem
{
    private:
        const uint16_t id;
        const char* name;
        bool isActive = false;

    public:
        MenuItem(const uint16_t mId, const char* mName);
        MenuItem();

        uint16_t getId();
        const char* getName();
        bool getIsActive();
        void setIsActive(bool active);
};


#endif