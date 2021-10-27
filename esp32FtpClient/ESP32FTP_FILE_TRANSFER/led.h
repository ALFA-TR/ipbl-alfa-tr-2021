/** @file led.h
    @brief Function prototypes for led.

    This contains the prototypes for the Led
    and eventually any macros, constants,
    or global variables you will need.

    @author Dhananjay Khairnar
*/
#ifndef __LED_H
#define __LED_H
#include <Arduino.h>

#define LED_1_PIN 13
#define LED_2_PIN 12
#define LED_3_PIN 14
#define LED_4_PIN 27
#define LED_5_PIN 26
#define LED_6_PIN 25
#define LED_7_PIN 21
#define LED_8_PIN 22

/** Enum for LED LED numbers  */
typedef enum LED_NUM
{
  LED_1 = 1,
  LED_2,
  LED_3,
  LED_4,
  LED_5,
  LED_6,
  LED_7,
  LED_8
}LED_NUM;

void initLED();

void setLed(uint8_t led, uint8_t val);


#endif
