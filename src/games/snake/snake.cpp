/*#include "snake.h"

// Positions
static Vector2i_8 snake[SNAKE_MAX_LENGTH];
static Vector2i_8 currentDirection;
static Vector2i_8 nextDirection;
static Vector2i_8 applePosition;

// Integer
static uint16_t gameScore;
static uint8_t headIdx;
static uint8_t tailIdx;

// Boolean
static bool joyStickMovedX;
static bool joyStickMovedY;
static bool appleIsWhite;
static bool needsRedraw;

// Reset values
static void prepareSnake()
{
    // Set default values
    memset(snake, 0, sizeof(snake));

    currentDirection = {1, 0};
    nextDirection = {1, 0};
    applePosition = {
        ((SNAKE_GRID_WIDTH >> 1) + 4),
        (SNAKE_GRID_HEIGHT >> 1)};

    joyStickMovedX = false;
    joyStickMovedY = false;
    needsRedraw = false;
    appleIsWhite = true;

    headIdx = 0;
    tailIdx = 0;

    // Clear menu screen
    display.clearDisplay();

    for (uint8_t i = 0; i <= SNAKE_INIT_LENGTH; i++)
    {
        // Create body part
        int8_t posX = (SNAKE_GRID_WIDTH >> 1) + i - SNAKE_INIT_LENGTH;
        int8_t posY = (SNAKE_GRID_HEIGHT >> 1);
        snake[headIdx] = {posX, posY};

        // Draw it
        display.fillRect(
            posX << 2, posY << 2,
            SNAKE_GRID_SIZE, SNAKE_GRID_SIZE,
            WHITE);

        headIdx = (headIdx + 1) & 63;
    }
    headIdx--;

    display.display();
    Vector2i_8 tailPos = snake[0];

    // Clear tail
    display.fillRect(
        tailPos.x << 2, tailPos.y << 2,
        SNAKE_GRID_SIZE, SNAKE_GRID_SIZE,
        BLACK);
}

// Draw
static void draw()
{
    // Unset flag
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

// If snake crashed / reached maximum value
static void onGameOver()
{
    display.clearDisplay();

    // Setup text
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10, 0);

    // Draw score
    display.print(F("Game Over"));
    display.setCursor(10, 20);
    display.print(gameScore);

    // Refresh
    display.display();
    delay(500);
}

// Check collision with (Vector2i_8)
static bool collideWithSnake(Vector2i_8 pos, bool excludeHead)
{
    // Index in array
    int8_t posIndex = tailIdx;

    // Search in ring array
    while (posIndex != headIdx)
    {
        posIndex = (posIndex + 1) & 63;
        Vector2i_8 checkPos = snake[posIndex];

        // If snake part position is requested
        // And is not head(if excludeHead)
        if (checkPos.x == pos.x && checkPos.y == pos.y)
        {
            if (!(excludeHead && posIndex == headIdx))
                return true;
        }
    }

    // If nothing found
    return false;
}

// If snake collides with apple
static void onAppleEaten()
{
    // Find new apple position without intersection
    do
    {
        applePosition.x = random(0, SNAKE_GRID_WIDTH - 1);
        applePosition.y = random(0, SNAKE_GRID_HEIGHT - 1);
    } while (collideWithSnake(applePosition, false));

    // Tail stays in place if eaten
    // (length ++)
    tailIdx = (tailIdx - 1) & 63;
    gameScore++;

    // If no more space in snake[]
    if (tailIdx == ((headIdx + 1) & 63))
    {
        // Maximum score (win)
        gameScore = -1;
        onGameOver();
        return;
    }
}

// User input (change direction, later exit to menu/pause)
static void handleInput(uint16_t joyStickX, uint16_t joyStickY)
{
    // Get boolean values
    bool overTresholdX = (joyStickX < TRESHOLD || joyStickX > CENTER + TRESHOLD);
    bool overTresholdY = (joyStickY < TRESHOLD || joyStickY > CENTER + TRESHOLD);
    bool overTreshold = overTresholdX || overTresholdY;

    // Get direction (not in current, to
    // prevent instant death)
    int8_t nextDirX = joyStickX > CENTER ? -1 : 1;
    int8_t nextDirY = joyStickY > CENTER ? -1 : 1;

    // If moved X
    if (!joyStickMovedX && overTresholdX)
    {
        if (nextDirX != -currentDirection.x)
            nextDirection.x = nextDirX;
    }

    // If moved Y
    if (!joyStickMovedY && overTresholdY)
    {
        if (nextDirY != -currentDirection.y)
            nextDirection.y = nextDirY;
    }

    // Set moved flags, redraw flag
    joyStickMovedX = overTresholdX;
    joyStickMovedY = overTresholdY;
    needsRedraw = overTreshold;
}

// Global function for menu
void startSnake()
{
    // Reset values
    prepareSnake();

    // Last time snake updated
    unsigned long lastFrame = millis();

    // JoyStick values
    uint16_t joyStickX;
    uint16_t joyStickY;

    // Game cycle
    while (true)
    {
        // Get input
        joyStickX = joyStick.getX();
        joyStickY = joyStick.getY();

        handleInput(joyStickX, joyStickY);

        // If past enough time
        if (millis() - lastFrame >= SNAKE_TICK_TIME)
        {
            needsRedraw = true;
            lastFrame = millis();

            // Prevent diagonal moving
            if (nextDirection.x != 0 && nextDirection.y != 0)
            {
                if (currentDirection.x != 0)
                    nextDirection.x = 0;
                else
                    nextDirection.y = 0;
            }

            // Move
            currentDirection = nextDirection;

            // Pointer to set values
            Vector2i_8 *headPosition = &snake[(headIdx + 1) & 63];
            *headPosition = snake[headIdx];

            // Move head
            headPosition->x += currentDirection.x;
            headPosition->y += currentDirection.y;
            headPosition->x &= 31;
            headPosition->y &= 15;

            // Move Idx
            headIdx = (headIdx + 1) & 63;
            tailIdx = (tailIdx + 1) & 63;

            // If crashed
            if (collideWithSnake(*headPosition, true))
            {
                onGameOver();
                return;
            }

            // If collide with apple
            if (collideWithSnake(applePosition, false))
            {
                onAppleEaten();
            }
        }

        // Redraw
        if (needsRedraw)
            draw();
    }
}*/