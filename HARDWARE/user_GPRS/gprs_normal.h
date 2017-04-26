#ifndef __GPRS_NORMAL_H
#define __GPRS_NORMAL_H
#include "stm32f10x.h"

u8 gprs_chejia_mode(void);
u8 chejia_protocol(void);
u8 henan_protocol(void);
char* itoa(int num,char*str,int radix);
char* makeprotocol(void);
char* fix_zero(char *expand_str,char *expand_length);
#endif
