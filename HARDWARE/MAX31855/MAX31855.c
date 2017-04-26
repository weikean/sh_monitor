#include "stm32f10x.h"
#include "MAX31855.h"
#include "stdio.h"
uint8_t SCV_BIT=0;//��·��VCC��־λ  0û��·   1 ��·
uint8_t SCG_BIT=0;//��·��GND��־λ  0û��·   1 ��·
uint8_t OC_BIT =0;//�ȵ�ż��·��־λ 0û��·   1 ��·
uint8_t FAULT=0;  //	������һ��ʧ�ܷ�Ӧ�����λ  0ͨ��  1ʧ��
float tmprtr=0,ntrnl_tmprtr=0;
float tmprtr_m=0,ntrnl_tmprtr_m=0;
float tmprtr_b=0,ntrnl_tmprtr_b=0;
/********************************************************************* 
*��    ��: Delay_us(u16 time)
*��    ����ѭ��΢�����ʱ����
*��ڲ�����time����λ΢��
*���ڲ�������
*********************************************************************/
static void delay_us(u16 time)
{
   u16 i = 0;
	 while(time--)
	 {
      i = 10;
		  while(i--);
   }
}
/**
  * @brief  MAX31855оƬRCC����
  *
  */
void MAX31855_RCCConfig(void)
{
  RCC_APB2PeriphClockCmd(MAX31855_RCC_SO,ENABLE);
  RCC_APB2PeriphClockCmd(MAX31855_RCC_CS1,ENABLE);
	RCC_APB2PeriphClockCmd(MAX31855_RCC_CS2,ENABLE);
	RCC_APB2PeriphClockCmd(MAX31855_RCC_CS3,ENABLE);
	RCC_APB2PeriphClockCmd(MAX31855_RCC_SCK,ENABLE);
}

/**
  * @brief  MAX31855оƬGPIO����
  *
  */

void MAX31855_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = MAX31855_Pin_SO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 
	GPIO_Init(MAX31855_Port_SO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MAX31855_Pin_CS1;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	
	GPIO_Init(MAX31855_Port_CS1, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MAX31855_Pin_CS2;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	
	GPIO_Init(MAX31855_Port_CS2, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = MAX31855_Pin_CS3;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;	
	GPIO_Init(MAX31855_Port_CS3, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = MAX31855_Pin_SCK;
	GPIO_Init(MAX31855_Port_SCK, &GPIO_InitStructure);
}
/**
  * @brief  MAX31855��ʼ��
  *
  */
void MAX31855_Init(void)
{
	  MAX31855_RCCConfig();
    MAX31855_GPIOConfig();

}



uint32_t max31855_read(uint32_t chip)
{
	uint32_t data = 0;
	int i;
	max31855_cs(chip);
	MAX31855_SCK_LOW();
	for (i=0; i<32; i++) {
		data <<= 1;
		MAX31855_SCK_HIGH();
		if (MAX31855_READ_SO()) {
			data |= 0x01;
		}
		MAX31855_SCK_LOW();
	}
	max31855_cs(0);
	delay_us(1000);
	
	return max31855_temp_ext(data);
}


/**
  * @brief ��MAX31855�ȶ�14λ��ż�¶Ȳɼ�
  *
  */
// float MAX31855_ReadData_1(void)
// {
//     uint32_t temp=0;
// 	  int16_t tmp=0;
// 	  int i=0;
//     MAX31855_CS(0);
// 	  MAX31855_SCK(0);	  
//    	for(i=0;i<32;i++)
// 	  {			
// 			 temp<<=1;
// 			 MAX31855_SCK(1);
// 			 if(GPIO_ReadInputDataBit(MAX31855_Port_SO,MAX31855_Pin_SO)==1)
// 			 temp |=0x01; else temp |=0x00;		  
// 			 MAX31855_SCK(0);
//     } 
//     MAX31855_CS(1);
// 		for(i=0;i<100;i++)
// 		delay_us(1000);
// 		
// 	  /*�ⲿ�¶Ȼ�����*/
//     tmp=(int16_t)((temp >>18)&0x3fff);
// 		tmp<<=2;
// 		tmprtr=tmp*0.25/4;
// 		
// 		/*�ڲ��¶Ȼ�����*/
//  		tmp=(int16_t)((temp >>4)&0xfff);
// 		tmp<<=4;
// 		ntrnl_tmprtr=tmp*0.0625/16;
// 		
// 		/*�Ƿ�·��־λ���*/
// 		OC_BIT= temp&0x01;
// 		
// 	  /*�Ƿ��·���ر�־λ���*/
// 		SCG_BIT= (temp>>1)&0x01;
// 		
// 		/*�Ƿ��·��VCC��־λ���*/
// 		SCV_BIT= (temp>>2)&0x01;
// 		
// 		/*�Ƿ�ʧ�ܱ�־λ���*/
// 		FAULT= (temp>>16)&0x01;
// 		return tmprtr;

// }

// float MAX31855_ReadData_2(void)
// {
//     uint32_t temp=0;
// 	  int16_t tmp=0;
// 	  int i=0;
//     MAX31855_CS_2(0);
// 	  MAX31855_SCK(0);	  
//    	for(i=0;i<32;i++)
// 	  {			
// 			 temp<<=1;
// 			 MAX31855_SCK(1);
// 			 if(GPIO_ReadInputDataBit(MAX31855_Port_SO,MAX31855_Pin_SO)==1)
// 			 temp |=0x01; else temp |=0x00;		  
// 			 MAX31855_SCK(0);
//     } 
//     MAX31855_CS_2(1);
// 		for(i=0;i<100;i++)
// 		delay_us(1000);
// 		
// 	  /*�ⲿ�¶Ȼ�����*/
//     tmp=(int16_t)((temp >>18)&0x3fff);
// 		tmp<<=2;
// 		tmprtr_m=tmp*0.25/4;
// 		
// 		/*�ڲ��¶Ȼ�����*/
//  		tmp=(int16_t)((temp >>4)&0xfff);
// 		tmp<<=4;
// 		ntrnl_tmprtr_m=tmp*0.0625/16;
// 		
// 		/*�Ƿ�·��־λ���*/
// 		OC_BIT= temp&0x01;
// 		
// 	  /*�Ƿ��·���ر�־λ���*/
// 		SCG_BIT= (temp>>1)&0x01;
// 		
// 		/*�Ƿ��·��VCC��־λ���*/
// 		SCV_BIT= (temp>>2)&0x01;
// 		
// 		/*�Ƿ�ʧ�ܱ�־λ���*/
// 		FAULT= (temp>>16)&0x01;
// 		return tmprtr_m;

// }


// float MAX31855_ReadData_3(void)
// {
//     uint32_t temp=0;
// 	  int16_t tmp=0;
// 	  int i=0;
//     MAX31855_CS_3(0);
// 	  MAX31855_SCK(0);	  
//    	for(i=0;i<32;i++)
// 	  {			
// 			 temp<<=1;
// 			 MAX31855_SCK(1);
// 			 if(GPIO_ReadInputDataBit(MAX31855_Port_SO,MAX31855_Pin_SO)==1)
// 			 temp |=0x01; else temp |=0x00;		  
// 			 MAX31855_SCK(0);
//     } 
//     MAX31855_CS_3(1);
// 		for(i=0;i<100;i++)
// 		delay_us(1000);
// 		
// 	  /*�ⲿ�¶Ȼ�����*/
//     tmp=(int16_t)((temp >>18)&0x3fff);
// 		tmp<<=2;
// 		tmprtr_b=tmp*0.25/4;
// 		
// 		/*�ڲ��¶Ȼ�����*/
//  		tmp=(int16_t)((temp >>4)&0xfff);
// 		tmp<<=4;
// 		ntrnl_tmprtr_b=tmp*0.0625/16;
// 		
// 		/*�Ƿ�·��־λ���*/
// 		OC_BIT= temp&0x01;
// 		
// 	  /*�Ƿ��·���ر�־λ���*/
// 		SCG_BIT= (temp>>1)&0x01;
// 		
// 		/*�Ƿ��·��VCC��־λ���*/
// 		SCV_BIT= (temp>>2)&0x01;
// 		
// 		/*�Ƿ�ʧ�ܱ�־λ���*/
// 		FAULT= (temp>>16)&0x01;
// 		return tmprtr_b;

// }







