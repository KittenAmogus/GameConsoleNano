#include "menu.h"

#include "games/snake/snake.h"

// #include "utils/power-save/power-save.h"

static bool needRedraw = true;
static bool joyStickMoved = false;

int8_t menuPage = 0;
int8_t menuSelect = 0;

void startGame()
{
    switch (menuSelect)
    {
    case 1:
        startSnake();
    }
}

static void draw()
{
    needRedraw = false;
    display.clearDisplay();
    display.setTextSize(2);
    for (uint8_t i = 0; i < 3; i++)
    {

        uint16_t iOffset = (menuPage << 1) + menuPage + i;
        if (iOffset >= MENU_LENGTH)
            break;

        MenuValueString curString = (MenuValueString)pgm_read_ptr(&(menuValues[iOffset]));
        display.setTextColor(i != menuSelect);
        display.setCursor(20, 2 + i * 20);
        if (i == menuSelect)
        {
            display.fillRect(0, i * 20 + 1, SCREEN_WIDTH, 20, WHITE);
        }
        display.print(curString);
        // Serial.println(curString);
    }
    display.display();
}

void startMenu()
{
    needRedraw = true;

    int16_t joyY = 0;
    bool overTresholdY;
    bool changePage;
    bool overZeroY;

    // Serial.println(PAGE_COUNT);

    while (true)
    {
        joyY = joyStick.getY() - CENTER;
        overTresholdY = joyY > TRESHOLD || joyY < -TRESHOLD;
        overZeroY = joyY > 0;

        if (joyStick.isButtonPressed())
        {
            startGame();
            return;
        }

        if (overTresholdY && !joyStickMoved)
        {
            if (!overZeroY)
            {
                menuSelect++;
                changePage = menuSelect >= PAGE_SIZE;
                changePage |= (menuPage << 1) + menuPage + menuSelect >= MENU_LENGTH;

                if (changePage)
                {
                    menuPage++;
                    menuSelect = 0;
                    if (menuPage >= PAGE_COUNT)
                    {
                        menuPage = 0;
                    }
                }
            }
            else
            {
                menuSelect--;
                changePage = menuSelect < 0;
                if (changePage)
                {
                    menuSelect = PAGE_SIZE - 1;
                    menuPage--;
                    if (menuPage < 0)
                    {
                        menuPage = PAGE_COUNT - 1;
                        changePage = (menuPage << 1) + menuPage + menuSelect >= MENU_LENGTH;
                        while (changePage)
                        {
                            menuSelect--;
                            changePage = (menuPage << 1) + menuPage + menuSelect >= MENU_LENGTH;
                        }
                    }
                }
            }
            needRedraw = true;
        }
        joyStickMoved = overTresholdY;

        if (needRedraw)
        {
            draw();
        }
        // startCpuWaiting(4);
        delay(50);
    }
}
