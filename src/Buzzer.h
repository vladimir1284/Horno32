#ifndef Buzzer_h
#define Buzzer_h

#include <Arduino.h>

// Driven through a transistor switch (see Schematic.fzz, J2/U8/R8) as a
// simple on/off DC load — assumed to be an active buzzer with its own
// oscillator, not a passive piezo needing a tone frequency.
class Buzzer
{
public:
    explicit Buzzer(int pin);

    void begin();

    void beepShort(); // short confirmation beep
    void beepLong();  // long beep (e.g. setpoint reached)
    void beepError(); // 3 short beeps

private:
    int _pin;
    void beep(unsigned long onMs, unsigned long offMs, uint8_t times);
};

#endif
