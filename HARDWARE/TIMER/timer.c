#include "timer.h"
#include "stm32f10x_tim.h"
#include "user_config.h"

// extern vu8 Timer0_start;
// extern u8 shijian;
// extern u8 Times;

u8 shijian=0;u16 Times=0;      //延时变量
u8 Time_UART1=0;  //串口1计时器
u8 Time_UART2=0;  //串口2计时器
u8 Time_UART3=0;  //串口2计时器
int Time_miao=0;
u8 Gps_time = 0;
u8 Time_gps = 0;
u8 Conn_miao = 0;//连接计时器
int sh_reg_miao = 0;//注册计时器
u8 reg_ack = 0; //注册等待计时器
u8 Time_fac = 0; //工厂模式计时器
u8 Time_filter = 0; //工厂模式计时器
u8 Time_ack = 0;
int Time_send = 0;
u8 Time_send_wait = 0; 
int heart_miao = 0; //断开后重连的时间
u8 Time_answer = 0;
u8 Timer0_start=0;	//定时器0延时启动计数器
u8 Uart2_Start=0;	  //串口2开始接收数据
u8 Uart2_End=0;	    //串口2接收数据结束
int Time_off = 0;
u8 Uart3_Start=0;	  //串口2开始接收数据
u8 Uart3_End=0;	    //串口2接收数据结束
u8 Time_reg_response=0;
vu8 Time_send_ok = 0;


/*******************************************************************************
* 函数名  : RCC_Configuration
* 描述    : 设置系统时钟为72MHZ(这个可以根据需要改)
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : STM32F107x和STM32F105x系列MCU与STM32F103x系列MCU时钟配置有所不同
*******************************************************************************/
void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;               //HSE工作状态变量
  
  RCC_DeInit();                               //将所有与时钟相关的寄存器设置为默认值
  RCC_HSEConfig(RCC_HSE_ON);                  //启动外部高速时钟HSE 
  HSEStartUpStatus = RCC_WaitForHSEStartUp(); //等待外部高速时钟(HSE)稳定

  if(SUCCESS == HSEStartUpStatus)             //如果外部高速时钟已经稳定
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //Flash设置
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    
  
    RCC_HCLKConfig(RCC_SYSCLK_Div1); //设置AHB时钟等于系统时钟(1分频)/72MHZ
    RCC_PCLK2Config(RCC_HCLK_Div1);  //设置APB2时钟和HCLK时钟相等/72MHz(最大为72MHz)
    RCC_PCLK1Config(RCC_HCLK_Div2);  //设置APB1时钟是HCLK时钟的2分频/36MHz(最大为36MHz)
  
#ifndef STM32F10X_CL                 //如果使用的不是STM32F107x或STM32F105x系列MCU,PLL以下配置  
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); //PLLCLK = 8MHz * 9 = 72 MHz 
#else                                //如果使用的是STM32F107x或STM32F105x系列MCU,PLL以下配置
    /***** 配置PLLx *****/
    /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    RCC_PLL2Config(RCC_PLL2Mul_8);

    RCC_PLL2Cmd(ENABLE); //使能PLL2 
    while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET);//等待PLL2稳定

    /* PLL configuration: PLLCLK = (PLL2 / 5) * 9 = 72 MHz */ 
    RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#endif

    RCC_PLLCmd(ENABLE); //使能PLL
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET); //等待PLL稳定

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);          //设置系统时钟的时钟源为PLL

    while(RCC_GetSYSCLKSource() != 0x08);               //检查系统的时钟源是否是PLL
    RCC_ClockSecuritySystemCmd(ENABLE);                 //使能系统安全时钟 
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,ENABLE);
  }
}


/*******************************************************************************
* 函数名  : Timer2_Init_Config
* 描述    : Timer2初始化配置
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 1s延时
*******************************************************************************/
void Timer2_Init_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);		//使能Timer2时钟
	
	TIM_TimeBaseStructure.TIM_Period = 9999;					//设置在下一个更新事件装入活动的自动重装载寄存器周期的值(计数到10000为1s)
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;					//设置用来作为TIMx时钟频率除数的预分频值(10KHz的计数频率)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//设置时钟分割:TDTS = TIM_CKD_DIV1
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);				//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	/*中断优先级NVIC设置*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				//TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能IRQ通道
	NVIC_Init(&NVIC_InitStructure); 							//初始化NVIC寄存器
	 
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE ); 				//使能TIM2指定的中断
	
	TIM_Cmd(TIM2, ENABLE);  									//使能TIMx外设
}	 


/*******************************************************************************
* 函数名  : TIM2_IRQHandler
* 描述    : 定时器2中断断服务函数
* 输入    : 无
* 输出    : 无
* 返回    : 无 
* 说明    : 无
*******************************************************************************/
void TIM2_IRQHandler(void)   //TIM3中断
{
	static u8 flag =1;

	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 

		if(flag)
		{
			LED4_ON(); 
			flag=0;
		}
		else
		{
			LED4_OFF(); 
			flag=1;
		}
		if(Time_miao>=1)Time_miao++;//Time_miao==1时启动计数，溢出自动停止
		if(Gps_time>=1) Gps_time++;
		if(Conn_miao>=1) Conn_miao++;
		if(sh_reg_miao>=1) sh_reg_miao++;
		if(Time_fac>=1) Time_fac++;
		if(Time_filter>=1) Time_filter++;
		if(heart_miao>=1) heart_miao++;
		if(Time_answer>=1) Time_answer++;
		if(Time_gps>=1) Time_gps++;
		if(Time_send>=1) Time_send++;
		if(Time_send_wait>=1) Time_send_wait++;
		if(Time_off>=1) Time_off++;
		if(Time_reg_response>=1) Time_reg_response++;
		if(Time_send_ok >= 1)Time_send_ok++;
 		if(Timer0_start)
		Times++;
		if(Times > (shijian))
		{
			Timer0_start = 0;
			Times = 0;
		}
		Time_UART2++;
		Time_UART3++;
		
		if(USART_RX_STA&(~0x8000))//正在接收状态中
		{
			Time_UART1++;
			if(Time_UART1>=200)USART_RX_STA=0;//接收超时，复位接收
		}
	}	
}


