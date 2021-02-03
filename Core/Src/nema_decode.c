#include "stm32f1xx_hal.h"
#include "bsp_gps.h"
#include "nmea/nmea.h"
#include "stdio.h"
#include "bsp_oled.h"


/**
  * @brief  NmeaDecode ����GPSģ����Ϣ
  * @param  ��
  * @retval ��
  */
int NmeaDecode(void)
{
    double deg_lat;//ת����[degree].[degree]��ʽ��γ��
    double deg_lon;//ת����[degree].[degree]��ʽ�ľ���

    nmeaINFO info;          //GPS�����õ�����Ϣ
    nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ
    uint8_t new_parse = 0;    //�Ƿ����µĽ������ݱ�־

    nmeaTIME beiJingTime;    //����ʱ��

    char str_buff[100];

    /* ��ʼ��GPS���ݽṹ */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    while (1)
    {
        if (GPS_HalfTransferEnd)     /* ���յ�GPS_RBUFF_SIZEһ������� */
        {
            /* ����nmea��ʽ���� */
            nmea_parse(&parser, (const char *) &gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);

            GPS_HalfTransferEnd = 0;   //��ձ�־λ
            new_parse = 1;             //���ý�����Ϣ��־
        }
        else if (GPS_TransferEnd)    /* ���յ���һ������ */
        {

            nmea_parse(&parser, (const char *) &gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);

            GPS_TransferEnd = 0;
            new_parse = 1;
        }

        if (new_parse)                //���µĽ�����Ϣ
        {
            /* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
            gmtConvert(&info.utc, &beiJingTime, 8, 1);
            //info.lat lon�еĸ�ʽΪ[degree][min].[sec/60]��ʹ�����º���ת����[degree].[degree]��ʽ
            deg_lat = nmea_ndeg2degree(info.lat);
            deg_lon = nmea_ndeg2degree(info.lon);


            /* ��ʾʱ������ */
            sprintf(str_buff, " Date:%4d/%02d/%02d ", beiJingTime.year + 1900, beiJingTime.mon, beiJingTime.day);
            BSP_OLED_Small_Str(0,0,str_buff);

            sprintf(str_buff, " Time:%02d:%02d:%02d", beiJingTime.hour, beiJingTime.min, beiJingTime.sec);
            BSP_OLED_Small_Str(2,0,str_buff);

            /* γ�� ����*/
            sprintf(str_buff, " latitude :%.6f ", deg_lat);
            BSP_OLED_Small_Str(4,0,str_buff);

            sprintf(str_buff, " longitude :%.6f", deg_lon);
            BSP_OLED_Small_Str(6,0,str_buff);

            /* �ٶ� */
            sprintf(str_buff, " speed:%4.2f km/h", info.speed);
            BSP_OLED_Small_Str(8,0,str_buff);

            new_parse = 0;
        }
    }

    /* �ͷ�GPS���ݽṹ */
    nmea_parser_destroy(&parser);
    return 0;
}








/**************************************************end of file****************************************/

