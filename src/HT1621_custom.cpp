/*******************************************************************************
Copyright 2016-2018 anxzhu (github.com/anxzhu)
Copyright 2018-2020 Valerio Nappi (github.com/5N44P) (changes)
Based on segment-lcd-with-ht1621 from anxzhu (2016-2018)
(https://github.com/anxzhu/segment-lcd-with-ht1621)

Partially rewritten and extended by Valerio Nappi (github.com/5N44P) in 2018

This file is part of the HT1621 arduino library, and thus under the MIT license.
More info on the project and the license conditions on :
https://github.com/5N44P/ht1621-7-seg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include <Arduino.h>
#include "HT1621_custom.h"

HT1621::HT1621()
{
    clear_buffer();
}
void HT1621::clear_buffer()
{
    _buffer[0] = 0x00;
    _buffer[1] = 0x00;
    _buffer[2] = 0x00;
    _buffer[3] = 0x00;
    _buffer[4] = 0x00;
    _buffer[5] = 0x00;
    _buffer[6] = 0x00;
    _buffer[7] = 0x00;
    _buffer[8] = 0x00;
    _buffer[9] = 0x00;
    _buffer[10] = 0x00;
    _buffer[11] = 0x00;
}

void HT1621::begin(int cs_p, int wr_p, int data_p, int backlight_p)
{
    pinMode(cs_p, OUTPUT);
    pinMode(wr_p, OUTPUT);
    pinMode(data_p, OUTPUT);
    pinMode(backlight_p, OUTPUT);
    _cs_p = cs_p;
    _wr_p = wr_p;
    _data_p = data_p;
    _backlight_p = backlight_p;
    _backlight_en = true;
    config();
}

void HT1621::begin(int cs_p, int wr_p, int data_p)
{
    pinMode(cs_p, OUTPUT);
    pinMode(wr_p, OUTPUT);
    pinMode(data_p, OUTPUT);
    _cs_p = cs_p;
    _wr_p = wr_p;
    _data_p = data_p;
    _backlight_en = false;
    config();
}

void HT1621::wrDATA(unsigned char data, unsigned char cnt)
{
    unsigned char i;
    unsigned char mask = 0x80;
    if (cnt == 4)
    {
        mask = 0x8;
    }

    for (i = 0; i < cnt; i++)
    {
        digitalWrite(_wr_p, LOW);
        delayMicroseconds(4);
        if (data & 0x80)
        {
            digitalWrite(_data_p, HIGH);
        }
        else
        {
            digitalWrite(_data_p, LOW);
        }
        digitalWrite(_wr_p, HIGH);
        delayMicroseconds(4);
        data <<= 1;
    }
}
void HT1621::wrclrdata(unsigned char addr, unsigned char sdata)
{
    addr <<= 2;
    digitalWrite(_cs_p, LOW);
    wrDATA(0xa0, 3);
    wrDATA(addr, 6);
    wrDATA(sdata, 8);
    digitalWrite(_cs_p, HIGH);
}

void HT1621::display()
{
    wrCMD(LCDON);
}

void HT1621::noDisplay()
{
    wrCMD(LCDOFF);
}

void HT1621::wrone(unsigned char addr, unsigned char sdata, unsigned char len)
{
    addr <<= 2;
    digitalWrite(_cs_p, LOW);
    wrDATA(0xa0, 3);
    wrDATA(addr, 6);
    wrDATA(sdata, len);
    digitalWrite(_cs_p, HIGH);
}

void HT1621::backlight()
{
    if (_backlight_en)
        digitalWrite(_backlight_p, HIGH);
    delay(1);
}

void HT1621::noBacklight()
{
    if (_backlight_en)
        digitalWrite(_backlight_p, LOW);
    delay(1);
}

void HT1621::wrCMD(unsigned char CMD)
{ //100
    digitalWrite(_cs_p, LOW);
    wrDATA(0x80, 4);
    wrDATA(CMD, 8);
    digitalWrite(_cs_p, HIGH);
}

void HT1621::config()
{
    wrCMD(BIAS);
    wrCMD(RC256);
    wrCMD(SYSDIS);
    wrCMD(WDTDIS1);
    wrCMD(SYSEN);
    wrCMD(LCDON);
}

void HT1621::wrCLR(unsigned char len)
{
    unsigned char addr = 0;
    unsigned char i;
    for (i = 0; i < len; i++)
    {
        wrclrdata(addr, 0x00);
        addr = addr + 2;
    }
}

void HT1621::setBatteryLevel(int level)
{
    // zero out the previous (otherwise the or couldn't be possible)
    _buffer[0] &= 0x7F;
    _buffer[1] &= 0x7F;
    _buffer[2] &= 0x7F;

    switch (level)
    {
    case 3: // battery on and all 3 segments
        _buffer[0] |= 0x80;
    case 2: // battery on and 2 segments
        _buffer[1] |= 0x80;
    case 1: // battery on and 1 segment
        _buffer[2] |= 0x80;
    case 0: // battery indication off
    default:
        break;
    }

    update();
}

void HT1621::clear()
{
    wrCLR(16);
}

// takes the buffer and puts it straight into the driver
void HT1621::update()
{
    // the buffer is backwards with respect to the lcd. could be improved
    wrone(1, _buffer[5], 8);
    wrone(3, _buffer[6], 8);
    wrone(5, _buffer[7], 8);
    wrone(7, _buffer[8], 8);
    wrone(9, _buffer[0], 8);
    wrone(11, _buffer[1], 8);
    wrone(13, _buffer[2], 8);
    wrone(16, _buffer[3], 8);
    wrone(18, _buffer[4], 8);
    wrone(20, _buffer[9], 8);
    wrone(15, _buffer[10], 4);
    wrone(0, _buffer[11], 8);
}

void HT1621::print_digit(int digit, char value)
{
    _buffer[digit] = charToSegBits(value + '0');
    Serial.println(_buffer[digit]);
    update();
}

void HT1621::setMarker(int marker_idx)
{
    int buffer = marker_idx / 8;
    int segment = marker_idx % 8;
    _buffer[buffer] |= 1 << segment;
    update();
}

void HT1621::print(long num, char *flags, int precision)
{
    if (num > 999999) // basic checks
        num = 999999; // clip into 999999
    if (num < -99999) // basic checks
        num = -99999; // clip into -99999

    char localbuffer[6];                  //buffer to work within the function
    snprintf(localbuffer, 6, flags, num); // convert the decimal into string
    Serial.begin(9600);
    Serial.print(localbuffer);
    Serial.print("\t");

    // horrible handling but should get us working. needs refactor in next major
    if (precision > 0 && (num) < pow(10, precision))
    {
        // we remove extra leading zeros
        for (int i = 0; i < (5 - precision); i++)
        {
            Serial.print(localbuffer[1]);
            if (localbuffer[i + 1] == '0' && localbuffer[i] != '-')
            { // we remove only if there is another zero ahead AND if it's not a minus sign
                localbuffer[i] = ' ';
            }
            else
            {
                break;
            }
            Serial.println();
        }
    }

    for (int i = 0; i < 6; i++)
    {
        _buffer[i] &= 0x80; // mask the first bit, used by batter and decimal point
        _buffer[i] |= charToSegBits(localbuffer[i]);
    }
    update();
}

void HT1621::print(double num, int precision)
{
    if (num > 999999) // basic checks
        num = 999999; // clip into 999999
    if (num < -99999) // basic checks
        num = -99999; // clip into -99999

    if (precision > 2 && num > 0)
        precision = 2; // if positive max precision allowed = 2
    else if (precision > 1 && num < 0)
        precision = 1; // if negative max precision allowed = 1
    if (precision < 0)
        precision = 0; // negative precision?!

    char *flags = "%5li";

    if (precision > 0 && abs(num) < 1)
    {
        flags = "%05li";
    }

    long integerpart;
    integerpart = ((long)(num * pow(10, precision)));

    print(integerpart, flags, precision); // draw the integerized number
    setdecimalseparator(precision);       // draw the decimal point

    update();
}

void HT1621::printCelsius(double num)
{
    if (num > 9999) // basic checks
        num = 9999; // clip into 999999
    if (num < -999) // basic checks
        num = -999; // clip into -99999

    int precision;

    if (num <= -100 || num >= 999)
        precision = 0; // if negative max precision allowed = 0
    else
        precision = 1; // if positive max precision allowed = 1

    char *flags = "%4li*C";

    if (precision > 0 && abs(num) < 1)
    {
        flags = "%04li*C";
    }

    long integerpart;
    integerpart = ((long)(num * pow(10, precision)));

    print(integerpart, flags, precision); // draw the integerized number
    if (precision > 0)
        setdecimalseparator(precision + 2); // draw the decimal point shifted by 2
    else
        setdecimalseparator(0); // or clear the decimal separator

    update();
}

void HT1621::print(char *str, bool leftPadded)
{
    int chars = strlen(str);
    int padding = 6 - chars;

    for (int i = 0; i < 6; i++)
    {
        _buffer[i] &= 0x80; // mask the first bit, used by batter and decimal point
        char character = leftPadded
                             ? i < padding ? ' ' : str[i - padding]
                             : i >= chars ? ' ' : str[i];
        _buffer[i] |= charToSegBits(character);
    }

    setdecimalseparator(0); // Hide decimal point
    update();
}

void HT1621::setdecimalseparator(int decimaldigits)
{
    setMarker(DECIMAL_POINT);
}

char HT1621::charToSegBits(char character)
{
    switch (character)
    {
    case '*': // For degree for now
        return 0b1100110;
    case '|':
        return 0b1010000;
    case '-':
        return 0b0100000;
    case '_':
        return 0b0001000;
    case '0':
        return 0b1011111;
    case '1':
        return 0b0000011;
    case '2':
        return 0b0111110;
    case '3':
        return 0b0101111;
    case '4':
        return 0b1100011;
    case '5':
        return 0b1101101;
    case '6':
        return 0b1111101;
    case '7':
        return 0b0000111;
    case '8':
        return 0b1111111;
    case '9':
        return 0b1101111;
    case 'A':
    case 'a':
        return 0b1110111;
    case 'b':
    case 'B':
        return 0b1111001;
    case 'c':
    //	return 0b0001110;
    case 'C':
        return 0b1011100;
    case 'd':
    case 'D':
        return 0b0111011;
    case 'e':
    //	return 0b0001110;
    case 'E':
        return 0b1111100;
    case 'f':
    //	return 0b0000111;
    case 'F':
        return 0b1110100;
    case 'G':
    case 'g':
        return 0b1011101;
    case 'h':
    //	return 0b1000111;
    case 'H':
        return 0b1110011;
    case 'i':
    //	return 0b1000000;
    case 'I':
        return 0b0000011;
    case 'J':
    case 'j':
        return 0b1101000;
    case 'l':
    //	return 0b1100000;
    case 'L':
        return 0b0001101;
    case 'm':
    case 'M':
        return 0b1010100;
    case 'n':
    case 'N':
        return 0b1000110;
    case 'O': // we can keep this for zero
    //	return 0b1111101;
    case 'o':
        return 0b0111001;
    case 'P':
    case 'p':
        return 0b1110110;
    case 'q':
    case 'Q':
        return 0b1100111;
    case 'r':
    case 'R':
        return 0b0110000;
    case 'S':
    case 's':
        return 0b1101101;
    case 't':
    case 'T':
        return 0b1111000;
    case 'u':
    //	return 0b1001100;
    case 'U':
        return 0b1011011;
    case 'Y':
    case 'y':
        return 0b1101011;
    case 'z':
    case 'Z':
        return 0b0111110;
    case ' ':
    default:
        return 0b0000000;
    }
}