#include "Box.h"

Box::Box(
    const int16_t topLeftCornerX,
    const int16_t topLeftCornerY,
    const uint16_t width,
    const uint16_t height,
    const uint8_t padding,
    const uint8_t border
) : topLeftCornerX(topLeftCornerX),
    topLeftCornerY(topLeftCornerY),
    width(width),
    height(height),
    padding(padding),
    border(border)
{
    
}

int16_t Box::x()
{
    return topLeftCornerX;
}

int16_t Box::y()
{
    return topLeftCornerY;
}

uint16_t Box::wdth()
{
    return width;
}

uint16_t Box::hght()
{
    return height;
}

uint8_t Box::pad()
{
    return padding;
}

uint8_t Box::brd()
{
    return border;
}
