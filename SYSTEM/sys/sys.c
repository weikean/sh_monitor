#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//系统中断分组设置化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/10
//版本：V1.4
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************  
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;						//定义NVIC初始化结构体

  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				    //设置中断优先级组为2，先占优先级和从优先级各两位(可设0～3)
	
	/*定时器2中断向量配置*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				    //设置中断向量号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//设置抢先优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			  //设置响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				    //使能NVIC
	NVIC_Init(&NVIC_InitStructure);
	/*串口1中断向量配置*/
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;				  //设置中断向量号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//设置抢先优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			  //设置响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				    //使能NVIC
	NVIC_Init(&NVIC_InitStructure);
  /*串口2中断向量配置*/
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;				  //设置中断向量号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;	//设置抢先优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			  //设置响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				    //使能NVIC
	NVIC_Init(&NVIC_InitStructure);
	 /*串口3中断向量配置*/
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;				  //设置中断向量号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//设置抢先优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			  //设置响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				    //使能NVIC
	NVIC_Init(&NVIC_InitStructure);
}


