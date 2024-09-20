#include "box.h"

Box::Box(
    const uint16_t topLeftCornerX,
    const uint16_t topLeftCornerY,
    const uint8_t width,
    const uint8_t height,
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

uint16_t Box::x()
{
    return 0;
}

uint16_t Box::y()
{
    return 0;
}

uint8_t Box::wdth()
{
    return 0;
}

uint8_t Box::hght()
{
    return 0;
}

uint8_t Box::pad()
{
    return 0;
}

uint8_t Box::brd()
{
    return 0;
}
