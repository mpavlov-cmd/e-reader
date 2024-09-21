#ifndef BOX_H
#define BOX_H

#include <Arduino.h>

struct Box
{
    private:
        const int16_t topLeftCornerX;
        const int16_t topLeftCornerY;
        const uint16_t width;
        const uint16_t height;
        const uint8_t padding;
        const uint8_t border;

    public:
        Box(
            const int16_t topLeftCornerX,
            const int16_t topLeftCornerY,
            const uint16_t width,
            const uint16_t height, 
            const uint8_t padding,
            const uint8_t border
        );

        int16_t x(); 
        int16_t y(); 
        uint16_t wdth();
        uint16_t hght();
        uint8_t pad();
        uint8_t brd();
};


#endif