#ifndef SWITCHINPUTHANDLER_H
#define SWITCHINPUTHANDLER_H

#include <Arduino.h>

/* 
* 0b00000011 - right key press
* 0b00000100 - left key press, 
* 0b00000001 - up key press
* 0b00000010 - down key press, 
* 0b00001000 - mid button press
*/
#define BUTTON_ACTION_UP    B00000001
#define BUTTON_ACTION_DOWN  B00000010
#define BUTTON_ACTION_RIGHT B00000011
#define BUTTON_ACTION_LEFT  B00000100
#define BUTTON_ACTION_MID   B00001000

uint8_t controlDirection(uint8_t input, bool& held);

#endif