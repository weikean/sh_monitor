#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H
#include "includes.h"
#define Buf1_Max 128					  //����1���泤��
#define Buf2_Max 1024	          //����2���泤��
#define SYSTEM_SUPPORT_UCOS 1
#define FLASH_SAVE_FACTORY 0X0801F100
#define FLASH_SAVE_PHONE   0X0801F200 
#define FLASH_SAVE_ID      0X0801F300         //�豸ID�����ַ  
#define FLASH_SAVE_HADDR   0X0801FA00 				//����ip�����ַ
#define FLASH_SAVE_CJET    0X0801F800 				//����ip�����ַ
#define FLASH_SAVE_TIME    0X0801FB00 				//ͨ��Ƶ��
#define FLASH_SAVE_VID     0X0801FE00 				//vid

extern u8 Uart1_Buf[Buf1_Max];//����1���ݻ�����
extern u8 Uart2_Buf[Buf2_Max];//����2���ݻ�����
extern u8 Uart3_Buf[128];//����3���ݻ�����
extern u8 gps_buf[20];
extern u8 First_Int3 ;
extern u8 First_Int ;
extern u16 USART_RX_STA;
extern u16 USART_RX_STA2;
extern u8 Time_UART1;  //����1��ʱ��
extern u8 Time_UART2;  //����2��ʱ��
extern u8 Time_UART3;  //����2��ʱ��
extern u8 Time_filter; 
extern u8 Conn_miao;
extern int Time_send;
extern u8 Time_send_wait;
extern u8 shijian;
extern u16 Times;      //��ʱ����
extern u8 Timer0_start;	//��ʱ��0��ʱ����������
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

