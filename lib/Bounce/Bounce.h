#ifndef _BOUNCE_H
#define _BOUNCE_H
#include <stdio.h>
#include <Arduino.h>

class Bounce
{
protected:
    __uint16_t debounce();
    __uint16_t previous_millis, interval_millis, rebounce_millis;
    __uint8_t state;
    __uint8_t pin;
    __uint8_t stateChanged;
    /* data */
public:
    Bounce(/* args */);
    Bounce(__uint8_t pin, unsigned long interval_millis);
    void begin();
    void attach(int pin);
    void interval(unsigned long interval_millis);
    bool update();
    void rebounce(unsigned long interval);
    __int16_t read(void);
    void write(int new_state);
    unsigned long duration();
    bool risingEdge();
    bool fallingEdge();
};

#endif