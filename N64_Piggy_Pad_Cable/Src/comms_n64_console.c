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
	USART1->DR = (0x08);

	// Make sure the transmit data register is empty before sending a byte
	while(!(USART1->SR & USART_SR_TXE)){};
	// Send a byte (2 n64 bits)
	USART1->DR = (0x08 & 0xFF);

	// Make sure the last uart byte transmission is complete
	while(!(USART1->SR & USART_SR_TC)){};
	CommsN64Console_SendStopBit();
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
	USART1->DR = (0x08);

	// Make sure the transmit data register is empty before sending a byte
	while(!(USART1->SR & USART_SR_TXE)){};
	// Send a byte (2 n64 bits)
	USART1->DR = (0xE8 & 0xFF);

	// Make sure the last uart byte transmission is complete
	while(!(USART1->SR & USART_SR_TC)){};
	CommsN64Console_SendStopBit();
}
