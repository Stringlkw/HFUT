#ifndef _BSP_HDC1080_H
#define _BSP_HDC1080_H

#include "stm32f1xx_hal.h"


#define         HDC1080_ADDR              0x80
#define         HDC1080_WRITE_ADDR        0x80
#define         HDC1080_READ_ADDR         0x81
#define         HDC1080_CONFIGVLUE       0x1000


#define HDC1080_TEMPERATURE   0x00  //温度输出寄存器
#define HDC1080_HUMIDITY      0x01  //湿度输出寄存器
#define HDC1080_CONFIG        0x02  //配置寄存器


void BSP_HDC1080_Init();

void BSP_HDC1080_Read_TandH(uint8_t reg, float *temperature, float *humidity);

#endif

