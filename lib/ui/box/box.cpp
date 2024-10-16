#include "Box.h"

Box::Box(
    const int16_t cX,
    const int16_t cY,
    const uint16_t boxWidth,
    const uint16_t boxHeight, 
    const uint8_t boxPadding,
    const uint8_t boxBorder
) : x(cX), y(cY), width(boxWidth), height(boxHeight), padding(boxPadding), border(boxBorder)
{
}

int16_t Box::getX()
{
    return x;
}

int16_t Box::getY()
{
    return y;
}

uint16_t Box::getWidth()
{
    return width;
}

uint16_t Box::getHeight()
{
    return height;
}

uint8_t Box::getPadding()
{
    return padding;
}

uint8_t Box::getBorder()
{
    return border;
}
