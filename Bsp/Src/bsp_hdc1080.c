#include <i2c.h>
#include "bsp_hdc1080.h"
#include "stm32f1xx_hal_i2c.h"


uint16_t BSP_HDC1080_Read(uint8_t reg)
{
    uint8_t hdcTemp[2];
    uint16_t tempReturn;
    hdcTemp[0] = reg;
    if (HAL_I2C_Master_Transmit(&hi2c1, HDC1080_ADDR, &hdcTemp[0], 1, 1000) == HAL_OK) //写设备地址
    {
        if (HAL_I2C_Master_Receive(&hi2c1, HDC1080_ADDR, &hdcTemp[0], 2, 1000) == HAL_OK)
        {
            tempReturn = (hdcTemp[0] << 8) | hdcTemp[1];
            return tempReturn;
        }
        else
        {
            return 0xFFFF;  //返回错误标志
        }
    }
    else
    {
        return 0xFFFF;
    }
}

void BSP_HDC1080_Write(uint8_t reg, uint16_t data)
{
    uint8_t hdcTemp[3];
    hdcTemp[0] = reg;
    hdcTemp[1] = data >> 8;
    hdcTemp[2] = data;
    HAL_I2C_Master_Transmit(&hi2c1, HDC1080_ADDR, &hdcTemp[0], 3, 1000);    //写设备地址
}


void BSP_HDC1080_Init()
{
    BSP_HDC1080_Write(HDC1080_CONFIG, HDC1080_CONFIGVLUE);
}


void BSP_HDC1080_Read_TandH(uint8_t reg, float *temperature, float *humidity)
{
    uint8_t hdcTemp[4];
    hdcTemp[0] = reg;
    if (HAL_I2C_Master_Transmit(&hi2c1, HDC1080_ADDR, &hdcTemp[0], 1, 1000) == HAL_OK)
    {
        HAL_Delay(20);
        if (HAL_I2C_Master_Receive(&hi2c1, HDC1080_ADDR, &hdcTemp[0], 4, 1000) == HAL_OK)
        {
            *temperature = (hdcTemp[0] << 8) | hdcTemp[1];
            *temperature = *temperature / 65536 * 165 - 40;
            *humidity = (hdcTemp[2] << 8) | hdcTemp[3];
            *humidity = *humidity / 65536 * 100;
        }
    }
}


