#include <Arduino.h>
#include <FastLED.h>
#include "../lib/DS3231/DS3231.h"
#include "../lib/Bounce/Bounce.h"
#include "../lib/Encoder/Encoder.h"

#define BUTTON PA2
#define FRAMES_PER_SECOND 60

Bounce menuBouncer(BUTTON, 50);
DS3231 rtc;
Encoder encoder(PA0, PA1);
Time t;
Time old;

#define LED_PIN PB15
#define NUM_LEDS 60
#define BRIGHTNESS 100

bool menuButton = false;
bool menuPressed = false;
bool menuReleased = false;
bool swingBack = 0;
int advanceMove = 0;
bool countTime = false;
long menuTimePressed;
long lastRotary;
int rotaryTime = 1000;
int fiveMins = 0;
int pendulumPos = 0;

bool button_state = 0;
uint8_t mode = 0;
uint8_t LEDOffset = 0;
int32_t pos = 0;

int cyclesPerSec;
unsigned long newSecTime;
float cyclesPerSecFloat;
float fracOfSec;
int secondBrightness;
int secondBrightness2;
int breathBrightness;
int subSeconds;

CRGB leds[NUM_LEDS];
void bounce(void);

void setup()
{

  Serial.begin(115200);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  rtc.begin();
  rtc.setDate(4, 5, 2026);
  rtc.setTime(13, 5, 0);
  // dwt_init();
  delay(1000);
  attachInterrupt(digitalPinToInterrupt(PA2), bounce, CHANGE);
  // put your setup code here, to run once:
}

void minimalClock(Time *now);
void basicClock(Time *now);
void smoothSecond(Time *now);
void outlineClock(Time *now);
void minimalMilliSec(Time *now);
void simplePendulum(Time *now);
void breathingClock(Time *now);
void modeHysterical(Time *now);

void loop()
{
  t = rtc.getTime();
  int8_t val = encoder.EncoderGet();
  if (val)
  {
    noInterrupts();
    pos += val;
    interrupts();
  }
  FastLED.clear();
  switch (mode)
  {
  case 0:
    minimalClock(&t);
    break;
  case 1:
    basicClock(&t);
    break;
  case 2:
    smoothSecond(&t);
    break;
  case 3:
    outlineClock(&t);
    break;
  case 4:
    minimalMilliSec(&t);
    break;
  case 5:
    simplePendulum(&t);
    break;
  case 6:
    breathingClock(&t);
    break;
  case 7:
    modeHysterical(&t);
    break;
  default:
    mode = 0;
    break;
  }
  FastLED.show();
  FastLED.delay(1);
  // delay(998);

  // leds[0] = CRGB::Blue;
  // FastLED.show();
  // FastLED.delay(1000);

  // leds[0] = CRGB::Black;
  // FastLED.show();
  // FastLED.delay(1000);

  // put your main code here, to run repeatedly:
}
void minimalClock(Time *now)
{
  unsigned char hourPos = (now->hour % 12) * 5;
  leds[(hourPos + LEDOffset) % 60].r = 255;
  leds[(now->min + LEDOffset) % 60].g = 255;
  leds[(now->sec + LEDOffset) % 60].b = 255;
  FastLED.show();
}
void basicClock(Time *now)
{
  unsigned char hourPos = ((now->hour % 12) * 5 + (now->min + 6) / 12);
  leds[(hourPos + LEDOffset + 59) % 60].r = 255;
  leds[(hourPos + LEDOffset) % 60].r = 255;
  leds[(hourPos + LEDOffset + 1) % 60].r = 255;
  leds[(now->min + LEDOffset) % 60].g = 255;
  leds[now->sec + LEDOffset].b = 255;
  FastLED.show();
}

void smoothSecond(Time *now)
{
  if (now->sec != old.sec)
  {
    old = *now;
    cyclesPerSec = millis() - newSecTime;
    cyclesPerSecFloat = (float)cyclesPerSec;
    newSecTime = millis();
  }
  fracOfSec = (millis() - newSecTime) / cyclesPerSecFloat;
  if (subSeconds < cyclesPerSec)
  {
    secondBrightness = 50.0 * (1.0 + sin((3.1415 * fracOfSec) - 1.57));
  }
  if (subSeconds < cyclesPerSec)
  {
    secondBrightness2 = 50.0 * (1.0 + sin((3.1415 * fracOfSec) + 1.57));
  }

  unsigned char hourPos = ((now->hour % 12) * 5) + (now->min + 6) / 12;
  leds[(hourPos + LEDOffset + 59) % 60].r = 255;
  leds[(hourPos + LEDOffset) % 60].r = 255;
  leds[(hourPos + LEDOffset + 1) % 60].r = 255;
  leds[(now->min + LEDOffset) % 60].g = 255;
  leds[(now->sec + LEDOffset) % 60].b = secondBrightness;
  leds[(now->sec + LEDOffset + 59) % 60].b = secondBrightness2;
  FastLED.show();
}

void outlineClock(Time *now)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    fiveMins = i % 5;
    if (fiveMins == 0)
    {
      leds[i].r = 100;
      leds[i].g = 100;
      leds[i].b = 100;
    }
  }
  unsigned char hourPos = ((now->hour % 12) * 5 + (now->min + 6) / 12);
  leds[(hourPos + LEDOffset + 59) % 60].r = 255;
  leds[(hourPos + LEDOffset) % 60].r = 255;
  leds[(hourPos + LEDOffset + 1) % 60].r = 255;
  leds[(now->min + LEDOffset) % 60].g = 255;
  leds[(now->sec + LEDOffset) % 60].b = 255;
  FastLED.show();
}

void minimalMilliSec(Time *now)
{
  if (now->sec != old.sec)
  {
    old = *now;
    cyclesPerSec = (millis() - newSecTime);
    newSecTime = millis();
  }
  // set hour, min & sec LEDs
  unsigned char hourPos = ((now->hour % 12) * 5 + (now->min + 6) / 12);
  subSeconds = (((millis() - newSecTime) * 60) / cyclesPerSec) % 60; // This divides by 733, but should be 1000 and not sure why???
  // Millisec lights are set first, so hour/min/sec lights override and don't flicker as millisec passes
  leds[(subSeconds + LEDOffset) % 60].r = 50;
  leds[(subSeconds + LEDOffset) % 60].g = 50;
  leds[(subSeconds + LEDOffset) % 60].b = 50;
  // The colours are set last, so if on same LED mixed colours are created
  leds[(hourPos + LEDOffset + 59) % 60].r = 255;
  leds[(hourPos + LEDOffset) % 60].r = 255;
  leds[(hourPos + LEDOffset + 1) % 60].r = 255;
  leds[(now->min + LEDOffset) % 60].g = 255;
  leds[(now->sec + LEDOffset) % 60].b = 255;
  FastLED.show();
}
void simplePendulum(Time *now)
{
  if (now->sec != old.sec)
  {
    old = *now;
    cyclesPerSec = millis() - newSecTime;
    cyclesPerSecFloat = (float)cyclesPerSec;
    newSecTime = millis();
    if (swingBack == true)
    {
      swingBack = false;
    }
    else
    {
      swingBack = true;
    }
  }
  fracOfSec = (millis() - newSecTime) / cyclesPerSecFloat;
  if (subSeconds < cyclesPerSec && swingBack == true)
  {
    pendulumPos = 27.0 + 3.4 * (1.0 + sin((3.14 * fracOfSec) - 1.57));
  }
  if (subSeconds < cyclesPerSec && swingBack == false)
  {
    pendulumPos = 27.0 + 3.4 * (1.0 + sin((3.14 * fracOfSec) + 1.57));
  }
  unsigned char hourPos = ((now->hour % 12) * 5 + (now->min + 6) / 12);
  // Pendulum lights are set first, so hour/min/sec lights override and don't flicker as millisec passes
  leds[(pendulumPos + LEDOffset) % 60].r = 100;
  leds[(pendulumPos + LEDOffset) % 60].g = 100;
  leds[(pendulumPos + LEDOffset) % 60].b = 100;
  // The colours are set last, so if on same LED mixed colours are created
  leds[(hourPos + LEDOffset + 59) % 60].r = 255;
  leds[(hourPos + LEDOffset) % 60].r = 255;
  leds[(hourPos + LEDOffset + 1) % 60].r = 255;
  leds[(now->min + LEDOffset) % 60].g = 255;
  leds[(now->sec + LEDOffset) % 60].b = 255;
  FastLED.show();
}

void breathingClock(Time *now)
{
  breathBrightness = 15.0 * (1.0 + sin((3.14 * millis() / 2000.0) - 1.57));
  for (int i = 0; i < NUM_LEDS; i++)
  {
    fiveMins = i % 5;
    if (fiveMins == 0)
    {
      leds[i].r = breathBrightness + 5;
      leds[i].g = breathBrightness + 5;
      leds[i].b = breathBrightness + 5;
    }
    else
    {
      leds[i].r = 0;
      leds[i].g = 0;
      leds[i].b = 0;
    }
  }
  unsigned char hourPos = ((now->hour % 12) * 5 + (now->min + 6) / 12);
  leds[(hourPos + LEDOffset + 59) % 60].r = 255;
  leds[(hourPos + LEDOffset) % 60].r = 255;
  leds[(hourPos + LEDOffset + 1) % 60].r = 255;
  leds[(now->min + LEDOffset) % 60].g = 255;
  leds[(now->sec + LEDOffset) % 60].b = 255;
  FastLED.show();
}
void modeHysterical(Time *now)
{
  for (int i = 4; i < NUM_LEDS; i++)
  {
    for (int j = i - 4; j <= i; j++)
    {
      leds[(now->sec + j) % 60] = CHSV(HUE_BLUE, 255, 255);
    }
    leds[now->hour] = CHSV((HUE_RED % 255), 255, 255);
    leds[now->min] = CHSV(HUE_GREEN, 255, 255);
    for (int j = 0; j < 60; j++)
      if (random(10) == 1 && j != now->hour && j != now->min)
        leds[j] = leds[j].fadeToBlackBy(100);
    FastLED.show();
    delay(7);
  }
  for (int i = 0; i < 30; i++)
  {
    leds[now->hour] = CHSV(HUE_RED % 255, 255, 255);
    leds[now->min] = CHSV(HUE_GREEN, 255, 255);
    leds[now->sec] = CHSV(HUE_BLUE, 255, 255);
    for (int j = 0; j < NUM_LEDS; j++)
      if (j != now->hour && j != now->min && j != now->sec)
        leds[j] = leds[j].fadeToBlackBy(40);
    FastLED.show();
    delay(6);
  }
  for (int i = 0; i < NUM_LEDS; i++)
    if (i != now->hour && i != now->min && i != now->sec)
      leds[i] = 0;
  FastLED.show();
  FastLED.delay(3);
}

void bounce(void)
{
  noInterrupts();
  button_state = digitalRead(BUTTON);
  if (button_state == HIGH)
  {
    mode++;
  }
  else
  {
  }
  interrupts();
}
