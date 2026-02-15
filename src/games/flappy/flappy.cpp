#include "flappy.h"

extern GlobalData globalData;
extern GameData gameData;
#define global globalData
#define game gameData.flappyData

static void prepareFlappy()
{
    memset(&game, 0, sizeof(game));
    global.gameScore = 0;
    global.needRedraw = true;
    game.birdPositionY = (SCREEN_HEIGHT >> 1) << FLAPPY_DP_COUNT;
    display.clearDisplay();
}

static void draw()
{
    global.needRedraw = false;

    // Clear last score
    display.fillRect(
        0, 0,
        36, 16,
        BLACK);

    for (uint8_t i = 0; i < PIPE_COUNT; i++)
    {
        FlappyPipe *pipe = (i == 0 ? &game.firstPipe : &game.secondPipe);

        // Draw pipe
        display.fillRect(
            pipe->x, 0,
            PIPE_WIDTH, SCREEN_HEIGHT,
            WHITE);

        // Draw gap
        display.fillRect(
            pipe->x, pipe->offsetY,
            PIPE_GAP_SIZE, PIPE_GAP_SIZE,
            BLACK);
    }

    // Draw bird
    display.fillRect(
        BIRD_X, (game.birdPositionY >> FLAPPY_DP_COUNT),
        BIRD_SIZE, BIRD_SIZE,
        WHITE);

    // Draw score
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);

    display.print(global.gameScore);

    display.display();
}

static void onGameOver()
{
    draw();
    delay(250);
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.print(F("SCORE: "));
    display.print(global.gameScore);

    display.display();
    delay(500);
}

static bool collideWithPipe(FlappyPipe *pipe)
{
    int16_t shiftedY = (game.birdPositionY >> FLAPPY_DP_COUNT);

    if (pipe->x + PIPE_WIDTH < BIRD_X || pipe->x - BIRD_SIZE > BIRD_X)
    {
        return false;
    }

    if (shiftedY >= pipe->offsetY &&
        shiftedY <= pipe->offsetY + PIPE_GAP_SIZE - BIRD_SIZE)
    {
        if (!pipe->claimed)
        {
            global.gameScore++;
            pipe->claimed = true;
        }
        return false;
    }

    return true;
}

static void handleInput()
{
    if (!global.joyStickData.wasMovedY && global.joyStickData.isOverTresholdY)
    {
        game.birdVelocityY = -FLAPPY_JUMP_SPEED;
    }
    global.joyStickData.wasMovedY = global.joyStickData.isOverTresholdY;
}

void startFlappy()
{
    prepareFlappy();

    for (uint8_t i = 0; i < PIPE_COUNT; i++)
    {
        FlappyPipe *pipe = (i == 0 ? &game.firstPipe : &game.secondPipe);
        pipe->claimed = false;
        pipe->x = SCREEN_WIDTH + (i * PIPE_DISTANCE);
        pipe->offsetY = random(PIPE_MIN_Y, PIPE_MAX_Y);
    }

    uint16_t joyStickY;

    while (true)
    {
        // Get user input
        joyStickY = joyStick.getY();
        global.joyStickData.isOverTresholdY = (joyStickY > CENTER + TRESHOLD ||
                                               joyStickY < CENTER - TRESHOLD);
        global.joyStickData.isOverCenterY = joyStickY > CENTER;
        handleInput();

        global.currFrame = millis();
        if (global.currFrame - global.lastFrame > FLAPPY_TICK_TIME)
        {
            global.lastFrame = global.currFrame;
            global.needRedraw = true;

            // Clear bird
            display.fillRect(
                BIRD_X, game.birdPositionY >> FLAPPY_DP_COUNT,
                BIRD_SIZE, BIRD_SIZE,
                BLACK);

            game.birdPositionY += (game.birdVelocityY);
            game.birdVelocityY += FLAPPY_FALL_SPEED;

            uint8_t birdPosY = (game.birdPositionY >> FLAPPY_DP_COUNT);

            if (birdPosY > SCREEN_HEIGHT - BIRD_SIZE)
            {
                onGameOver();
                return;
            }

            for (uint8_t pipeId = 0; pipeId < PIPE_COUNT; pipeId++)
            {
                FlappyPipe *pipe = (pipeId == 0 ? &game.firstPipe : &game.secondPipe);
                display.fillRect(
                    pipe->x, 0,
                    PIPE_WIDTH, SCREEN_HEIGHT,
                    BLACK);
                pipe->x -= PIPE_SPEED;

                if (pipe->x >= 0xF0)
                {
                    pipe->claimed = false;
                    pipe->x = SCREEN_WIDTH;
                    pipe->offsetY = random(PIPE_MIN_Y, PIPE_MAX_Y);
                }
                else if (collideWithPipe(pipe))
                {
                    onGameOver();
                    return;
                }
            }
        }

        if (global.needRedraw)
            draw();
    }
}
