/*
**************************************************************************************************
* 文件名：MAX31855.h
* 描  述：冷端补偿热电偶至数字输出转换器
* 版本号：V0.0
* 备  注：1. MAX31855具有冷端补偿，将K、J、N、T或E型热电偶信号转换成数字量器件
          2. 输出14位带符号数据，通过SPI兼容接口、以只读格式输出。
					3. 转换器的温度分辨率为0.25℃，最高温度读数为+1800℃，最低温度读数为-270℃
					4. 对于K型热电偶，温度范围为-200℃至+700℃，保持±2℃精度。
          5. 检测热电偶对GND或VCC短路 
          6. 检测热电偶开路
					7. VOUT = (41.276uV/℃)X(TR - TAMB)   式中，VOUT为热电偶输出电压(μV)，TR为远端热电偶结温(℃)，TAMB为器件温度(℃)。
					
        引脚    |   名称    |  功能
         1      |    T-     | 热电偶输入，请勿连接至GND
         3      |    T+     | 热电偶输入，
         4      |    VCC    | 电源电压。
         5      |    SCK    | 串行时钟输入。
         6      |    CS     | 低电平有效片选。CS置为低电平时，使能串口。
         7      |    SO     | 串行数据输出。 
         8      |    DNC    | 不连接。
***************************************************************************************************
*/

#ifndef _MAX31855_H
#define _MAX31855_H

#include "stm32f10x.h"
#include "stdbool.h"

extern float tmprtr,ntrnl_tmprtr;
extern uint8_t SCV_BIT;//短路到VCC标志位  0没短路   1 短路
extern uint8_t SCG_BIT;//短路到GND标志位  0没短路   1 短路
extern uint8_t OC_BIT;//热电偶开路标志位 0没开路   1 开路
extern uint8_t FAULT;  //	上面任一个失败反应到这个位  0通过  1失败

/*******************************************MAX31855接口定义****************************/

#define MAX31855_Pin_CS1   GPIO_Pin_4
#define MAX31855_Port_CS1  GPIOA
#define MAX31855_RCC_CS1   RCC_APB2Periph_GPIOA

#define MAX31855_Pin_CS2  GPIO_Pin_7
#define MAX31855_Port_CS2 GPIOA
#define MAX31855_RCC_CS2  RCC_APB2Periph_GPIOA

#define MAX31855_Pin_CS3  GPIO_Pin_8
#define MAX31855_Port_CS3 GPIOC
#define MAX31855_RCC_CS3  RCC_APB2Periph_GPIOC

#define MAX31855_Pin_SCK   GPIO_Pin_5
#define MAX31855_Port_SCK  GPIOA
#define MAX31855_RCC_SCK   RCC_APB2Periph_GPIOA

#define MAX31855_Pin_SO    GPIO_Pin_6     
#define MAX31855_Port_SO   GPIOA
#define MAX31855_RCC_SO    RCC_APB2Periph_GPIOA

/******************************************MAX31855接口宏定义********************************/

#define MAX31855_CS1_HIGH()	GPIO_SetBits(MAX31855_Port_CS1,MAX31855_Pin_CS1)
#define MAX31855_CS1_LOW()	GPIO_ResetBits(MAX31855_Port_CS1,MAX31855_Pin_CS1)
#define MAX31855_CS2_HIGH() GPIO_SetBits(MAX31855_Port_CS2,MAX31855_Pin_CS2)
#define MAX31855_CS2_LOW()	GPIO_ResetBits(MAX31855_Port_CS2,MAX31855_Pin_CS2)
#define MAX31855_CS3_HIGH() GPIO_SetBits(MAX31855_Port_CS3,MAX31855_Pin_CS3)
#define MAX31855_CS3_LOW()	GPIO_ResetBits(MAX31855_Port_CS3,MAX31855_Pin_CS3)
#define MAX31855_SCK_HIGH() GPIO_SetBits(MAX31855_Port_SCK,MAX31855_Pin_SCK)
#define MAX31855_SCK_LOW()  GPIO_ResetBits(MAX31855_Port_SCK,MAX31855_Pin_SCK);
#define MAX31855_READ_SO()  GPIO_ReadInputDataBit(MAX31855_Port_SO,MAX31855_Pin_SO)
// #define MAX31855_CS(x)   x ? GPIO_SetBits(MAX31855_Port_CS,MAX31855_Pin_CS)  : GPIO_ResetBits(MAX31855_Port_CS,MAX31855_Pin_CS);
// #define MAX31855_CS_2(x)   x ? GPIO_SetBits(MAX31855_Port_CS_2,MAX31855_Pin_CS_2)  : GPIO_ResetBits(MAX31855_Port_CS_2,MAX31855_Pin_CS_2);
// #define MAX31855_CS_3(x)   x ? GPIO_SetBits(MAX31855_Port_CS_3,MAX31855_Pin_CS_3)  : GPIO_ResetBits(MAX31855_Port_CS_3,MAX31855_Pin_CS_3);
// #define MAX31855_SCK(x)  x ? GPIO_SetBits(MAX31855_Port_SCK,MAX31855_Pin_SCK): GPIO_ResetBits(MAX31855_Port_SCK,MAX31855_Pin_SCK);

static inline void max31855_cs(uint32_t chip)
{
	switch(chip)
	{
		case 0:
		MAX31855_CS1_HIGH();
		MAX31855_CS2_HIGH();
		MAX31855_CS3_HIGH();
		break;
		
		case 1:
		MAX31855_CS1_LOW();
		break;
		
		case 2:
		MAX31855_CS2_LOW();
		break;
		
		case 3:
		MAX31855_CS3_LOW();
		break;
	}

}

static inline float max31855_temp_ext(uint32_t data)
{
	int16_t tmp;
	tmp = (int16_t)((data >>18)&0x3fff);
	tmp <<= 2;
	return (tmp*0.25/4);
}

static inline float max31855_temp_intl(uint32_t data)
{
	int16_t tmp;
	tmp = (int16_t)((data >>4)&0xfff);
	tmp <<= 4;
	return (tmp*0.0625/16);
}

static inline bool max31855_is_oc(uint32_t data)
{
	return data & (1 << 0);
}

static inline bool max31855_is_scg(uint32_t data)
{
	return data & (1 << 1);
}

static inline bool max31855_is_scv(uint32_t data)
{
	return data & (1 << 2);
}

static inline bool max31855_is_fault(uint32_t data)
{
	return data & (1 << 16);
}

/******************************************MAX31855接口函数定义*****************************/
void MAX31855_RCCConfig(void);
void MAX31855_GPIOConfig(void);
void MAX31855_Init(void);
uint32_t max31855_read(uint32_t chip);
// float MAX31855_ReadData_1(void);
// float MAX31855_ReadData_2(void);
// float MAX31855_ReadData_3(void);

#endif



















