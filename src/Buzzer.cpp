#include "Buzzer.h"

Buzzer::Buzzer(int pin) : _pin(pin)
{
}

void Buzzer::begin()
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
}

void Buzzer::beep(unsigned long onMs, unsigned long offMs, uint8_t times)
{
    for (uint8_t i = 0; i < times; i++)
    {
        digitalWrite(_pin, HIGH);
        delay(onMs);
        digitalWrite(_pin, LOW);
        if (i + 1 < times)
        {
            delay(offMs);
        }
    }
}

void Buzzer::beepShort()
{
    beep(100, 0, 1);
}

void Buzzer::beepLong()
{
    beep(600, 0, 1);
}

void Buzzer::beepError()
{
    beep(150, 150, 3);
}
