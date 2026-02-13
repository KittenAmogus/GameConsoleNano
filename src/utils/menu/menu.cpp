#include "menu.h"

// Include all games, apps
#include "games/snake/snake.h"

// For input
static bool needRedraw = true;
static bool joyStickMoved = false;

int8_t menuPage = 0;
int8_t menuSelect = 0;

// On button press
static void startGame()
{
    switch (menuSelect)
    {
    case 1:
        startSnake();
    }
}

// Draw
static void draw()
{
    // Unset flag
    needRedraw = false;
    display.clearDisplay();
    display.setTextSize(2);

    // Draw all strings
    for (uint8_t i = 0; i < 3; i++)
    {
        // Offset in menuValues
        uint16_t iOffset = (menuPage << 1) + menuPage + i;
        if (iOffset >= MENU_LENGTH)
            break;

        // Load from PROGMEM
        MenuValueString curString = (MenuValueString)pgm_read_ptr(&(menuValues[iOffset]));

        // Color depends if is selected
        display.setTextColor(i != menuSelect);
        display.setCursor(20, 2 + i * 20);
        if (i == menuSelect)
        {
            // If selected draw rect over string
            display.fillRect(0, i * 20 + 1, SCREEN_WIDTH, 20, WHITE);
        }
        display.print(curString);
        // Serial.println(curString);
    }
    display.display();
}

// For loop()
void startMenu()
{
    needRedraw = true;

    // For input
    int16_t joyY = 0;
    bool overTresholdY;
    bool changePage;
    bool overZeroY;

    // Menu loop
    while (true)
    {
        // Get input
        joyY = joyStick.getY() - CENTER;
        overTresholdY = joyY > TRESHOLD || joyY < -TRESHOLD;
        overZeroY = joyY > 0;

        // Handle input
        if (joyStick.isButtonPressed())
        {
            startGame();
            return;
        }

        // TODO : Create 'static void handleInput(), clean up code'
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

        // Redraw only if changed select
        if (needRedraw)
        {
            draw();
        }
        // TODO : Add sleep mode
        delay(50);
    }
}
