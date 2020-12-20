//#include "bsp_mh_z19b.h"
//
//#include "usart.h"
//
//uint8_t tx_array[9] =
//{ 0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79 };
//uint8_t rx_array[9] =
//{ 0x00 };
//
//void BSP_CO2_Tx (uint16_t *CO2Data)
//{
//	uint16_t co2_value;
//	HAL_UART_Transmit (&huart2, tx_array, 9, 100);
//	HAL_UART_Receive (&huart2, rx_array, 9, 100);
//	if (getCheckSum() == rx_array[8])
//	{
//		co2_value = (uint16_t) rx_array[2] * 256 + (uint16_t) rx_array[3];
//	}
//	if(co2_value > 9999)
//	{
//		co2_value=0;
//	}
//	*CO2Data = co2_value;
//
//
//}
//
//
//uint8_t getCheckSum()
//{
//	uint8_t i, checksum = 0;
//	for (i = 1; i < 8; i++)
//	{
//		checksum += rx_array[i];
//	}
//	checksum = 0xff - checksum;
//	checksum += 1;
//	return checksum;
//
//}
//
