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
    Wire.write(OLED_COMMAND_MODE);
    Wire.write(OLED_DISPLAY_OFF);
    Wire.write(0x40); // set display start line to 0
    Wire.write(OLED_DISPLAY_OFFSET_MODE);
    Wire.write(0x00); // set display offset to 0
    Wire.write(OLED_NORMAL_V);
    Wire.write(OLED_NORMAL_H);
    Wire.write(OLED_CONTRAST);
    Wire.write(0xFF);
    Wire.write(OLED_DISPLAY_ON);
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

    uint8_t startPage = startY >> 3;
    uint8_t endPage = endY >> 3;

    for (uint8_t i = startPage; i <= endPage; i++)
    {
        Wire.beginTransmission(_address);
        Wire.write(OLED_COMMAND_MODE);
        Wire.write(OLED_PAGE + i);
        Wire.write(OLED_COLUMN_LOWER_BITS + ((startX + OLED_OFFSET) & 0x0F));
        Wire.write(OLED_COLUMN_HIGHER_BITS + ((startX + OLED_OFFSET) >> 4));
        Wire.endTransmission();

        if(startPage == endPage)
        {
            Wire.beginTransmission(_address);
            sendOneCommand(OLED_READ_MODIFY_WRITE);
            Wire.write(OLED_ONE_DATA_MODE);
            Wire.endTransmission();
            for (uint8_t i = startX; i <= endX; i++)
            {
                Wire.requestFrom(_address, 2);
                Wire.read();
                int data = Wire.read();
                Wire.beginTransmission(_address);
                Wire.write(OLED_ONE_DATA_MODE);
                Wire.write(~(0xFF >> (7 - endY + startY) << (startY & 0x07)) & data);
                Wire.endTransmission();
            }
            Wire.beginTransmission(_address);
            sendOneCommand(OLED_END);
            Wire.endTransmission();
        }
        else
        {
            if (i == startPage)
            {
                Wire.beginTransmission(_address);
                sendOneCommand(OLED_READ_MODIFY_WRITE);
                Wire.write(OLED_ONE_DATA_MODE);
                Wire.endTransmission();
                for (uint8_t i = startX; i <= endX; i++)
                {
                    Wire.requestFrom(_address, 2);
                    Wire.read();
                    int data = Wire.read();
                    Wire.beginTransmission(_address);
                    Wire.write(OLED_ONE_DATA_MODE);
                    Wire.write(~(0xFF << (startY & 0x07)) & data);
                    Wire.endTransmission();
                }            
                Wire.beginTransmission(_address);
                sendOneCommand(OLED_END);
                Wire.endTransmission();        
            }
            else if (i == endPage)
            {
                Wire.beginTransmission(_address);
                sendOneCommand(OLED_READ_MODIFY_WRITE);
                Wire.write(OLED_ONE_DATA_MODE);
                Wire.endTransmission();
                for (uint8_t i = startX; i <= endX; i++)
                {
                    Wire.requestFrom(_address, 2);
                    Wire.read();
                    int data = Wire.read();
                    Wire.beginTransmission(_address);
                    Wire.write(OLED_ONE_DATA_MODE);
                    Wire.write(~(0xFF >> (7 - (endY & 0x07))) & data);
                    Wire.endTransmission();
                }
                Wire.beginTransmission(_address);
                sendOneCommand(OLED_END);
                Wire.endTransmission();
            }
            else
            {
                uint8_t x = startX;
                for (uint8_t i = 0; i <= (endX - startX) / 27; i++)
                {
                    Wire.beginTransmission(_address);
                    Wire.write(OLED_DATA_MODE);
                    for (uint8_t j = 0; j < 27; j++)
                    {
                        Wire.write(0);
                        x++;
                        if(x > endX) break;
                    }
                    Wire.endTransmission();
                    if(x > endX) break;
                }
            }
        }    
    }
}

void Oled::fill(uint8_t fill){
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
                Wire.write(fill ? 0xFF : 0);
            Wire.endTransmission();
        }
    }
}

void Oled::drawPoint(uint8_t x, uint8_t y)
{
    Wire.beginTransmission(_address);
    Wire.write(OLED_COMMAND_MODE);
    Wire.write(OLED_PAGE + (y >> 3));
    Wire.write(OLED_COLUMN_LOWER_BITS + ((x + OLED_OFFSET) & 0x0F));
    Wire.write(OLED_COLUMN_HIGHER_BITS + ((x + OLED_OFFSET) >> 4));
    Wire.endTransmission();

    Wire.beginTransmission(_address);
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

    uint8_t startPage = start >> 3;
    uint8_t endPage = end >> 3;

    for (uint8_t i = startPage; i <= endPage; i++)
    {
        Wire.beginTransmission(_address);
        Wire.write(OLED_COMMAND_MODE);
        Wire.write(OLED_PAGE + i);
        Wire.write(OLED_COLUMN_LOWER_BITS + ((x + OLED_OFFSET) & 0x0F));
        Wire.write(OLED_COLUMN_HIGHER_BITS + ((x + OLED_OFFSET) >> 4));
        Wire.endTransmission();

        if(startPage == endPage)
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
            Wire.write((0xFF >> (7 - end + start) << (start & 0x07)) | data);

            sendOneCommand(OLED_END);

            Wire.endTransmission();
        }
        else
        {
            if (i == startPage)
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
                Wire.write((0xFF << (start & 0x07)) | data);

                sendOneCommand(OLED_END);

                Wire.endTransmission();
            }
            else if (i == endPage)
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
                Wire.write((0xFF >> (7 - (end & 0x07))) | data);

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
}

void Oled::drawLineH(uint8_t y, uint8_t x0, uint8_t x1)
{
    uint8_t start = min(x0, x1);
    uint8_t end = max(x0, x1);

    Wire.beginTransmission(_address);
    Wire.write(OLED_COMMAND_MODE);
    Wire.write(OLED_PAGE + (y >> 3));
    Wire.write(OLED_COLUMN_LOWER_BITS + ((start + OLED_OFFSET) & 0x0F));
    Wire.write(OLED_COLUMN_HIGHER_BITS + ((start + OLED_OFFSET) >> 4));
    Wire.endTransmission();

    Wire.beginTransmission(_address);
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
    Wire.write(OLED_COMMAND_MODE);
    Wire.write(OLED_COLUMN_LOWER_BITS + ((_x + OLED_OFFSET) & 0x0F));
    Wire.write(OLED_COLUMN_HIGHER_BITS + ((_x + OLED_OFFSET) >> 4));
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

void Oled::printFast(char text[])
{
    uint8_t length = strlen(text);
    uint8_t count = 0;

    for (uint8_t p = 0; p < _textScale; p++)
    {
        Wire.beginTransmission(_address);
        Wire.write(OLED_COMMAND_MODE);
        Wire.write(OLED_PAGE + (_y >> 3) + p);
        Wire.write(OLED_COLUMN_LOWER_BITS + ((_x + OLED_OFFSET) & 0x0F));
        Wire.write(OLED_COLUMN_HIGHER_BITS + ((_x + OLED_OFFSET) >> 4));
        Wire.endTransmission();

        for (uint8_t i = 0; i <= length / (5 / _textScale); i++)
        {
            Wire.beginTransmission(_address);
            Wire.write(OLED_DATA_MODE);
            for (uint8_t j = 0; j < 5 / _textScale; j++)
            {
                for (uint8_t c = 0; c < 6; c++)
                {
                    if (_textScale == 1)
                    {
                        Wire.write((c == 5) ? 0x00 : pgm_read_byte(&_charMap[text[count] - 32][c]));
                    }
                    else
                    {
                        uint8_t column = pgm_read_byte(&_charMap[text[count] - 32][c]);
                        uint32_t scaledColumn = scaleByte(column);

                        for (uint8_t s = 0; s < _textScale; s++)
                        {
                            Wire.write((c == 5) ? 0x00 : (0xFF & (scaledColumn >> (p * 8))));
                        }
                    }
                }

                count++;
                if (count == length)
                    break;
            }
            Wire.endTransmission();

            if (count == length)
                break;
        }
        count = 0;
    }

    _x += 6 * length * _textScale;
}

void Oled::setCursor(uint8_t x, uint8_t y)
{
    _x = x;
    _y = y;
}

void Oled::setTextScale(uint8_t scale)
{
    _textScale = constrain(scale, 1, 4);
}

void Oled::sendCommand(int8_t command)
{
    Wire.beginTransmission(_address);
    sendOneCommand(command);
    Wire.endTransmission();
}

void Oled::sendOneCommand(int8_t command)
{
    Wire.write(OLED_ONE_COMMAND_MODE);
    Wire.write(command);
}

uint32_t Oled::scaleByte(uint8_t data)
{
    uint32_t scaledColumn = 0;

    for (uint8_t i = 0, count = 0; i < 8; i++)
        for (uint8_t j = 0; j < _textScale; j++, count++)
            bitWrite(scaledColumn, count, bitRead(data, i));

    return scaledColumn;
}
