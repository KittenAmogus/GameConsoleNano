#include "utils/config.h"

#ifndef JOYSTICK_H
#define JOYSTICK_H

#define TRESHOLD 200
#define CENTER 512

typedef struct JoyStick
{

    bool isButtonPressed()
    {
        return !digitalRead(PIN_SEL);
    }

    uint16_t getX()
    {
        return analogRead(PIN_HOR);
    }

    uint16_t getY()
    {
        return analogRead(PIN_VER);
    }

} JoyStick;

extern JoyStick joyStick;

#endif JOYSTICK_H
