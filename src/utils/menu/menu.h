#include <Arduino.h>
#include <avr/pgmspace.h>

#include "utils/memory.h"
#include "utils/display.h"
#include "utils/joystick.h"

#ifndef MENU_H
#define MENU_H

// Just string type
typedef char *MenuValueString;

// Game names
const MenuValueString menuValue1 PROGMEM = "Flappy";
const MenuValueString menuValue2 PROGMEM = "Snake ";
const MenuValueString menuValue3 PROGMEM = "Tetris";
const MenuValueString menuValue4 PROGMEM = "3DMaze";
const MenuValueString menuValue5 PROGMEM = "Balls ";
const MenuValueString menuValue6 PROGMEM = "CarSim";
const MenuValueString menuValue7 PROGMEM = "Shoot ";

// Game count
#define MENU_LENGTH 7

// Game count in one page
#define PAGE_SIZE 3
#define PAGE_COUNT = (MENU_LENGTH / PAGE_SIZE + (MENU_LENGTH << 15 != 0));

// Main menu values
const MenuValueString menuValues[MENU_LENGTH] PROGMEM =
    {menuValue1,
     menuValue2,
     menuValue3,
     menuValue4,
     menuValue5,
     menuValue6,
     menuValue7};

void startMenu();

#endif // MENU_H
