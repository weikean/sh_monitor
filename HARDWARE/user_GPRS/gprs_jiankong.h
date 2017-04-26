#ifndef __GPRS_JIANKONG
#define __GPRS_JIANKONG

#include "stm32f10x.h"
#include "string.h"
#include "usart.h"
#include "user_config.h"
#include "gsm.h"
#include "SysTick.h"
#include <stdlib.h> 
#include <ctype.h> 
#include <stdio.h> 
#include "flash.h"
#include "delay.h"
#include "radix.h"
#include "gprs_sh.h"
#include "math.h"
#include "hexstring.h"
#include "exfuns.h"
#include "mmc_sd.h"	

void GSM_mtest(void);
u8 GSM_gsminfo(void);//GSM��Ϣ��ʾ(�ź�����,��ص���,����ʱ��)
void GPRS_main(void);//GPRS������
//void GSM_gprs_test(void);
void GPRS_main(void);
int get_GPS_data(void);
u8 GSM_gprs_test(void);
void gps_init(void);
int gps_check(char *longitude,char *latitude);
int vid_id_check(bind_vid *v);

typedef struct GPS_INFO
{
 char gps_time[20];
 char gps_longitude[11];
 char gps_latitude[10];
 char gps_speed[6];
 char gps_course[7];
 char gps_speed_not[6];

} gps_info;
extern u8 gps_flag;
#endif
