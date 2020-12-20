#ifndef BSP_OLED_H_
#define BSP_OLED_H_

/********************************************************
 INCLUDES
 ********************************************************/
#include "stm32f1xx_hal.h"

/********************************************************
 用户自定义类型
 ********************************************************/
typedef enum
{
	DC = 0, CS = 1, SDA = 2, SCL = 3
} OLED_INDEX;

/********************************************************
 本模块提供给外部调用的函数
 ********************************************************/
void BSP_OLED_CLS(); //清屏
void BSP_OLED_Config();//OLED显示屏初始化
void BSP_OLED_Small_Str(uint8_t x, uint8_t y, uint8_t ch[]);//显示6x8的ASCII字符
void BSP_OLED_Big_Str(uint8_t x, uint8_t y, uint8_t ch[]);//显示8x16的ASCII字符
void BSP_OLED_Chinese(uint8_t x, uint8_t y, uint8_t N);//显示16x16中文汉字,汉字要先在取模软件中取模
void BSP_OLED_BMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[]);//全屏显示128*64的BMP图片

#endif /* BSP_OLED_H_ */
/********************(END OF FILE)***********************/
