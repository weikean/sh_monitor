#include "gsm.h"
#include "string.h"
#include "usart.h"	  
#include "user_config.h"
#include "delay.h"
#include "gprs_jiankong.h"
#include "flash.h"
#include "lcd_rl.h"
#include "filter.h"
#include "stdlib.h"
#include "mmc_sd.h"
#include "ff.h"  
#include "exfuns.h"
#include "gprs_sh.h" 
void sim808_init(void)
{
	
  u8 sim_ready = 0;
  u32 sd_size = 0;
 	UART1_SendString("�ȴ�ģ������\r\n");
	delay_ms(6000);
	Time_fac = 1;
	bind_vid b_vid;
	
	while(SD_Initialize())//��ⲻ��SD��
	{
		printf("SD Card Error!\r\n");
		delay_ms(500);					
	}
	exfuns_init();							//Ϊfatfs��ر��������ڴ�				 
  f_mount(fs[0],"0:",1); 					//����SD�� 

	sd_size=SD_GetSectorCount();//�õ�������
	printf("SD Size: %d MB\r\n",sd_size>>11);
	
	f_unlink("0:log.txt");
	f_unlink("0:log1.txt");		
	f_open(vid_log,"0:vid.txt",FA_OPEN_ALWAYS | FA_WRITE| FA_READ);
	f_open(data_log,"0:log.txt",FA_CREATE_ALWAYS | FA_WRITE| FA_READ);
	f_open(data_log_1,"0:log1.txt",FA_CREATE_ALWAYS | FA_WRITE| FA_READ);
	f_open(offline,"0:offline.txt",FA_OPEN_ALWAYS | FA_WRITE| FA_READ);
	
	lcd_led_on();
 	lcd_beep_one();
	UART1_SendString("\r\n����5s�ӽ��빤��ģʽ\r\n");
	while(Time_fac <= 6)
	{
		if(strstr((const char*)Uart1_Buf,"����"))
			GSM_gprs_test();
	} Time_fac = 0;	

		while(GSM_send_cmd((u8*)"AT",(u8*)"OK",5))//��ѯ�Ƿ�Ӧ��ATָ��
	{
		UART1_SendString("δ��⵽ģ��\r\n");
		delay_ms(800);
		UART1_SendString("���ڳ�������\r\n");
		delay_ms(400);  
	} 
	
	
	GSM_send_cmd((u8*)"ATE0",(u8*)"OK",200);//������
	if(GSM_gsminfo()==0)
	{
		sim_ready=1;
	}
	if(sim_ready == 1)
	{	
	gps_init();
	sim808_conn_init();
	delay_ms(800);
//	ntp_init();

  STMFLASH_Read(FLASH_SAVE_ID,(u16*)id_len,1);
	STMFLASH_Read(FLASH_SAVE_ID,(u16*)device,id_len[0]);
	device[13] = '\0';
	if(strlen((const char*)device) > 5)
	{
		 UART1_SendString("��ȡid :");
     UART1_SendString((char *)device);
	   UART1_SendString("\r\n");
	}
		
   UART1_SendString("\r\n��������ģʽ\r\n");
	
}
	else
	{
  while(1)
	{
		UART1_SendString("���sim��,������!!!!!\r\n");
		delay_ms(3000);
	}
}
}


void sim808_conn_init(void)
{
 	  GSM_send_cmd("AT+CIPCLOSE=0","CLOSE OK",2);	//�ر�����
	  GSM_send_cmd("AT+CIPCLOSE=1","CLOSE OK",2);	//�ر�����
 		GSM_send_cmd("AT+CIPSHUT","SHUT OK",8);		//�ر��ƶ�����
		Second_AT_Command("AT+CGCLASS=\"B\"","OK",8);//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ���
		Second_AT_Command("AT+CGATT=1","OK",8);//����GPRSҵ��
		Second_AT_Command("AT+CIPMUX=1","OK",8);//����Ϊ��·����
		Second_AT_Command("AT+CSTT=\"CMNET\"","OK",8);//����ΪGPRS����ģʽ
		Second_AT_Command("AT+CIICR","OK",8);//�������糡��
	//	Second_AT_Command("AT+SAPBR=1,1","OK",8);//�������糡��
		UART2_SendString("AT+CIFSR\r\n");//��ģ��IP�����������
	//  Second_AT_Command("AT+CIPQSEND=1","OK",10);//�췢
    CLR_Buf2();	
}


void ntp_init(void)
{
	Second_AT_Command("AT+CNTP=\"202.108.6.95\",8","OK",8);//����ntp������
	if(GSM_send_cmd("AT+CNTP","+CNTP: 1",15) == 0)
	{
		printf("ntp ok\r\n");
	}
}

int ntp_to_gpstime(char *time)
{
	char *p1,*p2;
	u8 i = 0,start = 0;
	u8 year[2];
	
	GSM_send_cmd("AT+CCLK?","OK",10);//�������糡��
	if((p1=(char*)strstr((const char*)Uart2_Buf,"+CCLK:")),(p1 != NULL))
	{	
		if((p2=(char*)strstr((const char*)p1,"+")),(p2!=NULL))//Ѱ�ҽ�����
		{
		while(p2[i])
		{
			if(p2[i] == '\"') 
			{
				start = i;
				break;
			}
			i++;
    }
	}	
		memcpy(time,p2+start+1,2);
		memcpy(year,p2+start+1,2);
	  memcpy(time+2,p2+start+4,2);
	  memcpy(time+4,p2+start+7,2);
	  memcpy(time+6,p2+start+10,2);
	  memcpy(time+8,p2+start+13,2);
	  memcpy(time+10,p2+start+16,2);
	//	printf("ntp time: %s",time);	
	}
	
	if(atoi((char*)year) >= 16) return 1;
	else return 0;
}



/*******************************************************************************
* ������ : GSM_send_cmd
* ����   : ����ATָ���
* ����   : �������ݵ�ָ�롢���͵ȴ�ʱ��(��λ��S)
* ���   : 
* ����   : 0:����  1:����
* ע��   : 
*******************************************************************************/

u8 GSM_send_cmd(u8 *cmd,u8 *ack,u8 wait_time)         
{
	u8 res=1;
	//u8 *c;
	//c = cmd;										//�����ַ�����ַ��c
	if(recieving == 0)
	CLR_Buf2(); 
	for (; *cmd!='\0';cmd++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
	  USART_SendData(USART2,*cmd);
	}
	UART2_SendString("\r\n");	
	if(wait_time==0)return res;
	Times = 0;
	shijian = wait_time;
	Timer0_start = 1;
	while(Timer0_start&res)                    
	{
		if(strstr((const char*)Uart2_Buf,(char*)ack)==NULL)
			 res=1;
		else
		{
			 res=0;
		}

	}
	return res;
}

/*******************************************************************************
* ������ : Find
* ����   : �жϻ������Ƿ���ָ�����ַ���
* ����   : 
* ���   : 
* ����   : unsigned char:1 �ҵ�ָ���ַ���0 δ�ҵ�ָ���ַ� 
* ע��   : 
*******************************************************************************/

u8 Find(char *a)
{ 
  if(strstr((char*)Uart2_Buf,a)!=NULL)
	    return 1;
	else
			return 0;
}

/*******************************************************************************
* ������ : Second_AT_Command
* ����   : ����ATָ���
* ����   : �������ݵ�ָ�롢���͵ȴ�ʱ��(��λ��S)
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/

void Second_AT_Command(char *b,char *a,u8 wait_time)         
{
	u8 i;
	char *c;
	c = b;										//�����ַ�����ַ��c
	CLR_Buf2(); 
  i = 0;
	while(i == 0)                    
	{
		if(!Find(a)) 
		{
			if(Timer0_start == 0)
			{
				b = c;							//���ַ�����ַ��b
				for (; *b!='\0';b++)
				{
					while(USART_GetFlagStatus(USART2, USART_FLAG_TC)==RESET);
					USART_SendData(USART2,*b);//UART2_SendData(*b);
				}
				UART2_SendString("\r\n");	
				Times = 0;
				shijian = wait_time;
				Timer0_start = 1;
		   }
    }
 	  else
		{
			i = 1;
			Timer0_start = 0;
		}
	}
	CLR_Buf2(); 
}

