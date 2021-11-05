#include "comms_n64_console.h"

// Initializer
void CommsN64Console_Init()
{
	// Set parameters for expansion port pins
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct_CommsN64Console = {0};

	// USART1 TX
	GPIO_InitStruct_CommsN64Console.Pin = COMMS_N64_TX_PIN_HAL;
	GPIO_InitStruct_CommsN64Console.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct_CommsN64Console.Alternate = GPIO_AF7_USART1;
	GPIO_InitStruct_CommsN64Console.Pull = GPIO_NOPULL;
	GPIO_InitStruct_CommsN64Console.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(COMMS_N64_TX_PORT, &GPIO_InitStruct_CommsN64Console);

	// USART1 RX
	GPIO_InitStruct_CommsN64Console.Pin = COMMS_N64_RX_PIN_HAL;
	GPIO_InitStruct_CommsN64Console.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct_CommsN64Console.Alternate = GPIO_AF7_USART1;
	GPIO_InitStruct_CommsN64Console.Pull = GPIO_NOPULL;
	GPIO_InitStruct_CommsN64Console.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(COMMS_N64_RX_PORT, &GPIO_InitStruct_CommsN64Console);

	// Configure USART1
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 19200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_UART_Init(&huart1);
}

void CommsN64Console_SendData(uint8_t tempData)
{
	// Setup array to send data
	uint8_t dataToSend[] = {tempData};

	// Always 1 byte to send with 1ms timeout
	HAL_UART_Transmit(&huart1, dataToSend, OUTGOING_DATA_SIZE, COMMS_TIMEOUT_2MS);
}
