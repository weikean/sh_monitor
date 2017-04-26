#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#include "includes.h"
#define Buf1_Max 128					  //串口1缓存长度
#define Buf2_Max 1024	          //串口2缓存长度
#define SYSTEM_SUPPORT_UCOS 1
#define FLASH_SAVE_FACTORY 0X0801F100
#define FLASH_SAVE_PHONE   0X0801F200 
#define FLASH_SAVE_ID      0X0801F300         //设备ID保存地址  
#define FLASH_SAVE_HADDR   0X0801FA00 				//河南ip保存地址
#define FLASH_SAVE_CJET    0X0801F800 				//车佳ip保存地址
#define FLASH_SAVE_TIME    0X0801FB00 				//通信频率
#define FLASH_SAVE_VID     0X0801FE00 				//vid

extern u8 Uart1_Buf[Buf1_Max];//串口1数据缓存区
extern u8 Uart2_Buf[Buf2_Max];//串口2数据缓存区
extern u8 Uart3_Buf[128];//串口3数据缓存区
extern u8 gps_buf[20];
extern u8 First_Int3 ;
extern u8 First_Int ;
extern u16 USART_RX_STA;
extern u16 USART_RX_STA2;
extern u8 Time_UART1;  //串口1计时器
extern u8 Time_UART2;  //串口2计时器
extern u8 Time_UART3;  //串口2计时器
extern u8 Time_filter; 
extern u8 Conn_miao;
extern int Time_send;
extern u8 Time_send_wait;
extern u8 shijian;
extern u16 Times;      //延时变量
extern u8 Timer0_start;	//定时器0延时启动计数器
extern int Time_miao;
extern u8 Gps_time ;
extern u8 Time_fac;
extern u8 reg_ack;
extern int heart_miao;
extern u8 Time_answer;
extern u8 Time_ack;
extern u8 Time_reg_response;
extern char device[14];
extern char lati[10];
extern char longi[10];
extern int sh_reg_miao;
extern int Time_off;
extern vu8 Time_send_ok;

extern char press1[10];
extern char press2[10];
extern char press3[10];
extern char temp1[4];
extern char temp2[4];
extern char temp3[4];
extern OS_EVENT *Semsd;
extern u8 cjip[60];
extern u8 cj_len[2];
extern u8 id_len[2];
extern char my_clock[13];
extern u8 reg_ok;
extern int w_flag;
#endif

