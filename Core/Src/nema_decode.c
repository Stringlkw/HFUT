#include "stm32f1xx_hal.h"
#include "bsp_gps.h"
#include "nmea/nmea.h"
#include "stdio.h"
#include "bsp_oled.h"


/**
  * @brief  NmeaDecode 解码GPS模块信息
  * @param  无
  * @retval 无
  */
int NmeaDecode(void)
{
    double deg_lat;//转换成[degree].[degree]格式的纬度
    double deg_lon;//转换成[degree].[degree]格式的经度

    nmeaINFO info;          //GPS解码后得到的信息
    nmeaPARSER parser;      //解码时使用的数据结构
    uint8_t new_parse = 0;    //是否有新的解码数据标志

    nmeaTIME beiJingTime;    //北京时间

    char str_buff[100];

    /* 初始化GPS数据结构 */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

    while (1)
    {
        if (GPS_HalfTransferEnd)     /* 接收到GPS_RBUFF_SIZE一半的数据 */
        {
            /* 进行nmea格式解码 */
            nmea_parse(&parser, (const char *) &gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);

            GPS_HalfTransferEnd = 0;   //清空标志位
            new_parse = 1;             //设置解码消息标志
        }
        else if (GPS_TransferEnd)    /* 接收到另一半数据 */
        {

            nmea_parse(&parser, (const char *) &gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);

            GPS_TransferEnd = 0;
            new_parse = 1;
        }

        if (new_parse)                //有新的解码消息
        {
            /* 对解码后的时间进行转换，转换成北京时间 */
            gmtConvert(&info.utc, &beiJingTime, 8, 1);
            //info.lat lon中的格式为[degree][min].[sec/60]，使用以下函数转换成[degree].[degree]格式
            deg_lat = nmea_ndeg2degree(info.lat);
            deg_lon = nmea_ndeg2degree(info.lon);


            /* 显示时间日期 */
            sprintf(str_buff, " Date:%4d/%02d/%02d ", beiJingTime.year + 1900, beiJingTime.mon, beiJingTime.day);
            BSP_OLED_Small_Str(0,0,str_buff);

            sprintf(str_buff, " Time:%02d:%02d:%02d", beiJingTime.hour, beiJingTime.min, beiJingTime.sec);
            BSP_OLED_Small_Str(2,0,str_buff);

            /* 纬度 经度*/
            sprintf(str_buff, " latitude :%.6f ", deg_lat);
            BSP_OLED_Small_Str(4,0,str_buff);

            sprintf(str_buff, " longitude :%.6f", deg_lon);
            BSP_OLED_Small_Str(6,0,str_buff);

            /* 速度 */
            sprintf(str_buff, " speed:%4.2f km/h", info.speed);
            BSP_OLED_Small_Str(8,0,str_buff);

            new_parse = 0;
        }
    }

    /* 释放GPS数据结构 */
    nmea_parser_destroy(&parser);
    return 0;
}








/**************************************************end of file****************************************/

