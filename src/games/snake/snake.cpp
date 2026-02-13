#include "snake.h"

static Vector2i_8 snake[SNAKE_MAX_LENGTH];
static Vector2i_8 currentDirection;
static Vector2i_8 nextDirection;
static Vector2i_8 applePosition;

static uint16_t gameScore;
static uint8_t headIdx;
static uint8_t tailIdx;

static bool joyStickMovedX;
static bool joyStickMovedY;
static bool appleIsWhite;
static bool needsRedraw;

static void prepareSnake()
{
    // Set default values
    memset(snake, 0, sizeof(snake));
    currentDirection = {1, 0};
    nextDirection = {1, 0};
    applePosition = {
        ((SNAKE_GRID_WIDTH >> 1) + 3),
        (SNAKE_GRID_HEIGHT >> 1)};

    joyStickMovedX = false;
    joyStickMovedY = false;
    needsRedraw = false;
    appleIsWhite = true;
    display.clearDisplay();

    headIdx = 0;
    tailIdx = 0;

    for (uint8_t i = 0; i <= SNAKE_INIT_LENGTH; i++)
    {
        // Create body part
        int8_t posX = (SNAKE_GRID_WIDTH >> 1) - i;
        int8_t posY = (SNAKE_GRID_HEIGHT >> 1);
        snake[headIdx] = {posX, posY};

        // Draw it
        display.fillRect(
            posX << 2, posY << 2,
            SNAKE_GRID_SIZE, SNAKE_GRID_SIZE,
            WHITE);

        headIdx++;
    }

    display.display();
    Vector2i_8 tailPos = snake[0];

    // Clear tail
    display.fillRect(
        tailPos.x << 2, tailPos.y << 2,
        SNAKE_GRID_SIZE, SNAKE_GRID_SIZE,
        BLACK);
}

static void draw()
{
    needsRedraw = false;

    // Get positions from array
    Vector2i_8 headPosition = snake[headIdx];
    Vector2i_8 tailPosition = snake[tailIdx];

    // Clear tail
    display.fillRect(
        tailPosition.x << 2, tailPosition.y << 2,
        SNAKE_GRID_SIZE, SNAKE_GRID_SIZE,
        BLACK);

    // Draw head
    display.fillRect(
        headPosition.x << 2, headPosition.y << 2,
        SNAKE_GRID_SIZE, SNAKE_GRID_SIZE,
        WHITE);

    // Draw apple
    display.fillRect(
        applePosition.x << 2, applePosition.y << 2,
        SNAKE_GRID_SIZE, SNAKE_GRID_SIZE,
        appleIsWhite ? WHITE : BLACK);

    display.display();
}

static void onGameOver()
{
    Serial.println(F("Game Over!"));
    delay(500);
}

static bool collideWithSnake(Vector2i_8 pos, bool excludeHead)
{
    int8_t posIndex = tailIdx;

    while (posIndex != headIdx)
    {
        posIndex = (posIndex + 1) & 63;
        Vector2i_8 checkPos = snake[posIndex];

        if (checkPos.x == pos.x && checkPos.y == pos.y)
        {
            if (!(excludeHead && posIndex == headIdx))
                return true;
        }
    }

    return false;
}

static void onAppleEaten()
{
    do
    {
        applePosition.x = random(0, SNAKE_GRID_WIDTH - 1);
        applePosition.y = random(0, SNAKE_GRID_HEIGHT - 1);
    } while (
        collideWithSnake(applePosition, false));

    tailIdx = (tailIdx - 1) & 63;
}

static void handleInput(uint16_t joyStickX, uint16_t joyStickY)
{
    bool overTresholdX = (joyStickX < TRESHOLD || joyStickX > CENTER + TRESHOLD);
    bool overTresholdY = (joyStickY < TRESHOLD || joyStickY > CENTER + TRESHOLD);
    bool overTreshold = overTresholdX || overTresholdY;

    int8_t nextDirX = joyStickX > CENTER ? -1 : 1;
    int8_t nextDirY = joyStickY > CENTER ? -1 : 1;

    if (!joyStickMovedX && overTresholdX)
    {
        if (nextDirX != -currentDirection.x)
            nextDirection.x = nextDirX;
    }
    if (!joyStickMovedY && overTresholdY)
    {
        if (nextDirY != -currentDirection.y)
            nextDirection.y = nextDirY;
    }

    joyStickMovedX = overTresholdX;
    joyStickMovedY = overTresholdY;
    needsRedraw = overTreshold;
}

void startSnake()
{
    prepareSnake();
    Serial.println(--headIdx);

    unsigned long lastFrame = millis();

    uint16_t joyStickX;
    uint16_t joyStickY;

    bool overTresholdX;
    bool overTresholdY;
    bool overTreshold;

    while (true)
    {
        joyStickX = joyStick.getX();
        joyStickY = joyStick.getY();

        handleInput(joyStickX, joyStickY);

        if (millis() - lastFrame >= SNAKE_TICK_TIME)
        {
            needsRedraw = true;
            lastFrame = millis();

            if (nextDirection.x != 0 && nextDirection.y != 0)
            {
                if (currentDirection.x != 0)
                    nextDirection.x = 0;
                else
                    nextDirection.y = 0;
            }

            currentDirection = nextDirection;

            Vector2i_8 *headPosition = &snake[(headIdx + 1) & 63];
            *headPosition = snake[headIdx];
            headPosition->x += currentDirection.x;
            headPosition->y += currentDirection.y;
            headPosition->x &= 31;
            headPosition->y &= 15;

            headIdx = (headIdx + 1) & 63;
            tailIdx = (tailIdx + 1) & 63;

            if (collideWithSnake(*headPosition, true))
            {
                onGameOver();
                return;
            }

            if (collideWithSnake(applePosition, false))
            {
                onAppleEaten();
            }
        }
        if (needsRedraw)
            draw();
    }
}