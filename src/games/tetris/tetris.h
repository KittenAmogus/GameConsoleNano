#ifndef TETRIS_H
#define TETRIS_H

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
#define game gameData.tetrisData

// Time
#define TETRIS_TICK_TIME_START 15 // 64 *
#define TETRIS_TICK_TIME_MIN 2
#define TETRIS_MOVE_X_DELAY 100
#define TETRIS_TICK_FAST 1

// Size
#define TETRIS_GRID_HEIGHT 20
#define TETRIS_GRID_WIDTH 10
#define TETRIS_GRID_SIZE 6

// Offset
#define TETRIS_OFFSET_X 2
#define TETRIS_OFFSET_Y 8

// Redefinition of screen size(screen is rotated)
#define TETRIS_SCREEN_WIDTH SCREEN_HEIGHT
#define TETRIS_SCREEN_HEIGHT SCREEN_WIDTH

const uint16_t SHAPES[7][4] PROGMEM = {
    {0x0F00, 0x2222, 0x00F0, 0x4444}, // I
    {0x4460, 0x0E80, 0xC440, 0x2E00}, // J
    {0x44C0, 0x8E00, 0x6440, 0x0E20}, // L
    // {0x0660, 0x0660, 0x0660, 0x0660},
    {0xCC00, 0xCC00, 0xCC00, 0xCC00}, // O
    {0x06C0, 0x8C40, 0x6C00, 0x4620}, // S
    {0x0E40, 0x4C40, 0x4E00, 0x4640}, // T
    {0x0C60, 0x4C80, 0xC600, 0x2640}  // Z
};

void startTetris();

#endif
