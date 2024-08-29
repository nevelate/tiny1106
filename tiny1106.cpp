// Credits:
// http://www.technoblogy.com/show?23OS

#include <tiny1106.h>

Oled::Oled(uint8_t address)
{
    _address = address;
}

void Oled::init()
{
    Wire.begin();
    clear();
    Wire.beginTransmission(_address);
    sendOneCommand(OLED_DISPLAY_ON);
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

void Oled::clear()
{
    for (uint8_t p = 0; p < 8; p++)
    {
        Wire.beginTransmission(_address);
        sendOneCommand(OLED_PAGE + p);
        Wire.endTransmission();

        for (uint8_t i = 0; i < 8; i++)
        {
            Wire.beginTransmission(_address);
            Wire.write(OLED_DATA_MODE);
            for (uint8_t j = 0; j < 17; j++)
                Wire.write(0);
            Wire.endTransmission();
        }
    }
}

void Oled::clear(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    uint8_t startX = min(x0, x1);
    uint8_t endX = max(x0, x1);

    uint8_t startY = min(y0, y1);
    uint8_t endY = max(y0, y1);

    for (uint8_t i = (startY >> 3); i <= (endY >> 3); i++)
    {
        uint8_t x = startX;
        Wire.beginTransmission(_address);
        sendOneCommand(OLED_PAGE + i);
        Wire.endTransmission();

        for (uint8_t j = 0; j < 8; j++)
        {
            Wire.beginTransmission(_address);
            Wire.write(OLED_COLUMN_LOWER_BITS + ((x + OLED_OFFSET) & 0x0F)); // may not work
            Wire.write(OLED_COLUMN_HIGHER_BITS + ((x + OLED_OFFSET) >> 4));
            Wire.endTransmission();

            for (uint8_t k = 0; k < 17; k++)
            {
                if (i == (startY >> 3))
                {
                    Wire.requestFrom(_address, 1);
                    Wire.read();
                    int data = Wire.read();
                    Wire.beginTransmission(_address);
                    Wire.write(OLED_ONE_DATA_MODE);
                    Wire.write((0xFF >> (x & 0x07)) | data);

                    sendOneCommand(OLED_END);

                    Wire.endTransmission();
                }
                else if (i == (endY >> 3))
                {
                    Wire.requestFrom(_address, 1);
                    Wire.read();
                    int data = Wire.read();
                    Wire.beginTransmission(_address);
                    Wire.write(OLED_ONE_DATA_MODE);
                    Wire.write(~(0xFF >> (x & 0x07)) | data);

                    sendOneCommand(OLED_END);

                    Wire.endTransmission();
                }
                else
                {
                    Wire.beginTransmission(_address);
                    Wire.write(OLED_ONE_DATA_MODE);
                    Wire.write(0);
                    Wire.endTransmission();
                }

                if (x == endX)
                    break;
                x++;
            }
        }
    }
}

void Oled::drawPoint(uint8_t x, uint8_t y)
{
    Wire.beginTransmission(_address);

    sendOneCommand(OLED_PAGE + (y >> 3));
    sendOneCommand(OLED_COLUMN_LOWER_BITS + ((x + OLED_OFFSET) & 0x0F)); // may not work
    sendOneCommand(OLED_COLUMN_HIGHER_BITS + ((x + OLED_OFFSET) >> 4));

    sendOneCommand(OLED_READ_MODIFY_WRITE);
    Wire.write(OLED_ONE_DATA_MODE);
    Wire.endTransmission();

    Wire.requestFrom(_address, 2);
    Wire.read();
    int data = Wire.read();
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
    uint8_t start = min(y0, y1);
    uint8_t end = max(y0, y1);

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
            Wire.requestFrom(_address, 1);
            Wire.read();
            int data = Wire.read();
            Wire.beginTransmission(_address);
            Wire.write(OLED_ONE_DATA_MODE);
            Wire.write((0xFF >> (start & 0x07)) | data);

            sendOneCommand(OLED_END);

            Wire.endTransmission();
        }
        else if (i == (end >> 3))
        {
            Wire.requestFrom(_address, 1);
            Wire.read();
            int data = Wire.read();
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
    uint8_t start = min(x0, x1);
    uint8_t end = max(x0, x1);

    uint8_t x = start;
    Wire.beginTransmission(_address);
    sendOneCommand(OLED_PAGE + (y >> 3));
    sendOneCommand(OLED_COLUMN_LOWER_BITS + ((start + OLED_OFFSET) & 0x0F));
    sendOneCommand(OLED_COLUMN_HIGHER_BITS + ((start + OLED_OFFSET) >> 4));
    Wire.endTransmission();

    for (uint8_t i = 0; i < 8; i++)
    {
        Wire.beginTransmission(_address);
        sendOneCommand(OLED_READ_MODIFY_WRITE);
        Wire.write(OLED_ONE_DATA_MODE);
        Wire.endTransmission();
        for (uint8_t j = 0; j < 17; j++)
        {
            Wire.requestFrom(_address, 2);
            Wire.read();
            int data = Wire.read();
            Wire.beginTransmission(_address);
            Wire.write(OLED_ONE_DATA_MODE);
            Wire.write((1 << (y & 0x07)) | data);

            if (x == end)
            {
                sendOneCommand(OLED_END);
                Wire.endTransmission();
                return;
            }            
            Wire.endTransmission();
            x++;
        }
    }
}

void Oled::print(uint8_t x, uint8_t y, char text[])
{
}

void Oled::printChar(uint8_t x, uint8_t y, char character)
{
}

void Oled::setTextScale(uint8_t scale)
{
    _textScale = scale;
}

void Oled::sendOneCommand(int8_t command)
{
    Wire.write(OLED_ONE_COMMAND_MODE);
    Wire.write(command);
}
