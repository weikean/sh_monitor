#ifndef __LCD_RL_H
#define __LCD_RL_H

#include "stm32f10x.h"
void lcd_showdata(void);

void lcd_init(void);
void lcd_led_on(void);
void lcd_led_off(void);
void led_offs(void);
void led_red(void);
void led_yellow(void);
void led_green(void);
void lcd_beep_one(void);
void lcd_beep_on(void);
void lcd_beep_off(void);
void lcd_show_release_num(void);
#endif  
