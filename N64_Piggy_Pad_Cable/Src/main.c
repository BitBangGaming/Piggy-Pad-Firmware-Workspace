#include "stm32f4xx_hal.h"

// LED = PC13, bus AHB1EN bit0
int main(void)
{
		/* Quick test to make sure everything compiles okay in STM32CubeIDE */
	HAL_Init();

	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    while(1)
    {
    	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    	HAL_Delay(100);
    	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    	HAL_Delay(100);
    }
}

void SysTick_Handler(void)
{
	HAL_IncTick();
}
