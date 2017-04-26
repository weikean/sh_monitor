#include "lcd_rl.h"
#include "delay.h"
#include "sys.h"
#include "includes.h"
#include "stm32f10x.h"
#include "usart.h"
#include "SysTick.h"
#include "timer.h"
#include "user_config.h"
#include "gsm.h"
#include "gprs_jiankong.h"
#include "gprs_sh.h"
#include "pressure.h"
#include "filter.h"
#include "max31855.h"
#include "radix.h"
#include "malloc.h"	 
#include "ff.h"  
#include "exfuns.h"
#include "mmc_sd.h"
OS_EVENT *Semsd;

static OS_EVENT *task_semsd;
char device[14];
/////////////////////////UCOSII任务堆栈设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//创建任务堆栈空间	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数接口
void start_task(void *pdata);	

//CLOCK 任务
//设置任务优先级
#define CLOCK_TASK_PRIO      			3 
//设置任务堆栈大小
#define CLOCK_STK_SIZE  				64
//创建任务堆栈空间	
__align(8) OS_STK CLOCK_TASK_STK[CLOCK_STK_SIZE];
//任务函数接口
void clock_task(void *pdata);	

//LCD任务
//设置任务优先级
#define LCD_TASK_PRIO       			6
//设置任务堆栈大小
#define LCD_STK_SIZE  		    		64
//创建任务堆栈空间	
OS_STK LCD_TASK_STK[LCD_STK_SIZE];
//任务函数接口
void lcd_task(void *pdata);

//存储任务
//设置任务优先级
#define SD_TASK_PRIO       			5
//设置任务堆栈大小
#define SD_STK_SIZE  		    		256
//创建任务堆栈空间	
OS_STK SD_TASK_STK[SD_STK_SIZE];
//任务函数接口
void SD_task(void *pdata);


//传感器数据采集任务
//设置任务优先级
#define DATA_TASK_PRIO       			4
//设置任务堆栈大小
#define DATA_STK_SIZE  					 128
//创建任务堆栈空间	
OS_STK DATA_TASK_STK[DATA_STK_SIZE];
//任务函数接口
void data_task(void *pdata);

//main任务
//设置任务优先级
#define MAIN_TASK_PRIO       		 8
//设置任务堆栈大小
#define MAIN_STK_SIZE  					 768
//创建任务堆栈空间	
 __align(8) OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//任务函数接口

////filter协议
////设置任务优先级
//#define FILTER_TASK_PRIO       	 7
////设置任务堆栈大小
//#define FILTER_STK_SIZE  		     64
////创建任务堆栈空间	
//__align(8) static OS_STK FILTER_TASK_STK[FILTER_STK_SIZE];
////任务函数接口
//void filter_task(void *pdata);
 
 

void main_task(void *pdata);

	
 int main(void)
 {
	delay_init();	     //延时初始化
	Timer2_Init_Config();	 
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	GPIO_Config();
	uart_init(115200);
 	USART2_Init_Config(115200);
 	USART3_Init_Config(9600);
	mem_init(SRAMIN);		//初始化内部内存池
 	Adc_Init();
	MAX31855_Init();
	lcd_init();
	sim808_init(); 
	Times=0;
	shijian=0;
	Timer0_start=0; 
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();
 }

 
	  
//开始任务
void start_task(void *pdata)
{
	u8 err = 0;
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右
	sh_reg_miao = 0;
  Time_filter = 1;
	//Time_off = 1;
 	Semsd = OSMutexCreate(0,&err);
  task_semsd = OSMutexCreate(0,&err); 	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)
	OSTaskCreate(clock_task,(void *)0,(OS_STK*)&CLOCK_TASK_STK[CLOCK_STK_SIZE-1],CLOCK_TASK_PRIO);
 // OSTaskCreate(filter_task,(void *)0,(OS_STK*)&FILTER_TASK_STK[FILTER_STK_SIZE-1],FILTER_TASK_PRIO);	
 	OSTaskCreate(lcd_task,(void *)0,(OS_STK*)&LCD_TASK_STK[LCD_STK_SIZE-1],LCD_TASK_PRIO);	
 	OSTaskCreate(data_task,(void *)0,(OS_STK*)&DATA_TASK_STK[DATA_STK_SIZE-1],DATA_TASK_PRIO);
	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);
	OSTaskCreate(SD_task,(void *)0,(OS_STK*)&SD_TASK_STK[SD_STK_SIZE-1],SD_TASK_PRIO);	
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}


void clock_task(void *pdata)
{
	while(1)
	{
   plus_one_second(my_clock);
	 OSTimeDly(200);
	}           
}

void SD_task(void *pdata)
{	
	static int w_flag = 0;
	while(1)
	{
		if(reg_ok == 1)
		{	
		if(w_flag == 900)
		{
			w_flag = 0;
			Time_send = 1;
			file_finish = 1;
			file_option = !file_option;
			printf("file done\r\n");
		}
		  if(sd_encode())
			{	
				w_flag ++;
				if(w_flag % 100 == 0) 
					printf("data_num = %d\n",w_flag);				
			}
			else
			{
				w_flag = 0;
				printf("data_log_failed\n");
				f_unlink("0:log.txt");
				f_unlink("0:log1.txt");
				f_open(data_log,"0:log.txt",FA_CREATE_ALWAYS | FA_WRITE| FA_READ);
				f_open(data_log_1,"0:log1.txt",FA_CREATE_ALWAYS | FA_WRITE| FA_READ);
			}
	  }
   	  OSTimeDly(190);
	}
  
}


void data_task(void *pdata)
{	
	while(1)
	{
		itoa((int)(max31855_read(1)),temp1,10);//获取设备温度
		itoa((int)(max31855_read(2)),temp2,10);//获取设备温度
		itoa((int)(max31855_read(3)),temp3,10);//获取设备温度
		itoa(get_pressure_1(),press1,10);//获取设备压力
		itoa(get_pressure_2(),press2,10);
		itoa(0,press3,10);
		delay_ms(1000);
	}
}

void lcd_task(void *pdata)
{		
	while(1)
	{
	lcd_showdata();
	delay_ms(1200);
	}
}

//void filter_task(void *pdata)
//{
//	u8 err = 0;
//	while(1)
//	{
//	  if(Time_filter >= 120)
//	  {
//		OSMutexPend(task_semsd,0,&err);		
//		Time_filter = 1;
//		//filter_platform();
//		OSMutexPost(task_semsd);
//		}
//	  delay_ms(30000);
//	}           
//}


void main_task(void *pdata)
{	 
  u8 err = 0;
	while(1)
	{
	OSMutexPend(task_semsd,0,&err);	
	sh_task();
	OSMutexPost(task_semsd);		
	delay_ms(1000);
	}	
	
}
  		


