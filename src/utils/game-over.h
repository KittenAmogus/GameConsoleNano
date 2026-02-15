#include <Arduino.h>

#include "utils/memory.h"
#include "utils/display.h"

extern Adafruit_SSD1306 display;
extern GlobalData globalData;

#ifndef GAME_OVER_H
#define GAME_OVER_H

static void onGameOver()
{
    delay(250);
    display.clearDisplay();

    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(10, 20);
    display.print(F("SCORE: "));
    display.print(globalData.gameScore);

    display.display();
    delay(500);
}

#endif
