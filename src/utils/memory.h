#ifndef MEMORY_H
#define MEMORY_H

#include <Arduino.h>

// Disable serial
#define DEBUG 0

#if DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_BEGIN(x) Serial.begin(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_BEGIN(x)
#endif

// -- Flappy data
typedef struct FlappyPipe
{
    uint8_t x;
    uint8_t claimed : 1;
    uint8_t offsetY : 7;
} FlappyPipe;

typedef struct FlappyData
{
    uint16_t birdPositionY;
    int16_t birdVelocityY;
    FlappyPipe firstPipe;
    FlappyPipe secondPipe;
} FlappyData;

// -- Snake data
typedef struct SnakeData
{
    uint8_t snake[16];

    uint8_t headIdx : 4;
    uint8_t tailIdx : 4;

    uint8_t isAppleWhite : 1;
    uint8_t isOnRight : 1;
    uint8_t isAppleOnRight : 1;
    uint8_t unusedBits : 5;

    uint8_t applePosX : 4;
    uint8_t applePosY : 4;
} SnakeData;

// -- Tetris data
typedef struct Figure
{
    uint16_t rotateId : 2;
    uint16_t offsetX : 4;
    uint16_t offsetY : 5;
    uint16_t type : 3;
    uint16_t unused : 2;
} Figure;

typedef struct TetrisData
{
    uint8_t gameField[25];
    Figure figure;
} TetrisData;

class TetrisHelper
{
public:
    static void setPixel(uint8_t *field, uint8_t x, uint8_t y, bool state)
    {
        if (x >= 10 || y >= 20)
            return;

        int bitIndex = y * 10 + x;
        int bytePos = bitIndex >> 3;  // bitIndex / 8
        int bitPos = bitIndex & 0x07; // bitIndex % 8

        if (state)
            field[bytePos] |= (1 << bitPos);
        else
            field[bytePos] &= ~(1 << bitPos);
    }

    static bool getPixel(const uint8_t *field, uint8_t x, uint8_t y)
    {
        if (x >= 10 || y >= 20)
            return false;

        int bitIndex = y * 10 + x;
        return (field[bitIndex >> 3] >> (bitIndex & 0x07)) & 0x01;
    }

    static void clear(uint8_t *field)
    {
        memset(field, 0, 25);
    }
};

// -- Global structures
typedef union GameData
{
    FlappyData flappyData;
    SnakeData snakeData;
    TetrisData tetrisData;
} GameData;

// -- Non-union global
typedef struct GlobalData
{
    unsigned long lastFrame;
    unsigned long currFrame;
    uint16_t needFullRedraw : 1;
    uint16_t gameScore : 14;
    uint16_t needRedraw : 1;

    struct
    {
        uint8_t isButtonPressed : 1;
        uint8_t wasButtonPressed : 1;

        uint8_t isOverTresholdX : 1;
        uint8_t isOverCenterX : 1;
        uint8_t wasMovedX : 1;

        uint8_t isOverTresholdY : 1;
        uint8_t isOverCenterY : 1;
        uint8_t wasMovedY : 1;
    } joyStickData;
} GlobalData;

#endif
