/**
  ******************************************************************************
  * @file    bsp_gsm_gprs.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSM模块驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
//#include "./usart/bsp_usart.h"
#include "bsp_gsm.h"
//#include "ff.h"

static uint8_t MaxMessAdd = 50;


//0表示成功，1表示失败

uint8_t gsm_cmd(char *cmd, char *reply, uint32_t waittime)
{
    GSM_DEBUG_FUNC();

    GSM_CLEAN_RX();                 //清空了接收缓冲区数据

    GSM_TX(cmd);                    //发送命令

    GSM_DEBUG("Send cmd:%s", cmd);

    if (reply == 0)                      //不需要接收数据
    {
        return GSM_TRUE;
    }

    GSM_DELAY(waittime);                 //延时


    return gsm_cmd_check(reply);    //对接收数据进行处理
}


//0表示成功，1表示失败
uint8_t gsm_cmd_check(char *reply)
{
    uint8_t len;
    uint8_t n;
    uint8_t off;
    char *redata;

    GSM_DEBUG_FUNC();

    redata = GSM_RX(len);   //接收数据

    *(redata + len) = '\0';
    GSM_DEBUG("Reply:%s", redata);

//		GSM_DEBUG_ARRAY((uint8_t *)redata,len);

    n = 0;
    off = 0;
    while ((n + off) < len)
    {
        if (reply[n] == 0)                 //数据为空或者比较完毕
        {
            return GSM_TRUE;
        }

        if (redata[n + off] == reply[n])
        {
            n++;                //移动到下一个接收数据
        }
        else
        {
            off++;              //进行下一轮匹配
            n = 0;                //重来
        }
        //n++;
    }

    if (reply[n] == 0)   //刚好匹配完毕
    {
        return GSM_TRUE;
    }

    return GSM_FALSE;       //跳出循环表示比较完毕后都没有相同的数据，因此跳出
}

char *BSP_GSM_Waitask(uint8_t waitask_hook(void))      //等待有数据应答
{
    uint8_t len = 0;
    char *redata;

    GSM_DEBUG_FUNC();

    do
    {
        redata = GSM_RX(len);   //接收数据


        if (waitask_hook != 0)
        {
            if (waitask_hook() == GSM_TRUE)           //返回 GSM_TRUE 表示检测到事件，需要退出
            {
                redata = 0;
                return redata;
            }
        }
    } while (len == 0);                 //接收数据为0时一直等待

    GSM_DEBUG_ARRAY((uint8_t *) redata, len);


    GSM_DELAY(20);              //延时，确保能接收到全部数据（115200波特率下，每ms能接收11.52个字节）
    return redata;
}


//本机号码
//0表示成功，1表示失败
uint8_t gsm_cnum(char *num)
{
    char *redata;
    uint8_t len;

    GSM_DEBUG_FUNC();

    if (gsm_cmd("AT+CNUM\r", "OK", 100) != GSM_TRUE)
    {
        return GSM_FALSE;
    }

    redata = GSM_RX(len);   //接收数据

    GSM_DEBUG_ARRAY((uint8_t *) redata, len);

    if (len == 0)
    {
        return GSM_FALSE;
    }

    //第一个逗号后面的数据为:"本机号码"
    while (*redata != ',')
    {
        len--;
        if (len == 0)
        {
            return GSM_FALSE;
        }
        redata++;
    }
    redata += 2;

    while (*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //字符串结尾需要清0
    return GSM_TRUE;
}


//初始化并检测模块
//0表示成功，1表示失败
uint8_t gsm_init(void)
{
    char *redata;
    uint8_t len;

    GSM_DEBUG_FUNC();

    GSM_CLEAN_RX();                 //清空了接收缓冲区数据
    GSM_USART_Config();                    //初始化串口


    if (gsm_cmd("AT+CGMM\r", "OK", 100) != GSM_TRUE)
    {
        return GSM_FALSE;
    }

    redata = GSM_RX(len);   //接收数据

    if (len == 0)
    {
        return GSM_FALSE;
    }
    //本程序兼容GSM900A、GSM800A、GSM800C
    if (strstr(redata, "SIMCOM_GSM900A") != 0)
    {
        return GSM_TRUE;
    }
    else if (strstr(redata, "SIMCOM_SIM800") != 0)
    {
        return GSM_TRUE;
    }
    else
        return GSM_FALSE;

}




//检测是否有卡
//0表示成功，1表示失败
uint8_t IsInsertCard(void)
{
    GSM_DEBUG_FUNC();

    GSM_CLEAN_RX();
    return gsm_cmd("AT+CNUM\r", "OK", 200);

}


/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
/**
 * @brief  IsASSIC 判断是否纯ASSIC编码
 * @param  str: 字符串指针
 * @retval 纯ASSIC编码返回TRUE，非纯ASSIC编码返回FALSE
 */
uint8_t IsASSIC(char *str)
{
    GSM_DEBUG_FUNC();

    while (*str)
    {
        if (*str > 0x7F)
        {
            return GSM_FALSE;
        }
        str++;
    }

    return GSM_TRUE;
}


/**
 * @brief  BSP_GSM_Gbk2ucs2 把GBK编码转换成UCS2编码
 * @param  ucs2: ucs2字符串指针，gbk：GBK字符串指针
 * @retval 无
 */
void BSP_GSM_Gbk2ucs2(char *ucs2, char *gbk)
{
    WCHAR tmp;

    GSM_DEBUG_FUNC();


    while (*gbk)
    {
        if ((*gbk & 0xFF) < 0x7F)      //英文
        {

            *ucs2++ = 0;
            *ucs2++ = *gbk++;
        }
        else                        //中文
        {
            tmp = GSM_SWAP16(*(WCHAR *) gbk);

            *(WCHAR *) ucs2 = GSM_SWAP16(ff_convert(tmp, 1));
            gbk += 2;
            ucs2 += 2;
        }
    }
}

/**
 * @brief  gsm_char2hex 把字符转换成16进制字符
 * @param  hex: 16进制字符存储的位置指针，ch：字符
 * @retval 无
 */
void gsm_char2hex(char *hex, char ch)
{
    const char numhex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    GSM_DEBUG_FUNC();

    *hex++ = numhex[(ch & 0xF0) >> 4];
    *hex = numhex[ch & 0x0F];
}


/**
 * @brief  gsm_gbk2ucs2hex GBK编码转ucs2编码再转成16进制字符
 * @param  ucs2hex: 16进制数存储的位置指针，gbk：字符
 * @retval 无
 */
void gsm_gbk2ucs2hex(char *ucs2hex, char *gbk)
{
    WCHAR tmp;

    GSM_DEBUG_FUNC();

    while (*gbk)
    {
        if ((*gbk & 0xFF) < 0x7F)      //英文
        {

            *ucs2hex++ = '0';
            *ucs2hex++ = '0';
            gsm_char2hex(ucs2hex, *gbk);
            ucs2hex += 2;
            gbk++;
        }
        else                        //中文
        {
            tmp = GSM_SWAP16(*(WCHAR *) gbk);
            tmp = ff_convert(tmp, 1);
            gsm_char2hex(ucs2hex, (char) (tmp >> 8));
            ucs2hex += 2;
            gsm_char2hex(ucs2hex, (char) tmp);
            ucs2hex += 2;
            gbk += 2;
        }
    }
    *ucs2hex = 0;
}



//发送短信（支持中英文,中文为GBK码）
/**
 * @brief  BSP_GSM_Sms 发送短信（支持中英文,中文为GBK码）
 * @param  phoneNum: 电话号码，smsText：短信内容
 * @retval 无
 */
uint8_t BSP_GSM_Sms(char *phoneNum, char *smsText)
{
    char converBuff[160];
    char cmdBuff[80];
    char end = 0x1A;
    uint8_t testSend = 0;

    GSM_DEBUG_FUNC();

    GSM_CLEAN_RX();                 //清空了接收缓冲区数据

    if (IsASSIC(smsText) == GSM_TRUE)
    {
        //英文
        //"GSM"字符集
        if (gsm_cmd("AT+CSCS=\"GSM\"\r", "OK", 100) != GSM_TRUE) goto sms_failure;

        //文本模式
        if (gsm_cmd("AT+CMGF=1\r", "OK", 100) != GSM_TRUE) goto sms_failure;

        //生成电话号码命令
        sprintf(cmdBuff, "AT+CMGS=\"%s\"\r", phoneNum);
        //复制短信内容
        strcpy(converBuff, smsText);

    }
    else
    {
        //中文
        //"UCS2"字符集
        if (gsm_cmd("AT+CSCS=\"UCS2\"\r", "OK", 100) != GSM_TRUE) goto sms_failure;
        //文本模式
        if (gsm_cmd("AT+CMGF=1\r", "OK", 100) != GSM_TRUE) goto sms_failure;

        if (gsm_cmd("AT+CSMP=17,167,0,8\r", "OK", 100) != GSM_TRUE) goto sms_failure;

        //生成电话号码命令，UCS2的电话号码(需要转成 ucs2码)
        gsm_gbk2ucs2hex(converBuff, phoneNum);//转换为UCS2
        sprintf(cmdBuff, "AT+CMGS=\"%s\"\r", converBuff);

        //转换短信内容
        gsm_gbk2ucs2hex(converBuff, smsText);
    }

    //发送电话号码
    if (gsm_cmd(cmdBuff, ">", 200) == 0)
    {
        

        GSM_CLEAN_RX();
        gsm_cmd(&end, 0, 100);        //0x1A结束符

        //检测是否发送完成
        while (gsm_cmd_check("OK") != GSM_TRUE)
        {
            if (++testSend > 100)//最长等待10秒
            {
                goto sms_failure;
            }
            GSM_DELAY(100);
        }
        return GSM_TRUE;
    }
    else
    {
        sms_failure:
        end = 0x1B;
        gsm_cmd(&end, 0, 0);    //ESC,取消发送
        return GSM_FALSE;
    }
}





/*---------------------------------------------------------------------*/
