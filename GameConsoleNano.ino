#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "utils/config.h"
#include "utils/menu/menu.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
JoyStick joyStick;

void setup()
{

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        for (;;)
            ;
    }

    Serial.begin(9600);

    pinMode(PIN_BUZ, OUTPUT);
    pinMode(PIN_HOR, INPUT);
    pinMode(PIN_VER, INPUT);
    pinMode(PIN_SEL, INPUT);

    digitalWrite(PIN_SEL, HIGH);

    tone(PIN_BUZ, 300, 300);
    noTone(PIN_BUZ);

    display.display();
    delay(1000);
}

void loop()
{
    startMenu();
    delay(1000);
}
