#ifndef __BSP_GSM_H__
#define __BSP_GSM_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "bsp_usartx.h"

/* 类型定义 ------------------------------------------------------------------*/
typedef enum {
    GSM_TRUE,
    GSM_FALSE,
} GSM_RES;

/* 宏定义 --------------------------------------------------------------------*/
#define BSP_GSM_Config()                        GSM_USARTx_Init()
#define GSM_TX_Printf(fmt, ...)     GSM_USART_Printf(GSM_USARTx,fmt,##__VA_ARGS__)     //printf格式发送命令（纯发送，不进行任何接收数据处理）

#define GSM_CLEAN_Rx()              cleanRebuff()
#define GSM_Tx(cmd)                    GSM_TX_Printf("%s",cmd)
#define GSM_Rx(len)                 ((char *)getRebuff(&(len)))

/*************************** 电话 功能 ***************************/
#define GSM_HANGON()                GSM_Tx("ATA\r");
#define GSM_HANGOFF()                GSM_Tx("ATH\r");    //挂断电话

/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
//                  指令             正常返回
//本机号码          AT+CNUM\r         +CNUM: "","13265002063",129,7,4            //很多SIM卡默认都是没设置本机号码的，解决方法如下 http://www.multisilicon.com/blog/a21234642.html
//SIM营运商         AT+COPS?\r        +COPS: 0,0,"CHN-UNICOM"   OK
//SIM卡状态         AT+CPIN?\r        +CPIN: READY   OK
//SIM卡信号强度     AT+CSQ\r          +CSQ: 8,0   OK
uint8_t BSP_GSM_Cmd(char *cmd, char *reply, uint32_t waittime);
uint8_t BSP_GSM_CmdCheck(char *reply);
uint8_t BSP_GSM_Init(void); //初始化并检测模块
uint8_t BSP_GSM_IsInsertCard();

/*************************** 电话 功能 ***************************/
uint8_t BSP_GSM_Cnum(char *num); //获取本机号码
void BSP_GSM_Call(char *num); //发起拨打电话（不管接不接通）
uint8_t BSP_GSM_IsRing(char *num); //查询是否来电，并保存来电号码

/***************************  短信功能  ****************************/
void BSP_GSM_Sms(char *num, char *smstext); //发送短信（支持中英文,中文为GBK码）
char *BSP_GSM_Waitask(uint8_t waitask_hook(void)); //等待有数据应答，返回接收缓冲区地址
uint8_t BSP_GSM_IsReceiveMS(void);
uint8_t BSP_GSM_ReadMessage(uint8_t messadd, char *num, char *str);

#endif
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
