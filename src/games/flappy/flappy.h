#ifndef FLAPPY_H
#define FLAPPY_H

// Integer defines, pin defines
#include <Arduino.h>

// Some utils, idk
#include "utils/joystick.h"
#include "utils/display.h"
#include "utils/memory.h"
// #include "utils/vector.h"

// Speed
#define PIPE_SPEED 3
#define FLAPPY_FALL_SPEED 33
#define FLAPPY_JUMP_SPEED 333

// Count, size
#define BIRD_SIZE 4
#define PIPE_COUNT 2
#define PIPE_WIDTH 8
#define PIPE_GAP_SIZE 20
#define PIPE_DISTANCE (SCREEN_WIDTH >> 1)

// Distance from borders
#define BIRD_X 30
#define PIPE_MIN_Y 20
#define PIPE_MAX_Y (SCREEN_HEIGHT - 20)

#define FLAPPY_TICK_TIME 30 // ~30 FPS
#define FLAPPY_DP_COUNT 8   // Decimal Places (00000000 . 00000000)

void startFlappy();

#endif
