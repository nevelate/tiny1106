#ifndef tiny1106_h
#define tiny1106_h

#include <Arduino.h>
#include <Wire.h>

#include <charMapEN.h>

#ifdef OLED_RU
#include <charMapRU.h>
#endif

#define OLED_COMMAND_MODE 0x00
#define OLED_ONE_COMMAND_MODE 0x80
#define OLED_DATA_MODE 0x40
#define OLED_ONE_DATA_MODE 0xC0

#define OLED_CONTRAST 0x81

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

    void print(uint8_t x, uint8_t y, char text[]);

    void setTextScale(uint8_t scale);    

private:
    uint8_t _address;
};

#endif