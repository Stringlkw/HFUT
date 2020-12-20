/***************************************************************
 * 模块名称 : OLED驱动模块
 * 文件名	  : BSPOLED.C
 * 说明  	  : 驱动OLED0.96寸屏幕
 * 版本号	  : v1.0
 * 修改记录 :
 * 	版本号	        日期	                  作者
 *  v1.0     2020-7-10      李康伟
 ***************************************************************/

/***************************************************************
 模块移植性说明
 ****************************************************************/
//TODO
/***************************************************************
 INCLUDES
 ****************************************************************/


#include "bsp_oled.h"
#include "bsp_delay.h"
#include "codetab.h"
#include "stm32f1xx_hal.h"



/***************************************************************
OLED参数定义
 ****************************************************************/
#define XLevelL			0x00
#define XLevelH			0x10
#define XLevel	    ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row			64
#define	Brightness	0xCF
#define X_WIDTH 		128
#define Y_WIDTH 		64


/***************************************************************
 DS18B20硬件接口定义
 ****************************************************************/
#define OLEDn			4				//指示灯数量
//DC
#define DC_PIN					GPIO_PIN_4		//PC4
#define DC_PORT			GPIOC
#define DC_CLK_ENABLE()	__HAL_RCC_GPIOC_CLK_ENABLE()
//CS
#define CS_PIN					GPIO_PIN_8		//PG8
#define CS_PORT			GPIOG
#define CS_CLK_ENABLE()	__HAL_RCC_GPIOG_CLK_ENABLE()
//SDA
#define SDA_PIN					GPIO_PIN_10		//PB10
#define SDA_PORT			GPIOB
#define SDA_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()
//SCL
#define SCL_PIN					GPIO_PIN_11		//PB11
#define SCL_PORT			GPIOB
#define SCL_CLK_ENABLE()	__HAL_RCC_GPIOB_CLK_ENABLE()

//端口数组，数组中存放的都是OLED控制引脚所属端口
GPIO_TypeDef *OLED_PORT[OLEDn] =
{
DC_PORT,
CS_PORT,
SDA_PORT,
SCL_PORT };
//引脚数组，数组中存放的都是OLED控制引脚
const uint16_t OLED_PIN[OLEDn] =
{
DC_PIN,
CS_PIN,
SDA_PIN,
SCL_PIN, };
//端口时钟使能宏定义
#define OLEDx_CLK_ENABLE(__INDEX__) do{\
					    if((__INDEX__) == 0)			\
						    DC_CLK_ENABLE();		\
					    else if((__INDEX__) == 1)		\
						    CS_CLK_ENABLE();		\
					    else if((__INDEX__) == 2)		\
						    SDA_CLK_ENABLE();		\
					    else if((__INDEX__) == 3)		\
						    SCL_CLK_ENABLE();		\
}while(0)

/***************************************************************
 DS18B20函数宏定义
 ****************************************************************/
#define OLED_DC_Reset() HAL_GPIO_WritePin(DC_PORT,DC_PIN, GPIO_PIN_RESET)
#define OLED_DC_Set() HAL_GPIO_WritePin(DC_PORT,DC_PIN, GPIO_PIN_SET)     //接OLED模块D/C

#define OLED_CS_Reset() HAL_GPIO_WritePin(CS_PORT,CS_PIN, GPIO_PIN_RESET)
#define OLED_CS_Set() HAL_GPIO_WritePin(CS_PORT,CS_PIN, GPIO_PIN_SET)    //接OLED模块CS

#define OLED_SDA_Reset() HAL_GPIO_WritePin(SDA_PORT,SDA_PIN, GPIO_PIN_RESET)
#define OLED_SDA_Set() HAL_GPIO_WritePin(SDA_PORT,SDA_PIN, GPIO_PIN_SET)    //接OLED模块MOSI

#define OLED_SCL_Reset() HAL_GPIO_WritePin(SCL_PORT,SCL_PIN, GPIO_PIN_RESET)
#define OLED_SCL_Set() HAL_GPIO_WritePin(SCL_PORT,SCL_PIN, GPIO_PIN_SET)    //接OLED模块的CLK

/***************************************************************
 本模块提供给内部调用的函数
 ***************************************************************/
void OLED_Init(OLED_INDEX Oled); //初始化
void OLED_WrDat(unsigned char dat); //写数据
void OLED_WrCmd(unsigned char cmd); //写命令
void OLED_SetPos(unsigned char x, unsigned char y); //设置起始点坐标
void OLED_Fill(unsigned char bmp_dat); //全屏填充




void OLED_WrDat(unsigned char dat) //写数据
{
	unsigned char i;
	OLED_DC_Set();
	for (i = 0; i < 8; i++)
	{
		if ((dat << i) & 0x80)
		{
			OLED_SDA_Set();
		}
		else
			OLED_SDA_Reset();
		OLED_SCL_Reset();
		OLED_SCL_Set();
	}
}

void OLED_WrCmd(unsigned char cmd) //写命令
{
	unsigned char i;
	OLED_DC_Reset();
	for (i = 0; i < 8; i++) //发送一个八位数据
	{
		if ((cmd << i) & 0x80)
		{
			OLED_SDA_Set();
		}
		else
		{
			OLED_SDA_Reset();
		}
		OLED_SCL_Reset();
		OLED_SCL_Set();
	}
}

void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{
	OLED_WrCmd(0xb0 + y);
	OLED_WrCmd(((x & 0xf0) >> 4) | 0x10);
	OLED_WrCmd((x & 0x0f) | 0x01);
}

void OLED_Fill(unsigned char bmp_dat) //全屏填充
{
	unsigned char y, x;
	for (y = 0; y < 8; y++)
	{
		OLED_WrCmd(0xb0 + y);
		OLED_WrCmd(0x01);
		OLED_WrCmd(0x10);
		for (x = 0; x < X_WIDTH; x++)
		{
			OLED_WrDat(bmp_dat);
		}
	}
}



void OLED_Init(OLED_INDEX Oled)
{

	OLEDx_CLK_ENABLE(Oled);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = OLED_PIN[Oled];
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(OLED_PORT[Oled], &GPIO_InitStruct);
	HAL_GPIO_WritePin(OLED_PORT[Oled], OLED_PIN[Oled], GPIO_PIN_SET);
}

void BSP_OLED_Config()
{
	OLED_Init(DC);
	OLED_Init(CS);
	OLED_Init(SDA);
	OLED_Init(SCL);

	HAL_Delay(500);
	OLED_CS_Reset();

	OLED_WrCmd(0xae);
	OLED_WrCmd(0xae);	//--turn off oled panel
	OLED_WrCmd(0x00);	//---set low column address
	OLED_WrCmd(0x10);	//---set high column address
	OLED_WrCmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WrCmd(0x81);	//--set contrast control register
	OLED_WrCmd(0xcf); // Set SEG Output Current Brightness
	OLED_WrCmd(0xa1); //--Set SEG/Column Mapping     0xa0,0xa1
	OLED_WrCmd(0xc8); //Set COM/Row Scan Direction   0xc0,0xc8
	OLED_WrCmd(0xa6); //--set normal display
	OLED_WrCmd(0xa8); //--set multiplex ratio(1 to 64)
	OLED_WrCmd(0x3f); //--1/64 duty
	OLED_WrCmd(0xd3); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WrCmd(0x00); //-not offset
	OLED_WrCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	OLED_WrCmd(0x80); //--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WrCmd(0xd9); //--set pre-charge period
	OLED_WrCmd(0xf1); //Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WrCmd(0xda); //--set com pins hardware configuration
	OLED_WrCmd(0x12);
	OLED_WrCmd(0xdb); //--set vcomh
	OLED_WrCmd(0x40); //Set VCOM Deselect Level
	OLED_WrCmd(0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WrCmd(0x02); //
	OLED_WrCmd(0x8d); //--set Charge Pump enable/disable
	OLED_WrCmd(0x14); //--set(0x10) disable
	OLED_WrCmd(0xa4); // Disable Entire Display On (0xa4/0xa5)
	OLED_WrCmd(0xa6); // Disable Inverse Display On (0xa6/a7)
	OLED_WrCmd(0xaf); //--turn on oled panel
	OLED_Fill(0x00);
	OLED_SetPos(0, 0);
}

void BSP_OLED_CLS() //清屏
{
	OLED_Fill(0x00);
}

void BSP_OLED_Small_Str(uint8_t x, uint8_t y, uint8_t ch[])
{
	uint8_t c = 0, i = 0, j = 0;
	while (ch[j] != '\0')
	{
		c = ch[j] - 32;
		if (x > 126)
		{
			x = 0;
			y++;
		}
		OLED_SetPos(x, y);
		for (i = 0; i < 6; i++)
		{
			OLED_WrDat(F6x8[c][i]);
		}
		x += 6;
		j++;
	}
}

void BSP_OLED_Big_Str(uint8_t x, uint8_t y, uint8_t ch[])
{
	uint8_t c = 0, i = 0, j = 0;
	while (ch[j] != '\0')
	{
		c = ch[j] - 32;
		if (x > 120)
		{
			x = 0;
			y++;
		}
		OLED_SetPos(x, y);
		for (i = 0; i < 8; i++)
		{
			OLED_WrDat(F8X16[c * 16 + i]);
		}
		OLED_SetPos(x, y + 1);
		for (i = 0; i < 8; i++)
		{
			OLED_WrDat(F8X16[c * 16 + i + 8]);
		}
		x += 8;
		j++;
	}
}

void BSP_OLED_Chinese(uint8_t x, uint8_t y, uint8_t N)
{
	uint8_t wm = 0;
	uint32_t adder = 32 * N;
	OLED_SetPos(x, y);
	for (wm = 0; wm < 16; wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x, y + 1);
	for (wm = 0; wm < 16; wm++)
	{
		OLED_WrDat(F16x16[adder]);
		adder += 1;
	}
}

void BSP_OLED_BMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[])
{
	uint32_t j = 0;
	uint8_t x, y;

	if (y1 % 8 == 0)
	{
		y = y1 / 8;
	}
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		OLED_SetPos(x0, y);
		for (x = x0; x < x1; x++)
		{
			OLED_WrDat(BMP[j++]);
		}
	}
}
