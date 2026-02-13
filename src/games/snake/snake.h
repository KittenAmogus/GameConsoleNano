#ifndef SNAKE_H
#define SNAKE_H

#include <Arduino.h>

#include "utils/joystick.h"
#include "utils/display.h"
#include "utils/vector.h"

#define SNAKE_MAX_LENGTH 64
#define SNAKE_INIT_LENGTH 3

#define SNAKE_GRID_SIZE 4
#define SNAKE_GRID_WIDTH 32
#define SNAKE_GRID_HEIGHT 16

#define SNAKE_TICK_TIME 150

void startSnake();

#endif SNAKE_H
