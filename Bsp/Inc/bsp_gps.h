#ifndef __GPS_CONFIG_H
#define	__GPS_CONFIG_H

#include "stm32f1xx_hal.h"
#include "nmea/nmea.h"


#define GPS_RBUFF_SIZE            512                   //���ڽ��ջ�������С
#define HALF_GPS_RBUFF_SIZE       (GPS_RBUFF_SIZE/2)    //���ڽ��ջ�����һ��

/* �����־ */
#define GPS_DMA_FLAG_TC              DMA_FLAG_TC6
#define GPS_DMA_FLAG_TE              DMA_FLAG_TE6
#define GPS_DMA_FLAG_HT              DMA_FLAG_HT6

extern uint8_t gps_rbuff[GPS_RBUFF_SIZE];
extern __IO uint8_t GPS_TransferEnd ;
extern __IO uint8_t GPS_HalfTransferEnd;

void gmtConvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT, uint8_t AREA) ;
#endif