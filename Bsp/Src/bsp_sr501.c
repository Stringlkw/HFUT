/**
 ******************************************************************************
 * @file    bsp_exti.c
 * @author  fire
 * @version V1.0
 * @date    2013-xx-xx
 * @brief   I/O线中断应用bsp
 ******************************************************************************
 * @attention
 *
 * 实验平台:野火 F103-霸道 STM32 开发板
 * 论坛    :http://www.firebbs.cn
 * 淘宝    :https://fire-stm32.taobao.com
 *
 ******************************************************************************
 */

#include "bsp_sr501.h"

//引脚定义
#define SR501_INT_PORT         GPIOA
#define SR501_INT_PIN          GPIO_PIN_8
#define SR501_CLK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE()

#define SR501_INT_EXTI_IRQ          EXTI9_5_IRQn

/**
 * @brief  配置 IO为EXTI中断口，并设置中断优先级
 * @param  无
 * @retval 无
 */
void BSP_SR501_EXTI_Config()
{
	GPIO_InitTypeDef GPIO_InitStruct =
	{ 0 };

	/*开启SR501 GPIO口的时钟*/
	SR501_CLK_ENABLE();

	/*--------------------------GPIO配置-----------------------------*/
	/* 选择按键用到的GPIO */
	GPIO_InitStruct.Pin = SR501_INT_PIN;
	/* 配置为浮空输入 */
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(SR501_INT_PORT, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(SR501_INT_EXTI_IRQ, 15, 0);
	HAL_NVIC_EnableIRQ(SR501_INT_EXTI_IRQ);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);

}
/*********************************************END OF FILE**********************/

