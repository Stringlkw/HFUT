/*******************************************************
 * 模块名称 : 系统定时器驱动模块
 * 文件名	  : BSP_SYSTICK.H
 * 说明  	  : 头文件,提供自定义数据类型及外部调用的接口函数的声明
 * 版本号	  : v1.0
 * 修改记录 :
 * 版本号		日期		作者
 * v1.0	     2021-1-23	               李康伟
 ********************************************************/

#include "bsp_systick.h"

static __IO u32 TimingDelay;

/**
  * @brief  启动系统滴答定时器 SysTick
  * @param  无
  * @retval 无
  */
void BSP_SYSTICK_Init(void)
{
    /* SystemFrequency / 1000    1ms中断一次
     * SystemFrequency / 100000	 10us中断一次
     * SystemFrequency / 1000000 1us中断一次
     */
    if (HAL_SYSTICK_Config(SystemCoreClock / 100000))
    {
        /* Capture error */
        while (1);
    }
}

/**
  * @brief   us延时程序,10us为一个单位
  * @param
  *	@arg nTime: Delay_us( 1 ) 则实现的延时为 1 * 10us = 10us
  * @retval  无
  */
void BSP_Delay(__IO u32 nTime)
{
    TimingDelay = nTime;

    while (TimingDelay != 0);
}

/**
  * @brief  获取节拍程序
  * @param  无
  * @retval 无
  * @attention  在 SysTick 中断函数 SysTick_Handler()调用
  */
void TimingDelay_Decrement(void)
{
    if (TimingDelay != 0x00)
    {
        TimingDelay--;
    }
}
/*********************************************END OF FILE**********************/