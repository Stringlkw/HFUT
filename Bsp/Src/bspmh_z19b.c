//#include "bsp_mh_z19b.h"
//
//
//uint8_t tx_array[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
//uint8_t rx_array[9] = {0x00};
//
//void BSP_CO2_Read(uint16_t * concentration)
//{
//    uint16_t co2_concentration;
//    HAL_UART_Transmit(&huart1, tx_array, 9, 100);
////    HAL_Delay(2);
//    HAL_UART_Receive(&huart1, rx_array, 9, 100);
//    if (rx_array[1] == 0x86)
//    {
//        co2_concentration = (uint16_t)rx_array[2]*256 + (uint16_t)rx_array[3];
//    }
//    else
//    {
//        co2_concentration = 0;
//    }
//
//    *concentration = co2_concentration;
//}




