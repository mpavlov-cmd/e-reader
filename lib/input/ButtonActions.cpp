#include "ButtonActions.h"

uint8_t controlDirection(uint8_t input, bool &held)
{
    // Chhek of hold bit is set
    held = input & B00010000;

    // return input without hold bit
    return input & ~B00010000;;
}