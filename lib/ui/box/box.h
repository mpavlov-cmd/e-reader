#ifndef BOX_H
#define BOX_H

#include <Arduino.h>

struct Box
{
    private:
        const uint16_t topLeftCornerX;
        const uint16_t topLeftCornerY;
        const uint8_t width;
        const uint8_t height;
        const uint8_t padding;
        const uint8_t border;

    public:
        Box(
            const uint16_t topLeftCornerX,
            const uint16_t topLeftCornerY,
            const uint8_t width,
            const uint8_t height, 
            const uint8_t padding,
            const uint8_t border
        );

        uint16_t x(); 
        uint16_t y(); 
        uint8_t wdth();
        uint8_t hght();
        uint8_t pad();
        uint8_t brd();
};


#endif