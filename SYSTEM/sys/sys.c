#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ϵͳ�жϷ������û�		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/10
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************  
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;						//����NVIC��ʼ���ṹ��

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				    //�����ж����ȼ���Ϊ2����ռ���ȼ��ʹ����ȼ�����λ(����0��3)
	
	/*��ʱ��2�ж���������*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				    //�����ж�������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//�����������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			  //������Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				    //ʹ��NVIC
	NVIC_Init(&NVIC_InitStructure);
	/*����1�ж���������*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;				  //�����ж�������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//�����������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			  //������Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				    //ʹ��NVIC
	NVIC_Init(&NVIC_InitStructure);
  /*����2�ж���������*/
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;				  //�����ж�������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;	//�����������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			  //������Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				    //ʹ��NVIC
	NVIC_Init(&NVIC_InitStructure);
	 /*����3�ж���������*/
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;				  //�����ж�������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//�����������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			  //������Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				    //ʹ��NVIC
	NVIC_Init(&NVIC_InitStructure);
}


