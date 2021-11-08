#include "comms_n64_console.h"

// Initializer
void CommsN64Console_Init()
{
	/* Set parameters for n64 communication pins */

	// Clocks
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();

	// Init structure
	GPIO_InitTypeDef GPIO_InitStruct_CommsN64Console = {0};

	// Stop bit control
	GPIO_InitStruct_CommsN64Console.Pin = COMMS_N64_STOP_PIN_HAL;
	GPIO_InitStruct_CommsN64Console.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct_CommsN64Console.Pull = GPIO_NOPULL;
	GPIO_InitStruct_CommsN64Console.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(COMMS_N64_STOP_PORT, &GPIO_InitStruct_CommsN64Console);
	COMMS_N64_STOP_PORT->BSRR = COMMS_N64_STOP_SET;

	// External pullup control
	GPIO_InitStruct_CommsN64Console.Pin = COMMS_N64_PULLUP_PIN_HAL;
	GPIO_InitStruct_CommsN64Console.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct_CommsN64Console.Pull = GPIO_NOPULL;
	GPIO_InitStruct_CommsN64Console.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(COMMS_N64_PULLUP_PORT, &GPIO_InitStruct_CommsN64Console);

	// USART1 TX/RX
	GPIO_InitStruct_CommsN64Console.Pin = COMMS_N64_TX_PIN_HAL | COMMS_N64_RX_PIN_HAL;
	GPIO_InitStruct_CommsN64Console.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct_CommsN64Console.Alternate = GPIO_AF7_USART1;
	GPIO_InitStruct_CommsN64Console.Pull = GPIO_NOPULL;
	GPIO_InitStruct_CommsN64Console.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(COMMS_N64_TX_PORT, &GPIO_InitStruct_CommsN64Console);

	// Configure USART1
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 1250000;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_8;

	HAL_UART_Init(&huart1);
}

void CommsN64Console_SetPullup(N64PullupState_t pull)
{
	if(pull == N64_NOPULL)
	{
		HAL_GPIO_WritePin(COMMS_N64_PULLUP_PORT, COMMS_N64_PULLUP_PIN_HAL, GPIO_PIN_RESET);
	}
	else if(pull == N64_PULLUP)
	{
		HAL_GPIO_WritePin(COMMS_N64_PULLUP_PORT, COMMS_N64_PULLUP_PIN_HAL, GPIO_PIN_SET);
	}
	else
	{
		// invalid argument
	}
}

void CommsN64Console_SendStopBit()
{
	/* The timing of the stop bit does not need to be so precise. */
	COMMS_N64_STOP_PORT->BSRR = COMMS_N64_STOP_CLEAR;
	volatile uint32_t counter = 1;
	while(counter--);
	COMMS_N64_STOP_PORT->BSRR = COMMS_N64_STOP_SET;
}

void CommsN64Console_GetContollerStatus()
{
	// Future implementation
}

void CommsN64Console_GetContollerInputs()
{
	/* Send send a byte of data to the n64, you must send 4 bytes of data from
	 * the usart1 module. This means that for every byte sent byte sent by
	 * usart1, 2 bits are transferred to the n64 data line. Note that at the
	 * end of the 4th byte sent from usart1, a stop bit must be sent. It is
	 * possible to send 0xFF from usart1 to achieve this but the n64 controller
	 * will respond before the usart1 transmission of 0xFF is complete. Instead
	 * we can tie another open drain output to the n64 data line. When the 4th
	 * usart1 byte is sent, we can toggle this additional open drain output. Doing
	 * so will act as a stop but required for the n64 protocol. After this is
	 * performed, there is about 2.5 microseconds left before the n64 controller
	 * begins to respond. For a STM32F411CE running at 100MHz, this be more than
	 * enough time to prepare a way to capture the data.
	 */

	// Disable the receiver
	USART1->CR1 &= ~USART_CR1_RE;

	// Make sure the transmit data register is empty before sending next byte
	while(!(USART1->SR & USART_SR_TXE)){};
	// Send a byte (2 n64 bits)
	USART1->DR = (0x08 & 0xFF);

	// Make sure the transmit data register is empty before sending a byte
	while(!(USART1->SR & USART_SR_TXE)){};
	// Send a byte (2 n64 bits)
	USART1->DR = (0x08 & 0xFF);

	// Make sure the transmit data register is empty before sending a byte
	while(!(USART1->SR & USART_SR_TXE)){};
	// Send a byte (2 n64 bits)
	USART1->DR = (0x08 & 0xFF);

	// Make sure the transmit data register is empty before sending a byte
	while(!(USART1->SR & USART_SR_TXE)){};
	// Send a byte (2 n64 bits)
	USART1->DR = (0xE8 & 0xFF);

	// Make sure the last uart byte transmission is complete before sending stop bit
	while(!(USART1->SR & USART_SR_TC)){};
	CommsN64Console_SendStopBit();

	/* Receive 4 n64 bytes from the controller. This means that our uart will
	 * have to receive 16 bytes because 1 uart byte = 2 n64 bits and therefore
	 * 4 uart bytes = 1 n64 byte. But remember, the controller will send a stop
	 * bit after this 4 n64 byte transmission. That means the uart receiver must
	 * be disabled after the last acquired byte. Of course before grabbing the
	 * first byte, enabled the uart receiver.
	 */
	// Enable the uart receiver
	USART1->CR1 |= USART_CR1_RE;
	controllerResponse[0] = USART1->DR;

	// Grab states for A and B
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[0] = USART1->DR;

	// Grab states for Z and START
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[1] = USART1->DR;

	// Grab states for DU and DD
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[2] = USART1->DR;

	// Grab states for DL and DR
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[3] = USART1->DR;

	// Grab states for RESET and RESERVED
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[4] = USART1->DR;

	// Grab states for L and R
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[5] = USART1->DR;

	// Grab states for CU and CD
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[6] = USART1->DR;

	// Grab states for CL and CR
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[7] = USART1->DR;

	// Grab states for X-AXIS BIT7 & BIT6
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[8] = USART1->DR;

	// Grab states for X-AXIS BIT5 & BIT4
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[9] = USART1->DR;

	// Grab states for X-AXIS BIT3 & BIT2
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[10] = USART1->DR;

	// Grab states for X-AXIS BIT1 & BIT0
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[11] = USART1->DR;

	// Grab states for Y-AXIS BIT7 & BIT6
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[12] = USART1->DR;

	// Grab states for Y-AXIS BIT5 & BIT4
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[13] = USART1->DR;

	// Grab states for Y-AXIS BIT3 & BIT2
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[14] = USART1->DR;

	// Grab states for Y-AXIS BIT1 & BIT0
	while(!(USART1->SR & USART_SR_RXNE)){};
	controllerResponse[15] = USART1->DR;

	// Disable the uart receiver
	USART1->CR1 &= ~USART_CR1_RE;

	// Temp code for debugging on logic analyzer
	HAL_Delay(1);
	while(!(USART1->SR & USART_SR_TXE)){};
	USART1->DR = (controllerResponse[3] & 0xFF);
	while(!(USART1->SR & USART_SR_TC)){};
	HAL_Delay(10);;
}
