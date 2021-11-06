#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "io_mapping_stm32f411ce_blackpill_weactstudio_v3_0.h"
#include "comms_n64_console.h"

// Private enums
static GPIO_PinState BLUE_LED_ON = GPIO_PIN_RESET;
static GPIO_PinState BLUE_LED_OFF = GPIO_PIN_SET;

// Initializer
void Main_Init(void);

// Setters
void Main_SetBlueLed(GPIO_PinState);

#endif
