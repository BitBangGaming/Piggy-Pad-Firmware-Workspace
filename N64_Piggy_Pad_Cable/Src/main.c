#include "main.h"

int main(void)
{
	/* Initialize all required modules */
	HAL_Init();
	CommsN64Console_Init();
	Main_Init();

	/* Apply a pullup on n64 data line */
	CommsN64Console_SetPullup(N64_PULLUP);

	/* Main application loop */
    while(1)
    {
    	CommsN64Console_SendData(0xAA);
    	Main_SetBlueLed(BLUE_LED_ON);
    	HAL_Delay(50);
    	Main_SetBlueLed(BLUE_LED_OFF);
    	HAL_Delay(50);
    }
}

// Setters
void Main_SetBlueLed(GPIO_PinState pinState)
{
	HAL_GPIO_WritePin(BLUE_LED_PORT, BLUE_LED_PIN_HAL, pinState);
}

// Initializer
void Main_Init()
{
	/* Initialize the blue led */
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct_Main = {0};

	GPIO_InitStruct_Main.Pin = BLUE_LED_PIN_HAL;
	GPIO_InitStruct_Main.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct_Main.Pull = GPIO_NOPULL;
	GPIO_InitStruct_Main.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(BLUE_LED_PORT, &GPIO_InitStruct_Main);
}

// Misc
void SysTick_Handler(void)
{
	HAL_IncTick();
}