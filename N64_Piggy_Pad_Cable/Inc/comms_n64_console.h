#ifndef COMMS_N64_CONSOLE_H_
#define COMMS_N64_CONSOLE_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "io_mapping_stm32f411ce_blackpill_weactstudio_v3_0.h"

// Macros
/* The below macros help to translate a uart byte to n64 bits. For
 * example, when looking at the n64 controller as a device that has
 * built in registers, the structure will look as follows
 * (remember, the controller responds with 4 bytes when being polled):
 *
 *  BIT #: 7     | 6       | 5 | 4    | 3  | 2  | 1 | 0
 * BYTE 1: A     | B       | Z | START| DU | DD | DL| DR
 * BYTE 2: RESET | RESERVED| L | R    | CU | CD | CL| CR
 * BYTE 3:                  X AXIS
 * BYTE 4:                  Y AXIS
 *
 * It's worth noting that the n64 controller sends the information
 * from bit 7 to bit 0, as defined above.
 *
 * If you acquire the controller response and look at the controllerResponse
 * array, you can view each element as two n64 bits. The elements of the array
 * are mapped to the n64 registers as follows:
 *
 * controllerResponse[0] = A & B
 * controllerResponse[1] = Z & START
 * controllerResponse[2] = DU & DD
 * controllerResponse[3] = DL & DR
 * controllerResponse[4] = RESET & RESERVED
 * controllerResponse[5] = L & R
 * controllerResponse[6] = CU & CD
 * controllerResponse[7] = CL & CR
 * controllerResponse[8] = X AXIS BIT 7 & X AXIS BIT 6
 * controllerResponse[9] = X AXIS BIT 5 & X AXIS BIT 4
 * controllerResponse[10] = X AXIS BIT 3 & X AXIS BIT 2
 * controllerResponse[11] = X AXIS BIT 1 & X AXIS BIT 0
 * controllerResponse[12] = Y AXIS BIT 7 & X AXIS BIT 6
 * controllerResponse[13] = Y AXIS BIT 5 & X AXIS BIT 4
 * controllerResponse[14] = Y AXIS BIT 3 & X AXIS BIT 2
 * controllerResponse[15] = Y AXIS BIT 1 & X AXIS BIT 0
 *
 * Taking controllerResponse[1] as an example let's say you find a
 * 0xE8 or 0xC8. The macros hints at the binary state of 01. This means
 * Z = 0 & START = 1. Or Z = RELEASED & START = PUSHED. The reason for
 * two different cases per binary combination is because since this
 * code uses a uart to send and receive n64 data, there are edge cases
 * in sampling that don't always give a consistent value. If the baud rate
 * was set appropriately, this would not be the case. But I was not able
 * to find an ideal baud rate. But fortunately the cases are unique to
 * their respective binary combinations. This makes parsing possible
 * to successful populate the controllerRegisters array.
 */
#define UART_BYTE_2_N64_BITS_00_CASE1	0x08
#define UART_BYTE_2_N64_BITS_00_CASE2	0x88
#define UART_BYTE_2_N64_BITS_01_CASE1	0xE8
#define UART_BYTE_2_N64_BITS_01_CASE2	0xC8
#define UART_BYTE_2_N64_BITS_10_CASE1	0x0F
#define UART_BYTE_2_N64_BITS_10_CASE2	0x8F
#define UART_BYTE_2_N64_BITS_11_CASE1	0xEF
#define UART_BYTE_2_N64_BITS_11_CASE2	0xCF
#define MAX_CONTROLLER_RESPONSE			16
#define MAX_CONTROLLER_REGISTER			4

// Module specific enums
typedef enum
{
	N64_NOPULL = 0,
	N64_PULLUP
} N64PullupState_t;

// Private Variables
static UART_HandleTypeDef huart1;

/* The controllerRepsonse array is what the uart received from the controller.
 * This array needs to be parsed properly to convert the uart bytes into n64 bytes
 * in order to make any sense of it. To understand how this is structured, look
 * at the macros section of this file.
 */
static uint8_t controllerResponse[MAX_CONTROLLER_RESPONSE];

/* The controllerRegisters array is the result after parsing the controllerResponse
 * array. Here is the n64 register structure for convenience:
 *
 *  BIT #: 7     | 6       | 5 | 4    | 3  | 2  | 1 | 0
 * BYTE 1: A     | B       | Z | START| DU | DD | DL| DR
 * BYTE 2: RESET | RESERVED| L | R    | CU | CD | CL| CR
 * BYTE 3:                  X AXIS
 * BYTE 4:                  Y AXIS
 *
 * Here is the mapping from the controllerResponse array to the n64 registers:
 *
 * controllerRegisters[0] = BYTE 1
 * controllerRegisters[1] = BYTE 2
 * controllerRegisters[2] = BYTE 3
 * controllerRegisters[3] = BYTE 4
 *
 * This array structure maintains easy readability when comparing to the
 * actual n64 data response when using a logic analyzer and is easy
 * to grab input states by bitwise manipulation.
 */
uint8_t controllerRegisters[MAX_CONTROLLER_REGISTER];

// Initializer
void CommsN64Console_Init(void);

// Parsers
void CommsN64Console_ParseContollerInputs(void);

// Setters
void CommsN64Console_SetPullup(N64PullupState_t);
inline void CommsN64Console_SendStopBit(void);

// Getters
inline void CommsN64Console_GetContollerStatus(void);
inline void CommsN64Console_GetContollerInputs(void);

#endif
