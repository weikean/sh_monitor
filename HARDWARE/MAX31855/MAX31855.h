/*
**************************************************************************************************
* �ļ�����MAX31855.h
* ��  ������˲����ȵ�ż���������ת����
* �汾�ţ�V0.0
* ��  ע��1. MAX31855������˲�������K��J��N��T��E���ȵ�ż�ź�ת��������������
          2. ���14λ���������ݣ�ͨ��SPI���ݽӿڡ���ֻ����ʽ�����
					3. ת�������¶ȷֱ���Ϊ0.25�棬����¶ȶ���Ϊ+1800�棬����¶ȶ���Ϊ-270��
					4. ����K���ȵ�ż���¶ȷ�ΧΪ-200����+700�棬���֡�2�澫�ȡ�
          5. ����ȵ�ż��GND��VCC��· 
          6. ����ȵ�ż��·
					7. VOUT = (41.276uV/��)X(TR - TAMB)   ʽ�У�VOUTΪ�ȵ�ż�����ѹ(��V)��TRΪԶ���ȵ�ż����(��)��TAMBΪ�����¶�(��)��
					
        ����    |   ����    |  ����
         1      |    T-     | �ȵ�ż���룬����������GND
         3      |    T+     | �ȵ�ż���룬
         4      |    VCC    | ��Դ��ѹ��
         5      |    SCK    | ����ʱ�����롣
         6      |    CS     | �͵�ƽ��ЧƬѡ��CS��Ϊ�͵�ƽʱ��ʹ�ܴ��ڡ�
         7      |    SO     | ������������� 
         8      |    DNC    | �����ӡ�
***************************************************************************************************
*/

#ifndef _MAX31855_H
#define _MAX31855_H

#include "stm32f10x.h"
#include "stdbool.h"

extern float tmprtr,ntrnl_tmprtr;
extern uint8_t SCV_BIT;//��·��VCC��־λ  0û��·   1 ��·
extern uint8_t SCG_BIT;//��·��GND��־λ  0û��·   1 ��·
extern uint8_t OC_BIT;//�ȵ�ż��·��־λ 0û��·   1 ��·
extern uint8_t FAULT;  //	������һ��ʧ�ܷ�Ӧ�����λ  0ͨ��  1ʧ��

/*******************************************MAX31855�ӿڶ���****************************/

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

/******************************************MAX31855�ӿں궨��********************************/

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

/******************************************MAX31855�ӿں�������*****************************/
void MAX31855_RCCConfig(void);
void MAX31855_GPIOConfig(void);
void MAX31855_Init(void);
uint32_t max31855_read(uint32_t chip);
// float MAX31855_ReadData_1(void);
// float MAX31855_ReadData_2(void);
// float MAX31855_ReadData_3(void);

#endif



















