#include "Encoder.h"

Encoder *Encoder::instance = nullptr;

Encoder::Encoder(uint8_t clk_pin, uint8_t dt_pin)
    : clk_pin(clk_pin), dt_pin(dt_pin), quad(0) {}

void Encoder::begin()
{
    pinMode(clk_pin, INPUT_PULLUP);
    pinMode(dt_pin, INPUT_PULLUP);
    instance = this;
    delay(1000);
    attachInterrupt(digitalPinToInterrupt(clk_pin), EncoderStatus, CHANGE);
    attachInterrupt(digitalPinToInterrupt(dt_pin), EncoderStatus, CHANGE);
}

void Encoder::EncoderStatus()
{
    static uint8_t LastRead = 3;
    uint8_t ThisRead = 0;

    ThisRead = (digitalRead(instance->clk_pin) << 1 | digitalRead(instance->dt_pin));
    if (ThisRead == LastRead)
        return;

    if (bitRead(ThisRead, 0) == bitRead(LastRead, 1))
        instance->quad += 1;
    else
        instance->quad -= 1;

    LastRead = ThisRead;
}
int8_t Encoder::EncoderGet(void)
{
    int8_t val = 0;
    if (abs(quad) >= 4)
    {
        val = quad / 4;
        quad %= 4;
    }
    return val;
}
