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
    setCursor(0, 0);

    for (uint8_t p = 0; p < 8; p++)
    {
        Wire.beginTransmission(_address);
        sendOneCommand(OLED_PAGE + p);
        Wire.endTransmission();

        for (uint8_t i = 0; i < 5; i++)
        {
            Wire.beginTransmission(_address);
            Wire.write(OLED_DATA_MODE);
            for (uint8_t j = 0; j < 27; j++)
                Wire.write(0);
            Wire.endTransmission();
        }
    }
}

void Oled::clear(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    setCursor(0, 0);

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
            Wire.write(OLED_COLUMN_LOWER_BITS + ((x + OLED_OFFSET) & 0x0F));
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
    sendOneCommand(OLED_COLUMN_LOWER_BITS + ((x + OLED_OFFSET) & 0x0F));
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
        sendOneCommand(OLED_PAGE + i);
        sendOneCommand(OLED_COLUMN_LOWER_BITS + ((x + OLED_OFFSET) & 0x0F));
        sendOneCommand(OLED_COLUMN_HIGHER_BITS + ((x + OLED_OFFSET) >> 4));
        Wire.endTransmission();

        if (i == (start >> 3))
        {
            Wire.beginTransmission(_address);
            sendOneCommand(OLED_READ_MODIFY_WRITE);
            Wire.write(OLED_ONE_DATA_MODE);
            Wire.endTransmission();

            Wire.requestFrom(_address, 2);
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
            Wire.beginTransmission(_address);
            sendOneCommand(OLED_READ_MODIFY_WRITE);
            Wire.write(OLED_ONE_DATA_MODE);
            Wire.endTransmission();

            Wire.requestFrom(_address, 2);
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
    sendOneCommand(OLED_READ_MODIFY_WRITE);
    Wire.write(OLED_ONE_DATA_MODE);
    Wire.endTransmission();

    for (uint8_t i = start; i <= end; i++)
    {
        Wire.requestFrom(_address, 2);
        Wire.read();
        int data = Wire.read();
        Wire.beginTransmission(_address);
        Wire.write(OLED_ONE_DATA_MODE);
        Wire.write((1 << (y & 0x07)) | data);
        Wire.endTransmission();
    }

    Wire.beginTransmission(_address);
    sendOneCommand(OLED_END);
    Wire.endTransmission();
}

void Oled::print(char text[])
{
    uint8_t h = _y & 0x07;
    uint8_t length = strlen(text);
    uint8_t pageCount = (_y & 0x07) == 0 ? _textScale : _textScale + 1;

    Wire.beginTransmission(_address);
    sendOneCommand(OLED_COLUMN_LOWER_BITS + ((_x + OLED_OFFSET) & 0x0F));
    sendOneCommand(OLED_COLUMN_HIGHER_BITS + ((_x + OLED_OFFSET) >> 4));
    Wire.endTransmission();

    for (uint8_t p = 0; p < pageCount; p++)
    {
        Wire.beginTransmission(_address);
        sendOneCommand(OLED_PAGE + (_y >> 3) + p);
        sendOneCommand(OLED_READ_MODIFY_WRITE);
        Wire.write(OLED_ONE_DATA_MODE);
        Wire.endTransmission();

        for (uint8_t i = 0; i < length; i++)
        {
            for (uint8_t c = 0; c < 6; c++)
            {
                Wire.requestFrom(_address, 2);
                Wire.read();
                int data = Wire.read();
                Wire.beginTransmission(_address);
                Wire.write(OLED_ONE_DATA_MODE);

                if (c == 5)
                {
                    Wire.write(data);
                    Wire.endTransmission();
                    continue;
                }

                int column = pgm_read_byte(&_charMap[text[i] - 32][c]);
                Wire.write((column << h) >> (p << 3) | data);

                Wire.endTransmission();
            }
        }

        Wire.beginTransmission(_address);
        sendOneCommand(OLED_END);
        Wire.endTransmission();
    }

    _x += length * 6;
}

void Oled::printChar(char character)
{
    uint8_t h = _y & 0x07;
    uint8_t pageCount = (_y & 0x07) == 0 ? _textScale : _textScale + 1;

    for (uint8_t p = 0; p < pageCount; p++)
    {
        Wire.beginTransmission(_address);
        sendOneCommand(OLED_PAGE + (_y >> 3) + p);
        sendOneCommand(OLED_COLUMN_LOWER_BITS + ((_x + OLED_OFFSET) & 0x0F));
        sendOneCommand(OLED_COLUMN_HIGHER_BITS + ((_x + OLED_OFFSET) >> 4));
        sendOneCommand(OLED_READ_MODIFY_WRITE);
        Wire.write(OLED_ONE_DATA_MODE);
        Wire.endTransmission();

        for (uint8_t i = 0; i < 6; i++)
        {
            Wire.requestFrom(_address, 2);
            Wire.read();
            int data = Wire.read();
            Wire.beginTransmission(_address);
            Wire.write(OLED_ONE_DATA_MODE);

            if (i == 5)
            {
                Wire.write(data);
                sendOneCommand(OLED_END);
                Wire.endTransmission();
                continue;
            }

            int column = pgm_read_byte(&_charMap[character - 32][i]);
            Wire.write((column << h) >> (p << 3) | data);

            Wire.endTransmission();
        }
    }

    _x += 6;
}

void Oled::printFast(char text[])
{
    uint8_t length = strlen(text);
    uint8_t _count = 0;

    for (uint8_t p = 0; p < _textScale; p++)
    {
        Wire.beginTransmission(_address);
        sendOneCommand(OLED_PAGE + (_y >> 3) + p);
        sendOneCommand(OLED_COLUMN_LOWER_BITS + ((_x + OLED_OFFSET) & 0x0F));
        sendOneCommand(OLED_COLUMN_HIGHER_BITS + ((_x + OLED_OFFSET) >> 4));
        Wire.endTransmission();

        for (uint8_t i = 0; i <= length / (5 / _textScale); i++)
        {
            Wire.beginTransmission(_address);
            Wire.write(OLED_DATA_MODE);
            for (uint8_t c = 0; c < 5 / _textScale; c++)
            {
                for (uint8_t j = 0; j < 6 * _textScale; j++)
                {
                    if (_textScale == 1)
                    {
                        Wire.write((j == 5) ? 0x00 : pgm_read_byte(&_charMap[text[_count] - 32][j]));
                    }
                    else
                    {
                        uint8_t column = pgm_read_byte(&_charMap[text[_count] - 32][j]);
                        uint32_t scaledColumn = 0;

                        for (uint8_t k = 0, count = 0; k < 8; k++)
                            for (uint8_t l = 0; l < _textScale; l++, count++)
                                bitWrite(scaledColumn, count, bitRead(column, k));

                        for (uint8_t s = 0; s < _textScale; s++)
                        {
                            Wire.write((j >= 5 * _textScale) ? 0x00 : (0xFF & (scaledColumn >> (_textScale - p - 1) * 8)));
                        }
                    }
                }

                _count++;
                if (_count == length)
                    break;
            }
            Wire.endTransmission();

            if (_count == length)
                break;
        }
        _count = 0;
    }

    _x += 6 * length * _textScale;
}

void Oled::printCharFast(char character)
{
    for (uint8_t p = 0; p < _textScale; p++)
    {
        Wire.beginTransmission(_address);
        sendOneCommand(OLED_PAGE + (_y >> 3) + p);
        sendOneCommand(OLED_COLUMN_LOWER_BITS + ((_x + OLED_OFFSET) & 0x0F));
        sendOneCommand(OLED_COLUMN_HIGHER_BITS + ((_x + OLED_OFFSET) >> 4));

        Wire.write(OLED_DATA_MODE);
        for (uint8_t c = 0; c < 5; c++)
        {
            if (_textScale == 1)
            {
                Wire.write(pgm_read_byte(&_charMap[character - 32][c]));
            }
            else
            {
                uint8_t column = pgm_read_byte(&_charMap[character - 32][c]);
                uint32_t scaledColumn = 0;

                for (uint8_t i = 0, count = 0; i < 8; i++)
                    for (uint8_t j = 0; j < _textScale; j++, count++)
                        bitWrite(scaledColumn, count, bitRead(column, i));

                for (uint8_t s = 0; s < _textScale; s++)
                {
                    Wire.write(0xFF & (scaledColumn >> (_textScale - p - 1) * 8));
                }
            }
        }

        Wire.endTransmission();
    }
    _x += 6 * _textScale;
}

void Oled::setCursor(uint8_t x, uint8_t y)
{
    _x = x;
    _y = y;
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
