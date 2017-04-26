/*****************************************
*瑞丽光电，AP-RSIG1283A驱动程序
*xiaoan
******************************************/
#include "usart.h"
#include "user_config.h"
#include "pressure.h"
#include "lcd_rl.h"
#include "gprs_normal.h"
#include "max6675.h"
#include "stdlib.h"
#include "string.h"
#include "proto.h"
#include "delay.h"
static struct rs232_port_t *m_rs232 = NULL;
char press1[10];
char press2[10];
char press3[10];
char temp1[4];
char temp2[4];
char temp3[4];
static int s_proto_read(void *ctx, unsigned char *ch, unsigned int timeoutms)
{
	;
// 	struct rs232_port_t *m_rs232 = ctx;
// 	unsigned int readlen = 0;
// 	rs232_read_timeout(m_rs232, ch, 1, &readlen, timeoutms);
// 	return readlen == 1;
	return 1;
}

static int s_proto_write(void *ctx, unsigned char ch)
{
// 	struct rs232_port_t *m_rs232 = ctx;
// 	unsigned int writelen = 0;
// 	rs232_write(m_rs232, &ch, 1, &writelen);
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC)==RESET); 
	USART_SendData(USART3 ,ch);//发送当前字符
	return 1;
}

static void uint16_to_be16_bytes(unsigned short x, unsigned char *p)
{
	p[0] = (x >> 8) & 0xFF;
	p[1] = (x) & 0xFF;
}


void lcd_init(void)
{
	proto_set_fn(m_rs232, s_proto_read, s_proto_write);
  lcd_showdata();
	lcd_led_off();
	led_offs();
	lcd_show_release_num();
}

void lcd_showdata(void)
{
	unsigned char buf_tx[20];
	buf_tx[0] = CMD_STATE;
	buf_tx[1] = 12;
	uint16_to_be16_bytes(atoi(press1), &buf_tx[2]);
	uint16_to_be16_bytes(atoi(press2), &buf_tx[4]);
	uint16_to_be16_bytes(0, &buf_tx[6]);
	if(strlen(temp1) <= 3)
	uint16_to_be16_bytes(atoi(temp1), &buf_tx[8]);
	else
	uint16_to_be16_bytes(999, &buf_tx[8]);
 	if(strlen(temp2) <= 3)
	uint16_to_be16_bytes(atoi(temp2), &buf_tx[10]);
	else
	uint16_to_be16_bytes(999, &buf_tx[10]);
	if(strlen(temp3) <= 3)
	uint16_to_be16_bytes(atoi(temp3), &buf_tx[12]);
	else
	uint16_to_be16_bytes(999, &buf_tx[12]);
	
	if(atoi(press1) > 50000 || atoi(press2) > 50000  || atoi(temp1) > 500 || atoi(temp2) > 500 || atoi(temp3) > 500)
	{led_red();lcd_beep_on();}
	else if(atoi(press1) >= 20000)
	{led_yellow();lcd_beep_on();}
	else 
	led_green();
	proto_write_frame(buf_tx, buf_tx[1]+2);

}

void lcd_led_on(void)
{
	unsigned char buf_tx[5];
	buf_tx[0] = CMD_LED;
	buf_tx[1] = 1;
	buf_tx[2] = 0xFF;
  proto_write_frame(buf_tx, buf_tx[1]+2);
	//b = proto_read_frame(buf_rx, sizeof(buf_rx), 1000);
}

void lcd_led_off(void)
{
	unsigned char buf_tx[5];
	buf_tx[0] = CMD_LED;
	buf_tx[1] = 1;
	buf_tx[2] = 0x00;
  proto_write_frame(buf_tx, buf_tx[1]+2);
	//b = proto_read_frame(buf_rx, sizeof(buf_rx), 1000);
}


// void lcd_gps_led_on(void)
// {
// 	unsigned char buf_tx[128];
// 	buf_tx[0] = CMD_LED;
// 	buf_tx[1] = 1;
// 	buf_tx[2] = 0xF0;
//   proto_write_frame(buf_tx, buf_tx[1]+2);
// 	//b = proto_read_frame(buf_rx, sizeof(buf_rx), 1000);
// }

// void lcd_gps_led_off(void)
// {
// 	unsigned char buf_tx[128];
// 	buf_tx[0] = CMD_LED;
// 	buf_tx[1] = 1;
// 	buf_tx[2] = 0x0F;
//   proto_write_frame(buf_tx, buf_tx[1]+2);
// 	//b = proto_read_frame(buf_rx, sizeof(buf_rx), 1000);
// }

void led_offs(void)
{
	unsigned char buf_tx[5];
	buf_tx[0] = CMD_CLED;
	buf_tx[1] = 1;
	buf_tx[2] = 0x00;
	proto_write_frame(buf_tx, buf_tx[1]+2);
}


void led_red(void)
{
	unsigned char buf_tx[5];
	buf_tx[0] = CMD_CLED;
	buf_tx[1] = 1;
	buf_tx[2] = 0x01;
	proto_write_frame(buf_tx, buf_tx[1]+2);
}

void led_yellow(void)
{
	unsigned char buf_tx[5];
	buf_tx[0] = CMD_CLED;
	buf_tx[1] = 1;
	buf_tx[2] = 0x02;
	proto_write_frame(buf_tx, buf_tx[1]+2);
}

void led_green(void)
{
	unsigned char buf_tx[5];
	buf_tx[0] = CMD_CLED;
	buf_tx[1] = 1;
	buf_tx[2] = 0x03;
	proto_write_frame(buf_tx, buf_tx[1]+2);
}

void lcd_beep_one(void)
{
	unsigned char buf_tx[5];
	buf_tx[0] = CMD_BEEP;
	buf_tx[1] = 1;
	buf_tx[2] = 0x01;
	proto_write_frame(buf_tx, buf_tx[1]+2);

}

void lcd_beep_on(void)
{

	unsigned char buf_tx[5];
	buf_tx[0] = CMD_BEEP;
	buf_tx[1] = 1;
	buf_tx[2] = 0x01;
	proto_write_frame(buf_tx, buf_tx[1]+2);

}

void lcd_beep_off(void)
{
	unsigned char buf_tx[5];
	buf_tx[0] = CMD_BEEP;
	buf_tx[1] = 1;
	buf_tx[2] = 0x01;
	proto_write_frame(buf_tx, buf_tx[1]+2);

}

void lcd_show_release_num(void)
{
	unsigned char buf_tx[20];
	
	lcd_beep_one();
	
	buf_tx[0] = CMD_STATE;
	buf_tx[1] = 12;
	uint16_to_be16_bytes(0, &buf_tx[2]);
	uint16_to_be16_bytes(0, &buf_tx[4]);
	uint16_to_be16_bytes(0, &buf_tx[6]);
	uint16_to_be16_bytes(17, &buf_tx[8]);
	uint16_to_be16_bytes(3, &buf_tx[10]);
	uint16_to_be16_bytes(17, &buf_tx[12]);
	proto_write_frame(buf_tx, buf_tx[1]+2);
  delay_ms(2000);
}


