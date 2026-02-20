#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#include "utils/config.h"
#include "utils/memory.h"
#include "utils/menu/menu.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
JoyStick joyStick;

GlobalData globalData;
GameData gameData;

void setup() {
  Wire.begin();
  Wire.setClock(400000L);

  // Failed to find display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // TODO : Add alarm
    for (;;)
      ;
  }

  display.setTextWrap(false);

  DEBUG_BEGIN(9600);

  // Set pin modes
  pinMode(PIN_BUZ, OUTPUT);
  pinMode(PIN_HOR, INPUT);
  pinMode(PIN_VER, INPUT);
  pinMode(PIN_SEL, INPUT);

  // Reset button value
  digitalWrite(PIN_SEL, HIGH);

  // Beep on boot
  tone(PIN_BUZ, 300, 300);
  noTone(PIN_BUZ);

  // Draw AdaFruit logo
  display.display();
  delay(500);
}

void loop() {
  digitalWrite(13, HIGH);
  // Just menu
  // TODO : Add sleep mode
  startMenu();
}
