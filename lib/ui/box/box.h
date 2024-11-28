#ifndef BOX_H
#define BOX_H

#include <Arduino.h>

struct Box
{
        const int16_t x;
        const int16_t y;
        const uint16_t width;
        const uint16_t height;
        const uint8_t padding;
        const uint8_t border;

    public:
        // Default 
        Box(
            const int16_t cX,
            const int16_t cY,
            const uint16_t boxWidth,
            const uint16_t boxHeight, 
            const uint8_t boxPadding,
            const uint8_t boxBorder
        );

        int16_t getX(); 
        int16_t getY(); 
        uint16_t getWidth();
        uint16_t getHeight();
        uint8_t getPadding();
        uint8_t getBorder();
};


#endif