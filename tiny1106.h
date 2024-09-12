// Credits:
// http://www.technoblogy.com/show?23OS

#ifndef tiny1106_h
#define tiny1106_h

#include <Arduino.h>
#include <Wire.h>

#include "charMap.h"

#ifdef OLED_RU
#include "charMapRU.h"
#endif

#ifndef OLED_OFFSET
#define OLED_OFFSET 2
#endif

#define OLED_DISPLAY_ON 0xAF
#define OLED_DISPLAY_OFF 0xAE

#define OLED_COMMAND_MODE 0x00
#define OLED_ONE_COMMAND_MODE 0x80
#define OLED_DATA_MODE 0x40
#define OLED_ONE_DATA_MODE 0xC0

#define OLED_CONTRAST 0x81

#define OLED_NORMAL 0xA6
#define OLED_INVERTED 0xA7

#define OLED_READ_MODIFY_WRITE 0xE0
#define OLED_END 0xEE

#define OLED_PAGE 0xB0
#define OLED_COLUMN_HIGHER_BITS 0x10
#define OLED_COLUMN_LOWER_BITS 0x00

class Oled
{
public:
    Oled(uint8_t address = 0x3C);
    void init();
    void setContrast(uint8_t contrast);
    void clear();
    void clear(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

    void drawPoint(uint8_t x, uint8_t y);

    void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
    void drawLineV(uint8_t x, uint8_t y0, uint8_t y1);
    void drawLineH(uint8_t y, uint8_t x0, uint8_t x1);

    void print(char text[]);
    void printChar(char character);

    void setCursor(uint8_t x, uint8_t y);
    void setTextScale(uint8_t scale);

private:
    int _address;
    uint8_t _textScale, _x, _y;
    void sendOneCommand(int8_t command);
};

#endif