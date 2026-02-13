#include "snake.h"

Vector2i_8 snake[SNAKE_MAX_LENGTH];
Vector2i_8 currentDirection = {1, 0};
Vector2i_8 lastDirection = {1, 0};
Vector2i_8 applePos;

uint8_t headIdx;
uint8_t tailIdx;

bool needRedraw = false;
bool changed = false;
bool justSpawned = true;
uint16_t score;

unsigned long lastFrame;

static void draw()
{
    // display.clearDisplay();
    needRedraw = false;

    Vector2i_8 headPos = snake[(headIdx - 1) & 63];
    Vector2i_8 tailPos = snake[(tailIdx - 2) & 63];

    display.fillRect(tailPos.x << 2, tailPos.y << 2, SNAKE_GRID_SIZE, SNAKE_GRID_SIZE, BLACK);
    display.fillRect(headPos.x << 2, headPos.y << 2, SNAKE_GRID_SIZE, SNAKE_GRID_SIZE, WHITE);

    display.fillRect(applePos.x << 2, applePos.y << 2, SNAKE_GRID_SIZE, SNAKE_GRID_SIZE, WHITE);

    display.display();
}

static void gameOver()
{
    draw();
    delay(1000);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.print("Game Over");
    display.display();
}

static bool collideWithSnake(int16_t x, int16_t y, bool excludeHead)
{

    int8_t i = (tailIdx - 1) & 63;
    int8_t excl = (headIdx - 1) & 63;

    while (i != headIdx)
    {
        i = (i + 1) & 63;
        Vector2i_8 pos = snake[i];
        if (pos.x == x && pos.y == y)
        {
            if (excludeHead && i == excl)
                continue;
            else
            {
                return true;
            }
        }
    }
    return false;
}

static void handleInput(bool overX, bool overZeroX, bool overY, bool overZeroY)
{
    if (overX)
    {
        int16_t newPos = overZeroX ? -1 : 1;
        if (newPos != -lastDirection.x)
        {
            currentDirection.x = newPos;
            currentDirection.y = 0;
        }
    }
    else if (overY)
    {
        int16_t newPos = overZeroY ? -1 : 1;
        if (newPos != -lastDirection.y)
        {
            currentDirection.x = 0;
            currentDirection.y = newPos;
        }
    }
}

void startSnake()
{
    memset(snake, 0, sizeof(snake));
    currentDirection = {1, 0};
    headIdx = 0;
    tailIdx = 0;

    changed = false;
    justSpawned = true;
    needRedraw = true;

    applePos = {(SNAKE_GRID_WIDTH >> 1) + 3, (SNAKE_GRID_HEIGHT >> 1)};

    display.clearDisplay();
    for (uint8_t i = 0; i < SNAKE_INIT_LENGTH; i++)
    {
        int8_t posX = (16) - i;
        int8_t posY = (8);
        snake[headIdx++] = {posX, posY};
        display.fillRect(posX << 2, posY << 2, SNAKE_GRID_SIZE, SNAKE_GRID_SIZE, WHITE);
    }
    display.display();

    lastFrame = millis();

    bool joyStickMoved = false;
    int16_t joyY;
    int16_t joyX;

    bool overTresholdY;
    bool overTresholdX;
    bool overTreshold;

    while (true)
    {
        joyX = joyStick.getX() - CENTER;
        joyY = joyStick.getY() - CENTER;

        overTresholdY = (joyY > TRESHOLD) || (joyY < -TRESHOLD);
        overTresholdX = (joyX > TRESHOLD) || (joyX < -TRESHOLD);
        overTreshold = overTresholdX || overTresholdY;

        if (!joyStickMoved && overTreshold)
        {
            needRedraw = true;
            if (!changed)
            {
                handleInput(overTresholdX, joyX > 0, overTresholdY, joyY > 0);
                changed = true;
            }
        }
        joyStickMoved = overTreshold;

        if (millis() - lastFrame >= SNAKE_TICK_TIME)
        {
            lastDirection.x = currentDirection.x;
            lastDirection.y = currentDirection.y;
            changed = false;
            Vector2i_8 *headPos = &snake[headIdx];
            *headPos = snake[(headIdx - 1) & 63];
            headPos->x += currentDirection.x;
            headPos->y += currentDirection.y;
            headPos->x &= 31;
            headPos->y &= 15;
            lastFrame = millis();
            needRedraw = true;

            headIdx = (headIdx + 1) & 63;

            if (justSpawned)
                justSpawned = false;
            else if (collideWithSnake(headPos->x, headPos->y, true))
            {
                gameOver();
                return;
                // Serial.println("TOUCH HEAD");
            }

            if (!collideWithSnake(applePos.x, applePos.y, false))
            {
                tailIdx = (tailIdx + 1) & 63;
            }
            else
            {
                do
                {
                    applePos.x = random(0, SNAKE_GRID_WIDTH - 1);
                    applePos.y = random(0, SNAKE_GRID_HEIGHT - 1);
                } while (collideWithSnake(applePos.x, applePos.y, false));
                needRedraw = true;
            }
        }

        // Will never happen, i know
        if (headIdx == tailIdx)
            return;

        if (needRedraw)
            draw();
    }
}
