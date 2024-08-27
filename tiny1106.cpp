// Credits:
// http://www.technoblogy.com/show?23OS

#include <tiny1106.h>

Oled::Oled(uint8_t address)
{
    _address = address;
}

// Initialize display
void Oled::init()
{
    Wire.beginTransmission(_address);
    sendOneCommand(OLED_DISPLAY_ON);
    setContrast(255);
    Wire.endTransmission();
}

// Set display contrast (0-255)
void Oled::setContrast(uint8_t contrast)
{
    Wire.beginTransmission(_address);
    Wire.write(OLED_COMMAND_MODE);
    Wire.write(OLED_CONTRAST);
    Wire.write(contrast);
    Wire.endTransmission();
}

void Oled::clear()
{
    for (uint8_t p = 0; p < 8; p++)
    {
        Wire.beginTransmission(_address);
        Wire.write(OLED_PAGE + p);
        Wire.endTransmission();

        for (uint8_t i = 0; i < 8; i++)
        {
            Wire.beginTransmission(_address);
            Wire.write(OLED_DATA_MODE);
            for (int i = 0; i < 17; i++)
                Wire.write(0);
            Wire.endTransmission();
        }
    }
}

void Oled::clear(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
}

void Oled::drawPoint(uint8_t x, uint8_t y)
{
    Wire.beginTransmission(_address);
    Wire.write(OLED_COMMAND_MODE);

    Wire.write(OLED_PAGE + (y >> 3));
    Wire.write(OLED_COLUMN_LOWER_BITS + ((x + OLED_OFFSET) & 0x0F)); // may not work
    Wire.write(OLED_COLUMN_HIGHER_BITS + ((x + OLED_OFFSET) >> 4));

    Wire.write(OLED_READ_MODIFY_WRITE);
    Wire.endTransmission();

    Wire.requestFrom(_address, 2);
    Wire.read();
    int8_t data = Wire.read();
    Wire.beginTransmission(_address);
    Wire.write(OLED_ONE_DATA_MODE);
    Wire.write((1 << (y & 0x07)) | data);

    sendOneCommand(OLED_END);

    Wire.endTransmission();
}

void Oled::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    int sx, sy, e2, err;
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    if (x0 < x1)
        sx = 1;
    else
        sx = -1;
    if (y0 < y1)
        sy = 1;
    else
        sy = -1;
    err = dx - dy;
    for (;;)
    {
        drawPoint(x0, y0);
        if (x0 == x1 && y0 == y1)
            return;
        e2 = err << 1;
        if (e2 > -dy)
        {
            err = err - dy;
            x0 = x0 + sx;
        }
        if (e2 < dx)
        {
            err = err + dx;
            y0 = y0 + sy;
        }
    }
}

void Oled::drawLineV(uint8_t x, uint8_t y0, uint8_t y1)
{
    uint8_t start = y1 > y0 ? y0 : y1;
    uint8_t end = y1 > y0 ? y1 : y0;

    for (uint8_t i = (start >> 3); i <= (end >> 3); i++)
    {
        Wire.beginTransmission(_address);
        Wire.write(OLED_COMMAND_MODE);

        Wire.write(OLED_PAGE + i);
        Wire.write(OLED_COLUMN_LOWER_BITS + ((x + OLED_OFFSET) & 0x0F)); // may not work
        Wire.write(OLED_COLUMN_HIGHER_BITS + ((x + OLED_OFFSET) >> 4));
        Wire.endTransmission();

        if (i == (start >> 3))
        {
            Wire.requestFrom(_address, 2);
            Wire.read();
            int8_t data = Wire.read();
            Wire.beginTransmission(_address);
            Wire.write(OLED_ONE_DATA_MODE);
            Wire.write((0xFF >> (start & 0x07)) | data);

            sendOneCommand(OLED_END);

            Wire.endTransmission();
        }
        else if (i == (end >> 3))
        {
            Wire.requestFrom(_address, 2);
            Wire.read();
            int8_t data = Wire.read();
            Wire.beginTransmission(_address);
            Wire.write(OLED_ONE_DATA_MODE);
            Wire.write(~(0xFF >> (end & 0x07)) | data);

            sendOneCommand(OLED_END);

            Wire.endTransmission();
        }
        else
        {
            Wire.beginTransmission(_address);
            Wire.write(OLED_ONE_DATA_MODE);
            Wire.write(0xFF);
            Wire.endTransmission();
        }
    }
}

void Oled::drawLineH(uint8_t y, uint8_t x0, uint8_t x1)
{
    uint8_t x = x0;
    Wire.beginTransmission(_address);
    sendOneCommand(OLED_PAGE + (y >> 3));
    Wire.endTransmission();

    for (uint8_t i = 0; i < 8; i++)
    {
        Wire.beginTransmission(_address);
        Wire.write(OLED_COLUMN_LOWER_BITS + ((x + OLED_OFFSET) & 0x0F)); // may not work
        Wire.write(OLED_COLUMN_HIGHER_BITS + ((x + OLED_OFFSET) >> 4));
        Wire.endTransmission();

        for (uint8_t j = 0; j < 17; j++)
        {
            Wire.beginTransmission(_address);
            sendOneCommand(OLED_READ_MODIFY_WRITE);
            Wire.endTransmission();

            Wire.requestFrom(_address, 2);
            Wire.read();
            int8_t data = Wire.read();
            Wire.beginTransmission(_address);
            Wire.write(OLED_ONE_DATA_MODE);
            Wire.write((1 << (y & 0x07)) | data);
            sendOneCommand(OLED_END);
            Wire.endTransmission();

            if (x == x1)
                return;
            x++;
        }
    }
}

void Oled::sendOneCommand(int8_t command)
{
    Wire.write(OLED_ONE_COMMAND_MODE);
    Wire.write(command);
}
