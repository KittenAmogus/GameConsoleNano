#ifndef FLAPPY_H
#define FLAPPY_H

#include <Arduino.h>

#include "utils/memory.h"
#include "utils/display.h"
#include "utils/joystick.h"
#include "utils/game-over.h"

extern GameData gameData;
extern GlobalData globalData;

#define global globalData
#define game gameData.flappyData

// Speed
#define FLAPPY_PIPE_SPEED 3
#define FLAPPY_FALL_SPEED 33
#define FLAPPY_JUMP_SPEED 333

// Count, size
#define FLAPPY_BIRD_SIZE 4
#define FLAPPY_PIPE_COUNT 2
#define FLAPPY_PIPE_WIDTH 8
#define FLAPPY_PIPE_GAP_SIZE 20
#define FLAPPY_PIPE_DISTANCE (SCREEN_WIDTH >> 1)

// Distance from borders
#define FLAPPY_BIRD_X 30
#define FLAPPY_PIPE_MIN_Y 20
#define FLAPPY_PIPE_MAX_Y (SCREEN_HEIGHT - 20)

#define FLAPPY_TICK_TIME 30 // Max: ~30 FPS
#define FLAPPY_DP_COUNT 8   // Decimal Places (00000000 . 00000000)

void startFlappy();

#endif
