#ifndef __FILTER_H
#define __FILTER_H

#include "stm32f10x.h"

int filter_data_request(char* filter_buf);
void filter_platform(void);
void utc_to_bj(char * filter_time);
#endif
