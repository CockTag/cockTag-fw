#pragma once

#include "nrf_gpio.h"

// LEDs definitions for CockTag
#define LEDS_NUMBER 2
#define LEDS_ACTIVE_STATE 1
#define LED_0 1
#define LED_1 4
#define LEDS_LIST \
  { LED_0, LED_1 }
#define LEDS_INV_MASK LEDS_MASK

#define BUTTON_PIN 16
#define BUTTON_PULL NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 1

#define BUTTONS_LIST \
  { BUTTON_0 }

#define BUZZER 17

#define GYRO
#define GYRO_SDA 8
#define GYRO_SCL 9
#define GYRO_INT 10

#define HWFC true

// Low frequency clock source to be used by the SoftDevice

#define NRF_CLOCK_LFCLKSRC                                            \
  {                                                                   \
    .source = NRF_CLOCK_LF_SRC_XTAL, .rc_ctiv = 0, .rc_temp_ctiv = 0, \
    .xtal_accuracy = NRF_CLOCK_LF_XTAL_ACCURACY_20_PPM                \
  }
