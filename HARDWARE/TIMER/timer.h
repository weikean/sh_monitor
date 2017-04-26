#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"
#include "Led.h"

void Timer2_Init_Config(void);
void TIM2_IRQHandler(void) ;
void RCC_Configuration(void);
#endif
