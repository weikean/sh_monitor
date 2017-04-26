#ifndef __GSM_H
#define __GSM_H

#include "stm32f10x.h"

u8 GSM_send_cmd(u8 *cmd,u8 *ack,u8 wait_time) ;
u8 Find(char *a);
void Second_AT_Command(char *b,char *a,u8 wait_time);
void sim808_init(void);
void sim808_conn_init(void);
int ntp_to_gpstime(char *time);
void ntp_init(void);
#endif
