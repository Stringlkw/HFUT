#ifndef __BSP_GSM_H
#define __BSP_GSM_H

#include "stm32f1xx_hal.h"
#include "bsp_systick.h"

typedef enum {
    GSM_TRUE,
    GSM_FALSE,

} gsm_res_e;

typedef enum {
    GSM_NULL = 0,
    GSM_CMD_SEND = '\r',
    GSM_DATA_SEND = 0x1A,         //发送数据(ctrl + z)
    GSM_DATA_CANCLE = 0x1B,         //发送数据(Esc)
} gsm_cmd_end_e;

//                  指令             正常返回
//本机号码          AT+CNUM\r         +CNUM: "","13265002063",129,7,4            //很多SIM卡默认都是没设置本机号码的，解决方法如下 http://www.multisilicon.com/blog/a21234642.html
//SIM营运商         AT+COPS?\r        +COPS: 0,0,"CHN-UNICOM"   OK
//SIM卡状态         AT+CPIN?\r        +CPIN: READY   OK
//SIM卡信号强度     AT+CSQ\r          +CSQ: 8,0   OK

extern uint8_t gsm_cmd(char *cmd, char *reply, uint32_t waitTime);

extern uint8_t gsm_cmd_check(char *reply);


#define     GSM_CLEAN_RX()              clean_rebuff()
#define     gsm_ate0()                  gsm_cmd("ATE0\r","OK",100)              //关闭回显
#define     GSM_TX(cmd)                    GSM_USART_printf("%s",cmd)
#define     GSM_IS_RX()                 (USART_GetFlagStatus(GSM_USARTx, USART_FLAG_RXNE) != RESET)
#define     GSM_RX(len)                 ((char *)get_rebuff(&(len)))
#define     GSM_DELAY(time)             Delay_ms(time)                 //延时
#define     GSM_SWAP16(data)                     __REVSH(data)


/***************************  短信功能  ****************************/
uint8_t BSP_GSM_Sms(char *phoneNum, char *smsText);          //发送短信（支持中英文,中文为GBK码）
char *BSP_GSM_Waitask(uint8_t waitask_hook(void));       //等待有数据应答，返回接收缓冲区地址
void BSP_GSM_Gbk2ucs2(char *ucs2, char *gbk);













#endif

