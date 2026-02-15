#include "menu.h"

// Include all games, apps
#include "games/tetris/tetris.h"
#include "games/flappy/flappy.h"
#include "games/snake/snake.h"

extern GlobalData globalData;
#define global globalData

// On button press
static void startGame()
{
    // randomSeed(analogRead(A3));  // Work only on real hardware
    randomSeed(micros());
    switch (global.gameScore)
    {
    case 0:
        startFlappy();
        break;
    case 1:
        startSnake();
        break;
    case 2:
        startTetris();
        break;
    }
}

static void draw()
{
    global.needRedraw = false;
    display.clearDisplay();

    display.fillRect(
        0, 0,
        SCREEN_WIDTH, (1 << 5) - (1 << 3),
        WHITE);

    for (uint8_t i = 0; i < PAGE_SIZE; i++)
    {
        MenuValueString curStr = (char *)pgm_read_word(&(menuValues[global.gameScore + i]));

        display.setCursor(10, 1 + (i << 5) - (i << 3));
        display.setTextColor(i == 0 ? BLACK : WHITE);
        display.setTextSize(2);

        for (uint8_t i = 0; i < PAGE_SIZE; i++)
        {
            if (global.gameScore + i >= MENU_LENGTH)
                break;

            MenuValueString curStr = (char *)pgm_read_word(&(menuValues[global.gameScore + i]));
            display.setCursor(10, 1 + (i << 5) - (i << 3));
            display.setTextColor(i == 0 ? BLACK : WHITE);
            display.setTextSize(2);

            display.print(curStr);
        }
    }
    display.display();
}

static void handleInput()
{
    if (!global.joyStickData.wasMovedY && global.joyStickData.isOverTresholdY)
    {
        global.needRedraw = true;
        if (global.joyStickData.isOverCenterY)
        {
            if (global.gameScore != 0)
                global.gameScore--;
            else
                global.gameScore = MENU_LENGTH - 1;
        }
        else
        {
            if (global.gameScore < MENU_LENGTH - 1)
                global.gameScore++;
            else
                global.gameScore = 0;
        }
    }
    global.joyStickData.wasMovedY = global.joyStickData.isOverTresholdY;
}

void startMenu()
{
    global.needRedraw = true;
    global.gameScore = 0;

    uint16_t joyStickY;

    while (true)
    {
        // Get user input
        joyStickY = joyStick.getY();
        global.joyStickData.isOverTresholdY = (joyStickY > CENTER + TRESHOLD ||
                                               joyStickY < CENTER - TRESHOLD);
        global.joyStickData.isOverCenterY = joyStickY > CENTER;
        global.joyStickData.isButtonPressed = joyStick.isButtonPressed();
        handleInput();

        if (global.joyStickData.isButtonPressed)
        {
            while (global.joyStickData.isButtonPressed)
            {
                global.joyStickData.isButtonPressed = joyStick.isButtonPressed();
                delay(10);
            }
            startGame();
            return;
        }

        if (global.needRedraw)
            draw();
    }
}
