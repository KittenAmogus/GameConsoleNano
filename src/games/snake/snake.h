#ifndef SNAKE_H
#define SNAKE_H

#include <Arduino.h>

#include "utils/memory.h"
#include "utils/display.h"
#include "utils/joystick.h"
#include "utils/game-over.h"

extern JoyStick joyStick;
extern Adafruit_SSD1306 display;

extern GameData gameData;
extern GlobalData globalData;

#define global globalData
#define game gameData.snakeData

// Snake
#define SNAKE_LENGTH 32
#define SNAKE_TICK_TIME 100
#define SNAKE_APPLE_TICK_TIME 150

// Grid
#define SNAKE_GRID_SIZE 8
#define SNAKE_GRID_WIDTH (SCREEN_WIDTH >> 3)
#define SNAKE_GRID_HEIGHT (SCREEN_HEIGHT >> 3)
#define SNAKE_CENTERX (SNAKE_GRID_WIDTH >> 1)
#define SNAKE_CENTERY (SNAKE_GRID_WIDTH >> 1)

#define SNAKE_INITIAL_LENGTH 3

#define START_DIRECTION {1, 0}

void startSnake();

#endif
