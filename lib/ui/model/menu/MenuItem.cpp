#include "MenuItem.h"

MenuItem::MenuItem(const uint16_t mId, const char *mName, bool mIsActive = false): id(mId), name(mName), isActive(mIsActive)
{
}


uint16_t MenuItem::getId()
{
    return id;
}

const char* MenuItem::getName()
{
    return name;
}

bool MenuItem::getIsActive()
{
    return isActive;
}

void MenuItem::setIsActive(bool active)
{
    isActive = active;
}
