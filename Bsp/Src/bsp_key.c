/*
 * bspkey.cpp
 *
 *  Created on: 2020年7月6日
 *      Author: lkw
 */

#include "bsp_key.h"

/***************************************************************
 按键硬件接口定义
 ****************************************************************/
#define KEYn			4				//按键数量
//指示灯0
#define KEY0_PIN					GPIO_PIN_0		//PA0
#define KEY0_PORT			GPIOA
#define KEY0_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()
//指示灯1
#define KEY1_PIN					GPIO_PIN_13		//PC13
#define KEY1_PORT			GPIOC
#define KEY1_CLK_ENABLE()	__HAL_RCC_GPIOC_CLK_ENABLE()
//指示灯2
#define KEY2_PIN					GPIO_PIN_0		//PA0
#define KEY2_PORT			GPIOA
#define KEY2_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()
//指示灯3
#define KEY3_PIN					GPIO_PIN_0	//PA0
#define KEY3_PORT			GPIOA
#define KEY3_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()
//端口数组，数组中存放的都是按键控制引脚所属端口
GPIO_TypeDef *KEY_PORT[KEYn] =
{
KEY0_PORT,
KEY1_PORT,
KEY2_PORT,
KEY3_PORT};
//引脚数组，数组中存放的都是按键控制引脚
const uint16_t KEY_PIN[KEYn] =
{ KEY0_PIN, KEY1_PIN, KEY2_PIN, KEY3_PIN, };
//端口时钟使能宏定义
#define KEYx_CLK_ENABLE(__INDEX__) do{\
					    if((__INDEX__) == 0)			\
						    KEY0_CLK_ENABLE();		\
					    else if((__INDEX__) == 1)		\
						    KEY1_CLK_ENABLE();		\
					    else if((__INDEX__) == 2)		\
						    KEY2_CLK_ENABLE();		\
					    else if((__INDEX__) == 3)		\
						    KEY3_CLK_ENABLE();		\
}while(0)




/***************************************************************
 本模块提供给外部调用的函数
 ****************************************************************/
/***************************************************************
 * @name			BSP_KEY_Init
 * @brief		Configures KEY GPIO.
 * @param[in]	Key: Specifies the Key to be configured.
 * 				This parameter can be one of following parameters:
 * @arg			KEY0~KEY3
 * @return		None
 * @note
 ****************************************************************/
void BSP_KEY_Init(KEY_INDEX Key)
{
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };
	/* Enalbe the GPIO_KEY Clock */
	KEYx_CLK_ENABLE(Key);
	/* Configure the GPIO_LED pin */
	GPIO_InitStruct.Pin = KEY_PIN[Key];
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(KEY_PORT[Key], &GPIO_InitStruct);
}


void BSP_KEY_Config()
{
	BSP_KEY_Init(KEY0);
	BSP_KEY_Init(KEY1);
}

uint8_t BSP_KEY_Scan(KEY_INDEX Key)
{
	if (HAL_GPIO_ReadPin(KEY_PORT[Key], KEY_PIN[Key]) == KEY_ON) 	//检测按键有效标志
	{
		//松手检测
		while(HAL_GPIO_ReadPin(KEY_PORT[Key], KEY_PIN[Key]) == KEY_ON);
		return KEY_ON;
	}
	else
	{
		return KEY_OFF;
	}
}


