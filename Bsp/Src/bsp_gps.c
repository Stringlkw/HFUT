#include "bsp_gps.h"
#include "nmea/nmea.h"
#include "usart.h"


/* DMA���ջ���  */
uint8_t gps_rbuff[GPS_RBUFF_SIZE];

/* DMA���������־ */
__IO uint8_t GPS_TransferEnd = 0, GPS_HalfTransferEnd = 0;





/********************************************************************************************************
**     ��������:            bit        isLeapYear(uint8_t    iYear)
**    ��������:            �ж�����(�������2000�Ժ�����)
**    ��ڲ�����            iYear    ��λ����
**    ���ڲ���:            uint8_t        1:Ϊ����    0:Ϊƽ��
********************************************************************************************************/
static uint8_t isLeapYear(uint8_t iYear)
{
    uint16_t Year;
    Year = 2000 + iYear;
    if ((Year & 3) == 0)
    {
        return ((Year % 400 == 0) || (Year % 100 != 0));
    }
    return 0;
}

/********************************************************************************************************
**     ��������:            void    gmtConvert(uint8_t *DT,uint8_t GMT,uint8_t AREA)
**    ��������:            ��������ʱ�任�������ʱ��ʱ��
**    ��ڲ�����            *DT:    ��ʾ����ʱ������� ��ʽ YY,MM,DD,HH,MM,SS
**                        GMT:    ʱ����
**                        AREA:    1(+)���� W0(-)����
********************************************************************************************************/
void gmtConvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT, uint8_t AREA)
{
    uint32_t YY, MM, DD, hh, mm, ss;        //������ʱ�����ݴ����

    if (GMT == 0) return;                //�������0ʱ��ֱ�ӷ���
    if (GMT > 12) return;                //ʱ�����Ϊ12 �����򷵻�

    YY = SourceTime->year;                //��ȡ��
    MM = SourceTime->mon;                 //��ȡ��
    DD = SourceTime->day;                 //��ȡ��
    hh = SourceTime->hour;                //��ȡʱ
    mm = SourceTime->min;                 //��ȡ��
    ss = SourceTime->sec;                 //��ȡ��

    if (AREA)                        //��(+)ʱ������
    {
        if (hh + GMT < 24) hh += GMT;//������������ʱ�䴦��ͬһ�������Сʱ����
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ���
        {
            hh = hh + GMT - 24;        //�ȵó�ʱ��
            if (MM == 1 || MM == 3 || MM == 5 || MM == 7 || MM == 8 || MM == 10)    //���·�(12�µ�������)
            {
                if (DD < 31) DD++;
                else
                {
                    DD = 1;
                    MM++;
                }
            }
            else if (MM == 4 || MM == 6 || MM == 9 || MM == 11)                //С�·�2�µ�������)
            {
                if (DD < 30) DD++;
                else
                {
                    DD = 1;
                    MM++;
                }
            }
            else if (MM == 2)    //����2�·�
            {
                if ((DD == 29) || (DD == 28 && isLeapYear(YY) == 0))        //��������������2��29�� ���߲�����������2��28��
                {
                    DD = 1;
                    MM++;
                }
                else DD++;
            }
            else if (MM == 12)    //����12�·�
            {
                if (DD < 31) DD++;
                else        //�������һ��
                {
                    DD = 1;
                    MM = 1;
                    YY++;
                }
            }
        }
    }
    else
    {
        if (hh >= GMT) hh -= GMT;    //������������ʱ�䴦��ͬһ�������Сʱ����
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ���
        {
            hh = hh + 24 - GMT;        //�ȵó�ʱ��
            if (MM == 2 || MM == 4 || MM == 6 || MM == 8 || MM == 9 || MM == 11)    //�����Ǵ��·�(1�µ�������)
            {
                if (DD > 1) DD--;
                else
                {
                    DD = 31;
                    MM--;
                }
            }
            else if (MM == 5 || MM == 7 || MM == 10 || MM == 12)                //������С�·�2�µ�������)
            {
                if (DD > 1) DD--;
                else
                {
                    DD = 30;
                    MM--;
                }
            }
            else if (MM == 3)    //�����ϸ�����2�·�
            {
                if ((DD == 1) && isLeapYear(YY) == 0)                    //��������
                {
                    DD = 28;
                    MM--;
                }
                else DD--;
            }
            else if (MM == 1)    //����1�·�
            {
                if (DD > 1) DD--;
                else        //�����һ��
                {
                    DD = 31;
                    MM = 12;
                    YY--;
                }
            }
        }
    }

    ConvertTime->year = YY;                //������
    ConvertTime->mon = MM;                //������
    ConvertTime->day = DD;                //������
    ConvertTime->hour = hh;                //����ʱ
    ConvertTime->min = mm;                //���·�
    ConvertTime->sec = ss;                //������
}






/*********************************************************end of file**************************************************/
