#include "snake.h"

// Reset values
static void prepareSnake()
{
    display.clearDisplay();

    // Set default values
    memset(&game, 0, sizeof(game));

    global.gameScore = 0;
    global.needRedraw = false;

    game.applePos = {SNAKE_CENTERX + 3, SNAKE_CENTERY};

    for (int8_t i = SNAKE_INITIAL_LENGTH; i >= 0; i--)
    {
        uint8_t posX, posY;
        posX = SNAKE_CENTERX - i;
        game.snake[game.headIdx++] = {posX, SNAKE_CENTERY};

        // Draw
        display.fillRect(
            posX << 3, SNAKE_CENTERY << 3,
            SNAKE_GRID_SIZE, SNAKE_GRID_SIZE,
            WHITE);
    }

    // game.headIdx = SNAKE_INITIAL_LENGTH - 1;
    display.display();

    game.curDirectionX = 1;
    game.nextDirectionX = 1;
}

// Draw
static void draw()
{
    // Unset flag
    global.needRedraw = false;

    SnakePos headPos = game.snake[game.headIdx];
    SnakePos tailPos = game.snake[game.tailIdx];

    display.fillRect(
        headPos.x << 3, headPos.y << 3,
        SNAKE_GRID_SIZE, SNAKE_GRID_SIZE,
        WHITE);

    display.fillRect(
        game.applePos.x << 3, game.applePos.y << 3,
        SNAKE_GRID_SIZE, SNAKE_GRID_SIZE,
        BLACK);

    display.fillRect(
        (game.applePos.x << 3) + (game.isAppleBig), (game.applePos.y << 3) + (game.isAppleBig),
        SNAKE_GRID_SIZE - (game.isAppleBig << 1), SNAKE_GRID_SIZE - (game.isAppleBig << 1),
        WHITE);

    // DEBUG_PRINTLN(F("DRAWED"));

    display.display();
}

// Check collision with (Vector2i_8)

static bool collideWithSnake(SnakePos pos, bool excludeHead)
{
    uint8_t i = game.tailIdx;
    while (i != game.headIdx)
    {
        if (game.snake[i].x == pos.x && game.snake[i].y == pos.y)
            return true;
        i = (i + 1) & (SNAKE_LENGTH - 1);
    }

    if (!excludeHead && game.snake[game.headIdx].x == pos.x && game.snake[game.headIdx].y == pos.y)
        return true;
    return false;
}

// If snake collides with apple
static void onAppleEaten()
{
    // Find new apple position without intersection
    do
    {
        game.applePos.x = random(0, SNAKE_GRID_WIDTH - 1);
        game.applePos.y = random(0, SNAKE_GRID_HEIGHT - 1);
    } while (collideWithSnake(game.applePos, false));

    // Tail stays in place if eaten
    // (length ++)
    game.tailIdx--;
    global.gameScore++;

    // If no more space in snake[]
    if (game.tailIdx == ((game.headIdx + 1) & (SNAKE_LENGTH - 1)))
    {
        // Maximum score (win)
        global.gameScore = -1;
        onGameOver();
        return;
    }
}

// User input (change direction, later exit to menu/pause)
static void handleInput()
{
    if (!global.joyStickData.wasMovedX && global.joyStickData.isOverTresholdX)
    {
        if (game.curDirectionX == 0)
        {
            game.nextDirectionY = 0;
            game.nextDirectionX = global.joyStickData.isOverCenterX ? -1 : 1;
            global.needRedraw = true;
        }
    }

    else if (!global.joyStickData.wasMovedY && global.joyStickData.isOverTresholdY)
    {

        if (game.curDirectionY == 0)
        {
            game.nextDirectionX = 0;
            game.nextDirectionY = global.joyStickData.isOverCenterY ? -1 : 1;
            global.needRedraw = true;
        }
    }

    global.joyStickData.wasMovedX = global.joyStickData.isOverTresholdX;
    global.joyStickData.wasMovedY = global.joyStickData.isOverTresholdY;
}

// Global function for menu
void startSnake()
{
    // Reset values
    prepareSnake();

    // JoyStick values
    uint16_t joyStickX;
    uint16_t joyStickY;

    // Game cycle
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

        if (global.currFrame - global.lastFrame >= SNAKE_TICK_TIME)
        {
            global.needRedraw = true;
            global.lastFrame = global.currFrame;

            game.isAppleBig = !game.isAppleBig;

            // Prevent diagonal moving
            if (game.nextDirectionX != 0 && game.nextDirectionY != 0)
            {
                if (game.curDirectionX != 0)
                    game.nextDirectionX = 0;
                else
                    game.nextDirectionY = 0;
            }

            // Move
            game.curDirectionX = game.nextDirectionX;
            game.curDirectionY = game.nextDirectionY;

            // Pointer to set values
            SnakePos *headPosition = &game.snake[(game.headIdx + 1) & (SNAKE_LENGTH - 1)];
            SnakePos *tailPosition = &game.snake[(game.tailIdx)];

            SnakePos nextHeadPos = game.snake[game.headIdx];
            nextHeadPos.x += game.curDirectionX;
            nextHeadPos.y += game.curDirectionY;

            display.fillRect(
                tailPosition->x << 3, tailPosition->y << 3,
                SNAKE_GRID_SIZE, SNAKE_GRID_SIZE,
                BLACK);

            // If crashed
            if (collideWithSnake(nextHeadPos, true))
            {
                onGameOver();
                return;
            }

            // Move head
            *headPosition = game.snake[game.headIdx];
            headPosition->x += game.curDirectionX;
            headPosition->y += game.curDirectionY;

            // Move Idx
            game.headIdx++;
            game.tailIdx++;

            // If collide with apple
            if (collideWithSnake(game.applePos, false))
            {
                onAppleEaten();
            }
        }

        // Redraw
        if (global.needRedraw)
            draw();
    }
}
