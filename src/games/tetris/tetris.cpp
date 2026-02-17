#include "tetris.h"

const uint16_t lineScores[5] = {0, 100, 300, 500, 800};

void createFigure()
{
    // Clear previous figure data
    memset(&game.figure, 0, sizeof(game.figure));

    // Create figure
    game.figure.offsetX = (TETRIS_GRID_WIDTH >> 1);
    game.figure.type = random(0, 7);
    game.figure.updatedScore = true;
}

static void prepareTetris()
{
    display.setRotation(3);
    display.clearDisplay();
    memset(&game, 0, sizeof(game));
    global.gameScore = 0;
    global.needRedraw = true;
    global.needFullRedraw = false;
    game.tickTimePower = TETRIS_TICK_TIME_START;
    game.tickTimePrevPower = TETRIS_TICK_TIME_START;
}

inline uint16_t getShapeMask(uint8_t type, uint8_t rotation)
{
    return pgm_read_word(&(SHAPES[type][rotation & 3]));
}

static void drawBlock(uint8_t x, uint8_t y)
{
    x = ((x << 1) + x) << 1;
    y = ((y << 1) + y) << 1;

    x += TETRIS_OFFSET_X;
    y += TETRIS_OFFSET_Y;

    // Full block with padding
    display.fillRect(
        x + 1, y + 1,
        TETRIS_GRID_SIZE - 2, TETRIS_GRID_SIZE - 2,
        WHITE);

    // Hole in center
    display.fillRect(
        x + 2, y + 2,
        TETRIS_GRID_SIZE - 4, TETRIS_GRID_SIZE - 4,
        BLACK);
}

static void drawFull()
{
    global.needFullRedraw = false;
    display.clearDisplay();
    for (uint8_t y = 0; y < TETRIS_GRID_HEIGHT; y++)
    {
        for (uint8_t x = 0; x < TETRIS_GRID_WIDTH; x++)
        {
            if (TetrisHelper::getPixel(game.gameField, x, y))
                drawBlock(x, y);
        }
    }
    display.display();
}

static void drawScore()
{
    game.figure.updatedScore = false;
    display.fillRect(0, 0, SCREEN_WIDTH, TETRIS_OFFSET_Y, BLACK);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.print(global.gameScore);
}

static void draw()
{
    global.needRedraw = false;
    uint16_t mask = getShapeMask(game.figure.type, game.figure.rotateId);
    for (uint8_t i = 0; i < 16; i++)
    {
        if (mask & (0x8000 >> i))
        {
            uint8_t localX = i & 3;
            uint8_t localY = i >> 2;
            localX += game.figure.offsetX;
            localY += game.figure.offsetY;

            if ((localX >= 0 && localX < TETRIS_GRID_WIDTH) && (localY >= 0 && localY < TETRIS_GRID_HEIGHT))
                drawBlock(localX, localY);
        }
    }

    if (game.figure.updatedScore)
        drawScore();

    display.display();
}

bool isLineFull(uint8_t y)
{
    for (uint8_t x = 0; x < 10; x++)
    {
        if (!TetrisHelper::getPixel(game.gameField, x, y))
            return false;
    }
    return true;
}

void clearLine(uint8_t lineY)
{
    display.fillRect(0, (((lineY << 1) + lineY) << 1) + TETRIS_OFFSET_Y, SCREEN_WIDTH, TETRIS_GRID_SIZE, INVERSE);
    global.needFullRedraw = true;
    for (uint8_t x = 0; x < TETRIS_GRID_WIDTH; x++)
    {
        TetrisHelper::setPixel(game.gameField, x, lineY, false);
    }
}

void moveLines()
{
    for (int8_t y = TETRIS_GRID_HEIGHT - 1; y > 0; y--)
    {
        for (uint8_t x = 0; x < TETRIS_GRID_WIDTH; x++)
        {

            bool pixelAbove = TetrisHelper::getPixel(game.gameField, x, y - 1);
            TetrisHelper::setPixel(game.gameField, x, y, pixelAbove);
        }
    }

    for (uint8_t x = 0; x < TETRIS_GRID_WIDTH; x++)
        TetrisHelper::setPixel(game.gameField, x, 0, false);
}

void checkLines()
{
    uint8_t lineCleared = 0;
    int8_t y = TETRIS_GRID_HEIGHT - 1;
    do
    {
        if (isLineFull(y))
        {
            global.needFullRedraw = true;
            clearLine(y);
            moveLines();

            lineCleared++;
        }
        else
        {
            y--;
        }
    } while (y > 0);

    global.gameScore += lineScores[lineCleared];
    game.figure.updatedScore = true;

    if (lineCleared > 0)
    {
        if (lineCleared == 4)
            display.invertDisplay(1);

        display.display();
        global.needFullRedraw = true;

        global.lastFrame = millis();
        while (true)
        {
            if (millis() - global.lastFrame > 300)
                break;

            if (joyStick.getX() < CENTER - TRESHOLD)
                break;
        }

        if (lineCleared == 4)
            display.invertDisplay(0);
    }
}

bool checkCollisionWithField(int8_t nextX, int8_t nextY, uint8_t nextRot)
{
    uint16_t mask = getShapeMask(game.figure.type, nextRot);

    for (uint8_t i = 0; i < 16; i++)
    {
        if (mask & (0x8000 >> i))
        {
            int8_t fx = nextX + (i & 3);  // i % 4
            int8_t fy = nextY + (i >> 2); // i / 4

            if (fy >= TETRIS_GRID_HEIGHT)
                return true;

            if (fy >= 0 && TetrisHelper::getPixel(game.gameField, fx, fy))
            {
                return true;
            }
        }
    }
    return false;
}

bool checkCollisionWithBorders(int8_t nextX, int8_t nextY, uint8_t nextRot)
{
    // Используем nextRot, а не текущий rotateId!
    uint16_t mask = getShapeMask(game.figure.type, nextRot);

    for (uint8_t i = 0; i < 16; i++)
    {
        if (mask & (0x8000 >> i))
        {
            int8_t fx = nextX + (i & 3);  // координата X конкретного блока
            int8_t fy = nextY + (i >> 2); // координата Y конкретного блока

            // Проверка ВСЕХ границ
            if (fx < 0 || fx >= TETRIS_GRID_WIDTH ||
                fy < 0 || fy >= TETRIS_GRID_HEIGHT)
            {
                return true; // Столкновение со стеной или полом
            }
        }
    }
    return false;
}

/*
bool checkCollisionWithBorders(int8_t nextX, int8_t nextY, uint8_t nextRot)
{
    uint16_t mask = getShapeMask(game.figure.type, game.figure.rotateId);

    for (uint8_t i = 0; i < 16; i++)
    {
        if (mask & (0x8000 >> i))
        {
            int8_t fx = nextX + (i & 3);  // i % 4
            int8_t fy = nextY + (i >> 2); // i / 4

            if (fx < 0 || fx >= TETRIS_GRID_WIDTH || fy >= TETRIS_GRID_HEIGHT)
                return true;
        }
    }
    return false;
}
    */

static void clearFigure(uint8_t y)
{
    uint16_t mask = getShapeMask(game.figure.type, game.figure.rotateId);
    for (uint8_t i = 0; i < 16; i++)
    {
        if (mask & (0x8000 >> i))
        {
            uint8_t localX = i & 3;
            uint8_t localY = i >> 2;
            localX += game.figure.offsetX;
            localY += y;
            localX = ((localX << 1) + localX) << 1;
            localY = ((localY << 1) + localY) << 1;

            localX += TETRIS_OFFSET_X;
            localY += TETRIS_OFFSET_Y;

            display.fillRect(
                localX, localY,
                TETRIS_GRID_SIZE, TETRIS_GRID_SIZE,
                BLACK);
        }
    }
}

static void rotateFigure(int8_t rotation)
{
    uint8_t nextRot = (game.figure.rotateId + rotation) & 3;
    // Проверяем, не мешает ли что-то в новой позиции поворота
    if (!checkCollisionWithField(game.figure.offsetX, game.figure.offsetY, nextRot) &&
        !checkCollisionWithBorders(game.figure.offsetX, game.figure.offsetY, nextRot))
    {
        clearFigure(game.figure.offsetY);
        game.figure.rotateId = nextRot;
        global.needRedraw = true;
    }
}

static void handleInput()
{

    if (!global.joyStickData.isOverCenterX && global.joyStickData.isOverTresholdX)
    {
        game.tickTimePower = TETRIS_TICK_FAST;
    }
    else
    {
        game.tickTimePower = game.tickTimePrevPower;
    }

    // X and Y are swapped !
    if (global.joyStickData.isOverTresholdY)
    {
        clearFigure(game.figure.offsetY);
        global.needRedraw = true;
        uint8_t nextX = game.figure.offsetX + (global.joyStickData.isOverCenterY ? 1 : -1);

        if (!(
                checkCollisionWithField(nextX, game.figure.offsetY, game.figure.rotateId) ||
                checkCollisionWithBorders(nextX, game.figure.offsetY, game.figure.rotateId)))

        {
            if (millis() - game.lastMovedXMillis > TETRIS_MOVE_X_DELAY)
            {
                clearFigure(game.figure.offsetY);
                game.figure.offsetX = nextX;
                global.needRedraw = true;
                game.lastMovedXMillis = millis();
            }
        }
    }

    else if (!global.joyStickData.wasMovedX && global.joyStickData.isOverTresholdX)
    {
        if (global.joyStickData.isOverCenterX)
        {
            rotateFigure(1);
        }
        else
        {
        }
    }

    global.joyStickData.wasMovedX = global.joyStickData.isOverTresholdX;
    global.joyStickData.wasMovedY = global.joyStickData.isOverTresholdY;
}

void bakeFigure()
{
    global.gameScore += 20;
    game.figure.updatedScore = true;

    uint16_t mask = getShapeMask(game.figure.type, game.figure.rotateId);

    for (uint8_t i = 0; i < 16; i++)
    {
        if (mask & (0x8000 >> i))
        {
            int8_t fx = game.figure.offsetX + (i & 3);  // i % 4
            int8_t fy = game.figure.offsetY + (i >> 2); // i / 4

            TetrisHelper::setPixel(game.gameField, fx, fy, true);
        }
    }

    checkLines();
}

void startTetris()
{
    prepareTetris();
    createFigure();

    uint16_t joyStickX;
    uint16_t joyStickY;

    game.lastMovedXMillis = millis();
    global.lastFrame = millis();
    global.currFrame = global.lastFrame;

    while (true)
    {
        joyStickX = joyStick.getX();
        global.joyStickData.isOverTresholdX = (joyStickX > CENTER + TRESHOLD ||
                                               joyStickX < CENTER - TRESHOLD);
        global.joyStickData.isOverCenterX = joyStickX > CENTER;

        joyStickY = joyStick.getY();
        global.joyStickData.isOverTresholdY = (joyStickY > CENTER + TRESHOLD ||
                                               joyStickY < CENTER - TRESHOLD);
        global.joyStickData.isOverCenterY = joyStickY > CENTER;

        handleInput();

        global.currFrame = millis();
        if (global.currFrame - global.lastFrame > (game.tickTimePower << 6))
        {
            global.needRedraw = true;
            global.lastFrame = global.currFrame;

            if (checkCollisionWithField(
                    game.figure.offsetX,
                    game.figure.offsetY + 1,
                    game.figure.rotateId))
            {
                bakeFigure();
                global.needFullRedraw = true;
                createFigure();
                if (checkCollisionWithField(game.figure.offsetX, game.figure.offsetY, game.figure.rotateId))
                {
                    display.setRotation(0);
                    onGameOver();
                    return;
                }
            }
            else
            {
                clearFigure(game.figure.offsetY);
                game.figure.offsetY++;

                if (game.tickTimePower == TETRIS_TICK_FAST)
                {
                    global.gameScore++;
                    game.figure.updatedScore = true;
                }
            }
        }

        // Redraw
        if (global.needFullRedraw)
            drawFull();
        else if (global.needRedraw)
            draw();
    }
}
