/*******************************************************
 * 模块名称 : 按键状态机驱动模块
 * 文件名	  : BSPKEY.H
 * 说明  	  : 头文件,提供自定义数据类型及外部调用的接口函数的声明
 * 版本号	  : v1.0
 * 修改记录 :
 * 	版本号		日期		作者
 * 	v1.0	     2020-7-6	               李康伟
 ********************************************************/

#ifndef BSP_KEY_BSPKEY_H_
#define BSP_KEY_BSPKEY_H_

#include "stm32f1xx_hal.h"
/********************************************************
 用户自定义类型
 ********************************************************/

typedef enum
{
	KEY0 = 0,
	KEY1 = 1,
	KEY2 = 2,
	KEY3 = 3
} KEY_INDEX;


/***************************************************************
 按键状态定义
 ****************************************************************/
#define KEY_ON 1
#define KEY_OFF 0


/********************************************************
 本模块提供给外部调用的函数
 ********************************************************/
void BSP_KEY_Init(KEY_INDEX Key);
void BSP_KEY_Config();	//多个按键初始化
uint8_t BSP_KEY_Scan(KEY_INDEX Key);
#endif /* BSP_KEY_BSPKEY_H_ */
/********************(END OF FILE)***********************/