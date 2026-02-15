#include "flappy.h"
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

    for (uint8_t i = 0; i < FLAPPY_PIPE_COUNT; i++)
    {
        FlappyPipe *pipe = (i == 0 ? &game.firstPipe : &game.secondPipe);

        // Draw pipe
        display.fillRect(
            pipe->x, 0,
            FLAPPY_PIPE_WIDTH, SCREEN_HEIGHT,
            WHITE);

        // Draw gap
        display.fillRect(
            pipe->x, pipe->offsetY,
            FLAPPY_PIPE_GAP_SIZE, FLAPPY_PIPE_GAP_SIZE,
            BLACK);
    }

    // Draw bird
    display.fillRect(
        FLAPPY_BIRD_X, (game.birdPositionY >> FLAPPY_DP_COUNT),
        FLAPPY_BIRD_SIZE, FLAPPY_BIRD_SIZE,
        WHITE);

    // Draw score
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);

    display.print(global.gameScore);

    display.display();
}

static bool collideWithPipe(FlappyPipe *pipe)
{
    int16_t shiftedY = (game.birdPositionY >> FLAPPY_DP_COUNT);

    if (pipe->x + FLAPPY_PIPE_WIDTH < FLAPPY_BIRD_X || pipe->x - FLAPPY_BIRD_SIZE > FLAPPY_BIRD_X)
    {
        return false;
    }

    if (shiftedY >= pipe->offsetY &&
        shiftedY <= pipe->offsetY + FLAPPY_PIPE_GAP_SIZE - FLAPPY_BIRD_SIZE)
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

    for (uint8_t i = 0; i < FLAPPY_PIPE_COUNT; i++)
    {
        FlappyPipe *pipe = (i == 0 ? &game.firstPipe : &game.secondPipe);
        pipe->claimed = false;
        pipe->x = SCREEN_WIDTH + (i * FLAPPY_PIPE_DISTANCE);
        pipe->offsetY = random(FLAPPY_PIPE_MIN_Y, FLAPPY_PIPE_MAX_Y);
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
                FLAPPY_BIRD_X, game.birdPositionY >> FLAPPY_DP_COUNT,
                FLAPPY_BIRD_SIZE, FLAPPY_BIRD_SIZE,
                BLACK);

            game.birdPositionY += (game.birdVelocityY);
            game.birdVelocityY += FLAPPY_FALL_SPEED;

            uint8_t birdPosY = (game.birdPositionY >> FLAPPY_DP_COUNT);

            if (birdPosY > SCREEN_HEIGHT - FLAPPY_BIRD_SIZE)
            {
                onGameOver();
                return;
            }

            for (uint8_t pipeId = 0; pipeId < FLAPPY_PIPE_COUNT; pipeId++)
            {
                FlappyPipe *pipe = (pipeId == 0 ? &game.firstPipe : &game.secondPipe);
                display.fillRect(
                    pipe->x, 0,
                    FLAPPY_PIPE_WIDTH, SCREEN_HEIGHT,
                    BLACK);
                pipe->x -= FLAPPY_PIPE_SPEED;

                if (pipe->x >= 0xF0)
                {
                    pipe->claimed = false;
                    pipe->x = SCREEN_WIDTH;
                    pipe->offsetY = random(FLAPPY_PIPE_MIN_Y, FLAPPY_PIPE_MAX_Y);
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
