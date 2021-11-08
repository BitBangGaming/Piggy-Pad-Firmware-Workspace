#ifndef COMMS_N64_CONSOLE_H_
#define COMMS_N64_CONSOLE_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "io_mapping_stm32f411ce_blackpill_weactstudio_v3_0.h"

// Macros
#define OUTGOING_DATA_SIZE	1
#define COMMS_TIMEOUT_2MS	2

// Module specific enums
typedef enum
{
	N64_NOPULL = 0,
	N64_PULLUP
} N64PullupState_t;

// Private Variables
static UART_HandleTypeDef huart1;
static uint8_t controllerResponse[16];

// Initializer
void CommsN64Console_Init(void);

// Setters
void CommsN64Console_SetPullup(N64PullupState_t);
inline void CommsN64Console_SendStopBit(void);

// Getters
inline void CommsN64Console_GetContollerStatus(void);
inline void CommsN64Console_GetContollerInputs(void);

#endif
