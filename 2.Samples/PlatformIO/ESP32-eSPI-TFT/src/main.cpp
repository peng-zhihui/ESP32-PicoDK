#include <Arduino.h>
#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();

unsigned long total = 0;
unsigned long tn = 0;

unsigned long testFillScreen()
{
    unsigned long start = micros();
    tft.fillScreen(TFT_BLACK);
    tft.fillScreen(TFT_RED);
    tft.fillScreen(TFT_GREEN);
    tft.fillScreen(TFT_BLUE);
    tft.fillScreen(TFT_BLACK);
    return (micros() - start) / 5;
}


void setup()
{
    Serial.begin(115200);
    while (!Serial);

    pinMode(5, OUTPUT);
    digitalWrite(5, LOW);

    Serial.println("");
    Serial.println("");
    Serial.println("TFT_eSPI library test!");

    tft.init();
    tft.fillScreen(TFT_BLACK);

    // Create a viewport 220 x 300 pixels
    tft.setViewport(0, 0, 240, 240);

    tft.frameViewport(TFT_RED, 1); // 1 pixel wide frame around viewport

}

void loop(void)
{
    for (uint8_t rotation = 0; rotation < 4; rotation++)
    {
        tft.setRotation(rotation);
        tft.resetViewport();       // reset viewport to whole screen
        tft.fillScreen(TFT_BLACK); // so it can be cleared

        printf("%lu\n", testFillScreen());

        delay(2000);
    }
}