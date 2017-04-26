#ifndef PRESSURE_H
#define PRESSURE_H

#include<stm32f10x.h>

void  Adc_Init(void);
u16 Get_Adc(u8 ch) ;
u16 Get_Adc_Average(u8 ch,u8 times);
u32 get_pressure_1(void);
u32 get_pressure_2(void);
u32 get_pressure_3(void);
#endif
