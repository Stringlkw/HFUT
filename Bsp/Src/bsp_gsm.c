
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "usart.h"
#include "bsp_usartx.h"
#include "bsp_gsm.h"


static uint8_t MaxMessAdd = 50;
uint8_t aRxBuffer;

/**
  * 函数功能: 向GSM发送一个命令，并验证返回值是否正确
  * 输入参数: cmd：命令
  *           replay：期待返回值
  *           waittime：等待时间
  * 返 回 值: GSM_TRUE：成功
  *           GSM_FALSE：失败
  * 说    明：无
  */
uint8_t BSP_GSM_Cmd(char *cmd, char *reply, uint32_t waittime)
{
    GSM_CLEAN_Rx();                 //清空了接收缓冲区数据
    GSM_Tx(cmd);                    //发送命令
    if (reply == 0)                      //不需要接收数据
    {
        return GSM_TRUE;
    }
    HAL_Delay(waittime);                 //延时
    return BSP_GSM_CmdCheck(reply);    //对接收数据进行处理
}

/**
  * 函数功能: 验证命令的返回值是否正确
  * 输入参数: replay：期待返回值
  * 返 回 值: GSM_TRUE：成功
  *           GSM_FALSE：失败
  * 说    明：无
  */
uint8_t BSP_GSM_CmdCheck(char *reply)
{
    uint8_t len;
    uint8_t n;
    uint8_t off;
    char *redata;

    redata = GSM_Rx(len); //接收数据
    n = 0;
    off = 0;
    while ((n + off) < len)
    {
        if (reply[n] == 0) //数据为空或者比较完毕
        {
            return GSM_TRUE;
        }
        if (redata[n + off] == reply[n])
        {
            n++; //移动到下一个接收数据
        }
        else
        {
            off++; //进行下一轮匹配
            n = 0; //重来
        }
        //n++;
    }
    if (reply[n] == 0) //刚好匹配完毕
    {
        return GSM_TRUE;
    }
    return GSM_FALSE; //跳出循环表示比较完毕后都没有相同的数据，因此跳出
}

/**
  * 函数功能: 等待GSM有数据应答
  * 输入参数: 无
  * 返 回 值: 应答数据串
  * 说    明：无
  */
char *BSP_GSM_Waitask(uint8_t waitask_hook(void))
{
    uint8_t len = 0;
    char *redata;
    do
    {
        redata = GSM_Rx(len);   //接收数据
        if (waitask_hook != 0)
        {
            if (waitask_hook() == GSM_TRUE) //返回 GSM_TRUE 表示检测到事件，需要退出
            {
                redata = 0;
                return redata;
            }
        }
    } while (len == 0);                 //接收数据为0时一直等待
    HAL_Delay(20);              //延时，确保能接收到全部数据（115200波特率下，每ms能接收11.52个字节）
    return redata;
}

/**
  * 函数功能: 获取本机号码
  * 输入参数: num：存放号码的缓冲区
  * 返 回 值: GSM_TRUE：成功
  *           GSM_FALSE：失败
  * 说    明：无
  */
uint8_t BSP_GSM_Cnum(char *num)
{
    char *redata;
    uint8_t len;

    if (BSP_GSM_Cmd("AT+CNUM\r", "OK", 100) != GSM_TRUE)
    {
        return GSM_FALSE;
    }
    redata = GSM_Rx(len);   //接收数据
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

/**
  * 函数功能: 初始化并检测模块
  * 输入参数: 无
  * 返 回 值: GSM_TRUE：成功
  *           GSM_FALSE：失败
  * 说    明：无
  */
uint8_t BSP_GSM_Init(void)
{
    char *redata;
    uint8_t len;

    GSM_CLEAN_Rx(); //清空了接收缓冲区数据
    GSM_TX_Printf("ATQ0\r");
    HAL_Delay(200);
    HAL_UART_Receive_IT(&huart_GSM, &aRxBuffer, 1);
    if (BSP_GSM_Cmd("ATE0\r", "OK", 200) != GSM_TRUE)
    {
        return GSM_FALSE;
    }
    GSM_CLEAN_Rx(); //清空了接收缓冲区数据
    if (BSP_GSM_Cmd("AT+CGMM\r", "OK", 200) != GSM_TRUE)
    {
        return GSM_FALSE;
    }
    redata = GSM_Rx(len);   //接收数据
    if (len == 0)
    {
        return GSM_FALSE;
    }
    if (strstr(redata, "SIMCOM_SIM") != 0)
    {
        return GSM_TRUE;
    }
    else
        return GSM_FALSE;
}

/**
  * 函数功能: 检测手机卡是否插入
  * 输入参数: 无
  * 返 回 值: GSM_TRUE：成功
  *          GSM_FALSE：失败
  * 说    明：无
  */
uint8_t BSP_GSM_IsInsertCard()
{
    GSM_CLEAN_Rx();
    return BSP_GSM_Cmd("AT+CNUM\r","OK",200);

}

/*-------------------------   电话功能  ---------------------------------*/
/**
  * 函数功能: 发起拨打电话（不管接不接通）
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
void BSP_GSM_Call(char *num)
{
    GSM_CLEAN_Rx(); //清空了接收缓冲区数据
    GSM_TX_Printf("ATD%s;\r", num);
    //拨打后是不返回数据的
    //不管任何应答，开头都是"\r\n",即真正有效的数据是从第3个字节开始
    //对方挂掉电话（没接通），返回：BUSY
    //对方接听了电话：+COLP: "555",129,"",0,"9648674F98DE"   OK
    //对方接听了电话,然后挂掉：NO CARRIER
    //对方超时没接电话：NO ANSWER
}

/**
  * 函数功能: 有来电电话
  * 输入参数: num：来电号码缓冲区
  * 返 回 值: GSM_TRUE：成功
  *           GSM_FALSE：失败
  * 说    明：无
  */
uint8_t BSP_GSM_IsRing(char *num)
{
    char *redata;
    uint8_t len;

    if (BSP_GSM_CmdCheck("RING"))
    {
        return GSM_FALSE;
    }
    redata = GSM_Rx(len);   //接收数据
    if (len == 0)
    {
        return GSM_FALSE;
    }
    //第一个逗号后面的数据为:”号码“
    while (*redata != ':')
    {
        len--;
        if (len == 0)
        {
            return GSM_FALSE;
        }
        redata++;
    }
    redata += 3;
    while (*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //字符串结尾需要清0
    GSM_CLEAN_Rx();
    return GSM_TRUE;
}

/*-------------------------   短信功能  ---------------------------------*/
/**
  * 函数功能: 判断字符串只是ASCLL，没有中文
  * 输入参数: str：字符串缓冲区
  * 返 回 值: GSM_TRUE：成功
  *           GSM_FALSE：失败
  * 说    明：无
  */
uint8_t IsASSIC(char *str)
{
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
  * 函数功能: 数字字符转16进制字符串
  * 输入参数: hex：16进制字符串缓冲区
  *           ch：数字字符
  * 返 回 值: 无
  * 说    明：无
  */
void GSM_char2hex(char *hex, char ch)
{
    const char numhex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    *hex++ = numhex[(ch & 0xF0) >> 4];
    *hex = numhex[ch & 0x0F];
}


/**
  * 函数功能: 发送短信（支持中英文,中文为GBK码）
  * 输入参数: num：对方手机号码
  *           smstext：短信内容
  * 返 回 值: 无
  * 说    明：无
  */
void BSP_GSM_Sms(char *num, char *smstext)
{
    char ucsbuff[160];
    char end[2] = {0x1A, 0x00};

    GSM_CLEAN_Rx();                 //清空了接收缓冲区数据
    if (IsASSIC(smstext) == GSM_TRUE)
    {
        //英文
        GSM_TX_Printf("AT+CSCS=\"GSM\"\r");     //"GSM"字符集
        HAL_Delay(100);
        GSM_TX_Printf("AT+CMGF=1\r");           //文本模式
        HAL_Delay(100);
        GSM_TX_Printf("AT+CMGS=\"%s\"\r", num);  //电话号码
        HAL_Delay(100);
        GSM_TX_Printf("%s", smstext);            //短信内容
    }
    HAL_Delay(1);
    GSM_TX_Printf("%s", end);
}

/**
  * 函数功能: 查询是否接收到新短信
  * 输入参数: 无
  * 返 回 值: 0:无新短信
  *           非0：新短信地址
  * 说    明：无
  */
uint8_t BSP_GSM_IsReceiveMS(void)
{
    char address[3] = {0};
    uint8_t addressnum = 0;
    char *redata;
    uint8_t len;
/*------------- 查询是否有新短信并提取存储位置 ----------------------------*/
    if (BSP_GSM_CmdCheck("+CMTI:"))
    {
        return 0;
    }
    redata = GSM_Rx(len);   //接收数据
    //printf("CMTI:redata:%s,len=%d\n",redata,len);
    if (len == 0)
    {
        return 0;
    }
    //第一个逗号后面的数据为:”短信存储地址“
    while (*redata != ',')
    {
        len--;
        if (len == 0)
        {
            return 0;
        }
        redata++;
    }
    redata += 1;//去掉；','
    address[0] = *redata++;
    address[1] = *redata++;
    address[2] = *redata++;
    if ((address[2] >= '0') && (address[2] <= '9'))
        addressnum = (address[0] - '0') * 100 + (address[1] - '0') * 10 + (address[2] - '0');
    else
        addressnum = (address[0] - '0') * 10 + (address[1] - '0');
    return addressnum;
}

/**
  * 函数功能: 读取短信内容
  * 输入参数: messadd：短信地址
  *        		num：保存发件人号码(unicode编码格式的字符串)
  *     			str：保存短信内容(unicode编码格式的字符串)
  * 返 回 值: 0：失败
  * 	        1：成功读取到短信，该短信未读（此处是第一次读，读完后会自动被标准为已读）
  *      			2：成功读取到短信，该短信已读
  * 说    明：无
  */
uint8_t BSP_GSM_ReadMessage(uint8_t messadd, char *num, char *str)
{
    char *redata, cmd[20] = {0};
    uint8_t len;
    char result = 0;
    GSM_CLEAN_Rx();                 //清空了接收缓冲区数据
    if (messadd > MaxMessAdd)return 0;

/*------------- 读取短信内容 ----------------------------*/
    sprintf(cmd, "AT+CMGR=%d\r", messadd);
    if (BSP_GSM_Cmd(cmd, "+CMGR:", 500) != GSM_TRUE)
    {
        return 0;
    }
    redata = GSM_Rx(len);   //接收数据
    if (len == 0)
    {
        return 0;
    }
    if (strstr(redata, "UNREAD") == 0)result = 2;
    else result = 1;
    //第一个逗号后面的数据为:”发件人号码“
    while (*redata != ',')
    {
        len--;
        if (len == 0)
        {
            return 0;
        }
        redata++;
    }
    redata += 2;//去掉',"'
    while (*redata != '"')
    {
        *num++ = *redata++;
        len--;
    }
    *num = 0;               //字符串结尾需要清0

    while (*redata != '+')
    {
        len--;
        if (len == 0)
        {
            return 0;
        }
        redata++;
    }
    redata += 6;//去掉'+32"\r'
    while (*redata != '\r')
    {
        *str++ = *redata++;
    }
    *str = 0;               //字符串结尾需要清0
    return result;
}




