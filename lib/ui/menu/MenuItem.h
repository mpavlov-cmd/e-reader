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
        // Primary 
        MenuItem(const uint16_t mId, const char* mName, bool mIsActive);

        // Secondary
        MenuItem(const uint16_t mId, const char* mName): MenuItem(mId, mName, false) {};
        MenuItem(const MenuItem& other) : MenuItem(other.id, other.name, other.isActive) {}; 

        ~MenuItem() {};

        uint16_t getId();
        const char* getName();
        bool getIsActive();
        void setIsActive(bool active);
};


#endif