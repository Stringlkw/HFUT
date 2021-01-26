/***************************************************************
 * 模块名称 : 指示灯驱动模块
 * 文件名	  : BSP_LED.C
 * 说明  	  : 驱动LED指示灯
 * 版本号	  : v1.0
 * 修改记录 :
 * 	版本号	        日期	                  作者
 *      v1.0	 2020-4-16          李康伟
 ***************************************************************/

/***************************************************************
 模块移植性说明
 ****************************************************************/
/*
 * 1、如果用户的LED指示灯个数小于等于四个，可以将多余指示灯的引脚定
 * 	   义全部定义为和第一个指示灯一样，其余代码不做任何修改
 * 2、如果用户的LED指示灯个数大于4个,需要修改如下内容:
 *	  (1)增加.h文件中LED_INDEX类型的枚举常量
 *	  (2)修改指示灯数量定义LEDn
 *	  (3)修改指示灯控制引脚的定义
 *	  (4)修改端口和引脚两个数组:GPIO_PORT[LEDn]和GPIO_PIN[LEDn]
 *	  (5)修改引脚的时钟控制宏定义LEDx_GPIO_CLK_ENALBE(__INDEX__)的内容
 */

/***************************************************************
 INCLUDES
 ****************************************************************/
#include "bsp_led.h"
#include "stm32f1xx_hal.h"
/***************************************************************
 指示灯硬件接口定义
 ****************************************************************/
#define LEDn			4				//指示灯数量
//指示灯0
#define LED0_PIN					GPIO_PIN_5		//PB5
#define LED0_PORT			GPIOB
#define LED0_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
//指示灯1
#define LED1_PIN					GPIO_PIN_0		//PB0
#define LED1_PORT			GPIOB
#define LED1_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
//指示灯2
#define LED2_PIN					GPIO_PIN_1		//PB1
#define LED2_PORT			GPIOB
#define LED2_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
//指示灯3
#define LED3_PIN					GPIO_PIN_5		//PB5
#define LED3_PORT			GPIOB
#define LED3_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
//端口数组，数组中存放的都是指示灯控制引脚所属端口
GPIO_TypeDef *LED_PORT[LEDn] =
{
LED0_PORT,
LED1_PORT,
LED2_PORT,
LED3_PORT };
//引脚数组，数组中存放的都是指示灯控制引脚
const uint16_t LED_PIN[LEDn] =
{
LED0_PIN,
LED1_PIN,
LED2_PIN,
LED3_PIN, };
//端口时钟使能宏定义
#define LEDx_CLK_ENABLE(__INDEX__) do{\
					    if((__INDEX__) == 0)			\
						    LED0_CLK_ENABLE();		\
					    else if((__INDEX__) == 1)		\
						    LED1_CLK_ENABLE();		\
					    else if((__INDEX__) == 2)		\
						    LED2_CLK_ENABLE();		\
					    else if((__INDEX__) == 3)		\
						    LED3_CLK_ENABLE();		\
}while(0)

/***************************************************************
 本模块提供给外部调用的函数
 ****************************************************************/
/***************************************************************
 * @name			BSP_LED_Init
 * @brief		Init LED GPIO.
 * @param[in]	ledIndex: Specifies the ledIndex to be configured.
 * 				This parameter can be one of following parameters:
 * @arg			LED0~LED3
 * @return		None
 * @note
 ****************************************************************/
void BSP_LED_Init(LED_INDEX ledIndex)
{
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };
	/* Enalbe the GPIO_LED Clock */
	LEDx_CLK_ENABLE(ledIndex);
	/* Configure the GPIO_LED pin */
	GPIO_InitStruct.Pin = LED_PIN[ledIndex];
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_PORT[ledIndex], &GPIO_InitStruct);
	HAL_GPIO_WritePin(LED_PORT[ledIndex], LED_PIN[ledIndex], GPIO_PIN_SET);
}

/***************************************************************
 * @name			BSP_LED_Config
 * @brief		Configures LED GPIO.
 * @param[in]	Led: Specifies the Led to be configured.
 * 				This parameter can be one of following parameters:
 * @arg			LED0~LED3
 * @return		None
 * @note
 ****************************************************************/
void BSP_LED_Config()
{
	BSP_LED_Init(LED0);
	BSP_LED_Init(LED1);
	BSP_LED_Init(LED2);
}

/***************************************************************
 * @name			BSP_LED_On
 * @brief		Turns selected LED On
 * @param[in]	ledIndex: Specifies the ledIndex to be set on.
 * 				This parameter can be one of following parameters:
 * @arg			LED0~LED3
 * @return		None
 * @note
 ****************************************************************/
void BSP_LED_On(LED_INDEX ledIndex)
{
	HAL_GPIO_WritePin(LED_PORT[ledIndex], LED_PIN[ledIndex], GPIO_PIN_RESET);
}

/***************************************************************
 * @name			BSP_LED_Off
 * @brief		Turns selected LED Off
 * @param[in]	ledIndex: Specifies the ledIndex to be set off.
 * 				This parameter can be one of following parameters:
 * @arg			LED0~LED3
 * @return		None
 * @note
 ****************************************************************/
void BSP_LED_Off(LED_INDEX ledIndex)
{
	HAL_GPIO_WritePin(LED_PORT[ledIndex], LED_PIN[ledIndex], GPIO_PIN_SET);
}

/***************************************************************
 * @name			BSP_LED_Toggle
 * @brief		Toggles the selected LED.
 * @param[in]	ledIndex: Specifies the ledIndex to be toggled.
 * 				This parameter can be one of following parameters:
 * @arg			LED0~LED3
 * @return		None
 * @note
 ****************************************************************/
void BSP_LED_Toggle(LED_INDEX ledIndex)
{
	HAL_GPIO_TogglePin(LED_PORT[ledIndex], LED_PIN[ledIndex]);
}

