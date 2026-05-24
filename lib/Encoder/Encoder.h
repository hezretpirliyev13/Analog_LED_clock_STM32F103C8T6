#ifndef _ENCODER_H
#define _ENCODER_H

#include <Arduino.h>

class Encoder
{
private:
    __uint8_t clk_pin;
    __uint8_t dt_pin;
    volatile __int8_t quad;
    static Encoder *instance;

public:
    Encoder(__uint8_t clk_pin, __uint8_t dt_pin);

    void begin();

    static void EncoderStatus(void);
    __int8_t EncoderGet(void);
};

#endif