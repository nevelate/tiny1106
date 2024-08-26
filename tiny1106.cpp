#include <tiny1106.h>

Oled::Oled(uint8_t address)
{
    _address = address;
}

void Oled::init()
{
    Wire.beginTransmission(_address);
    Wire.write(OLED_ONE_COMMAND_MODE);
    Wire.write(0xAF); // Display on
    Wire.endTransmission();
}

void Oled::setContrast(uint8_t contrast)
{
    Wire.beginTransmission(_address);
    Wire.write(OLED_COMMAND_MODE);
    Wire.write(OLED_CONTRAST);
    Wire.write(contrast);
    Wire.endTransmission();
}