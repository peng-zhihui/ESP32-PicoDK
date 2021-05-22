
#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X80_NONAME_F_HW_I2C u8g2(U8G2_R1, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 22, /* data=*/21);


void setup(void)
{
    u8g2.begin();
    u8g2.enableUTF8Print();        // enable UTF8 support for the Arduino print() function
}

void loop(void)
{
    u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
    u8g2.setFontDirection(0);
    u8g2.clearBuffer();
    u8g2.setCursor(0, 15);
    u8g2.print("Hello World!");
    u8g2.setCursor(0, 40);
    u8g2.print("你好世界");        // Chinese "Hello World"
    u8g2.sendBuffer();

    delay(1000);
}

