/*
 * bspbeep.cpp
 *
 *  Created on: 2020年7月14日
 *      Author: lkw
 */

#include "stm32f1xx_hal.h"
#include "bsp_beep.h"
#include "bsp_systick.h"
/***************************************************************
 指示灯硬件接口定义
 ****************************************************************/
#define BEEP_PIN					GPIO_PIN_0		//PB5
#define BEEP_PORT			GPIOC
#define BEEP_CLK_ENABLE()	__HAL_RCC_GPIOC_CLK_ENABLE()

/***************************************************************
 DS18B20函数宏定义
 ****************************************************************/
#define BEEP_SET() HAL_GPIO_WritePin(BEEP_PORT, BEEP_PIN, GPIO_PIN_SET);
#define BEEP_RESET() HAL_GPIO_WritePin(BEEP_PORT, BEEP_PIN, GPIO_PIN_RESET);

void BSP_BEEP_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };
	/* Enalbe the GPIO_BEEP Clock */
	BEEP_CLK_ENABLE();
	/* Configure the GPIO_BEEP pin */
	GPIO_InitStruct.Pin = BEEP_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(BEEP_PORT, &GPIO_InitStruct);
}

void BSP_BEEP_StartRing()
{


		BEEP_RESET();
		HAL_Delay(100);
		BEEP_SET();
		HAL_Delay(100);
}

void BSP_BEEP_StopRing()
{
	BEEP_RESET();
}
