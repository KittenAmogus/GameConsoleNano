#include "flappy.h"

// Pipes
Pipe pipes[PIPE_COUNT];

// Integer
static uint16_t gameScore;
static int16_t birdSpeed;
static int16_t birdPositionY;
static int16_t birdPrevPositionY;

// Boolean
static bool needsRedraw;
static bool joyStickMovedX;
static bool joyStickMovedY;

static void prepareFlappy()
{
    display.clearDisplay();

    // Pipe
    memset(pipes, 0, sizeof(pipes));

    // Integer
    birdPositionY = (SCREEN_HEIGHT >> 1) << FLAPPY_DP_COUNT;
    birdPrevPositionY = birdPositionY;
    birdSpeed = 0;
    gameScore = 0;

    // Boolean
    needsRedraw = true;
    joyStickMovedX = false;
    joyStickMovedY = false;
}

static void draw()
{

    // Clear last score
    display.fillRect(
        0, 0,
        36, 16,
        BLACK);

    // Clear bird
    display.fillRect(
        BIRD_X, (birdPrevPositionY >> FLAPPY_DP_COUNT),
        BIRD_SIZE, BIRD_SIZE,
        BLACK);

    for (uint8_t pipeId = 0; pipeId < PIPE_COUNT; pipeId++)
    {
        Pipe *pipe = &pipes[pipeId];

        // Clear "tail"
        display.fillRect(
            pipe->previousX, 0,
            PIPE_WIDTH, SCREEN_HEIGHT,
            BLACK);

        // Draw pipe
        display.fillRect(
            pipe->x, 0,
            PIPE_WIDTH, SCREEN_HEIGHT,
            WHITE);

        // Draw gap
        display.fillRect(
            pipe->x, pipe->offsetFromY,
            PIPE_GAP_SIZE, PIPE_GAP_SIZE,
            BLACK);
    }
    // Draw bird
    display.fillRect(
        BIRD_X, (birdPositionY >> FLAPPY_DP_COUNT),
        BIRD_SIZE, BIRD_SIZE,
        WHITE);

    // Draw score
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);

    display.print(gameScore);

    display.display();
}

static void resetPipe(Pipe *pipe)
{
    pipe->x = SCREEN_WIDTH;
    pipe->offsetFromY = random(PIPE_MIN_Y, PIPE_MAX_Y);
    pipe->reached = false;
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
    display.print(gameScore);

    display.display();
    delay(500);
}

static bool collideWithPipe(Pipe *pipe)
{
    int16_t shiftedY = (birdPositionY >> FLAPPY_DP_COUNT);

    if (pipe->x + PIPE_WIDTH < BIRD_X || pipe->x - BIRD_SIZE > BIRD_X)
    {
        // Serial.println(F("First"));
        return false;
    }

    if (shiftedY >= pipe->offsetFromY &&
        shiftedY <= pipe->offsetFromY + PIPE_GAP_SIZE - BIRD_SIZE)
    {
        gameScore += !pipe->reached;
        pipe->reached = true;
        return false;
    }

    return true;
}

static void handleInput(uint16_t joyStickX, uint16_t joyStickY)
{
    bool overTresholdX = (joyStickX < CENTER - TRESHOLD) || (joyStickX > CENTER + TRESHOLD);
    bool overTresholdY = (joyStickY < CENTER - TRESHOLD) || (joyStickY > CENTER + TRESHOLD);
    bool overZeroX = joyStickX > CENTER;
    bool overZeroY = joyStickY > CENTER;

    if (!joyStickMovedY && overTresholdY)
    {
        birdSpeed = -FLAPPY_JUMP_SPEED;
    }
    joyStickMovedX = overTresholdX;
}

void startFlappy()
{
    prepareFlappy();

    for (uint8_t i = 0; i < PIPE_COUNT; i++)
    {
        Pipe *pipe = &pipes[i];
        resetPipe(pipe);
        pipe->x += SCREEN_WIDTH + (i * PIPE_DISTANCE);
        pipe->previousX = pipe->x;
    }

    uint16_t joyStickX;
    uint16_t joyStickY;

    unsigned long lastFrame = millis();

    while (true)
    {
        joyStickX = joyStick.getX();
        joyStickY = joyStick.getY();

        handleInput(joyStickX, joyStickY);

        if (millis() - lastFrame > FLAPPY_TICK_TIME)
        {
            lastFrame = millis();
            needsRedraw = true;

            birdPrevPositionY = birdPositionY;
            birdPositionY += (birdSpeed);
            birdSpeed += FLAPPY_FALL_SPEED; //  << FLAPPY_DP_COUNT;

            if ((birdPositionY >> FLAPPY_DP_COUNT) < 0 || (birdPositionY >> FLAPPY_DP_COUNT) + BIRD_SIZE > SCREEN_HEIGHT)
            {
                onGameOver();
                return;
            }

            for (uint8_t pipeId = 0; pipeId < PIPE_COUNT; pipeId++)
            {
                Pipe *pipe = &pipes[pipeId];
                pipe->previousX = pipe->x;
                pipe->x -= PIPE_SPEED;

                if (pipe->x < -PIPE_WIDTH)
                    resetPipe(pipe);
                else if (collideWithPipe(pipe))
                {
                    onGameOver();
                    return;
                }
            }
        }

        if (needsRedraw)
            draw();
    }
}
