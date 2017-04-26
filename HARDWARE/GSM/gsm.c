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
 	UART1_SendString("等待模块启动\r\n");
	delay_ms(6000);
	Time_fac = 1;
	bind_vid b_vid;
	
	while(SD_Initialize())//检测不到SD卡
	{
		printf("SD Card Error!\r\n");
		delay_ms(500);					
	}
	exfuns_init();							//为fatfs相关变量申请内存				 
  f_mount(fs[0],"0:",1); 					//挂载SD卡 

	sd_size=SD_GetSectorCount();//得到扇区数
	printf("SD Size: %d MB\r\n",sd_size>>11);
	
	f_unlink("0:log.txt");
	f_unlink("0:log1.txt");		
	f_open(vid_log,"0:vid.txt",FA_OPEN_ALWAYS | FA_WRITE| FA_READ);
	f_open(data_log,"0:log.txt",FA_CREATE_ALWAYS | FA_WRITE| FA_READ);
	f_open(data_log_1,"0:log1.txt",FA_CREATE_ALWAYS | FA_WRITE| FA_READ);
	f_open(offline,"0:offline.txt",FA_OPEN_ALWAYS | FA_WRITE| FA_READ);
	
	lcd_led_on();
 	lcd_beep_one();
	UART1_SendString("\r\n你有5s钟进入工厂模式\r\n");
	while(Time_fac <= 6)
	{
		if(strstr((const char*)Uart1_Buf,"工厂"))
			GSM_gprs_test();
	} Time_fac = 0;	

		while(GSM_send_cmd((u8*)"AT",(u8*)"OK",5))//查询是否应到AT指令
	{
		UART1_SendString("未检测到模块\r\n");
		delay_ms(800);
		UART1_SendString("正在尝试连接\r\n");
		delay_ms(400);  
	} 
	
	
	GSM_send_cmd((u8*)"ATE0",(u8*)"OK",200);//不回显
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
		 UART1_SendString("读取id :");
     UART1_SendString((char *)device);
	   UART1_SendString("\r\n");
	}
		
   UART1_SendString("\r\n进入正常模式\r\n");
	
}
	else
	{
  while(1)
	{
		UART1_SendString("请插sim卡,并重启!!!!!\r\n");
		delay_ms(3000);
	}
}
}


void sim808_conn_init(void)
{
 	  GSM_send_cmd("AT+CIPCLOSE=0","CLOSE OK",2);	//关闭连接
	  GSM_send_cmd("AT+CIPCLOSE=1","CLOSE OK",2);	//关闭连接
 		GSM_send_cmd("AT+CIPSHUT","SHUT OK",8);		//关闭移动场景
		Second_AT_Command("AT+CGCLASS=\"B\"","OK",8);//设置GPRS移动台类别为B,支持包交换和数据交换
		Second_AT_Command("AT+CGATT=1","OK",8);//附着GPRS业务
		Second_AT_Command("AT+CIPMUX=1","OK",8);//设置为多路连接
		Second_AT_Command("AT+CSTT=\"CMNET\"","OK",8);//设置为GPRS连接模式
		Second_AT_Command("AT+CIICR","OK",8);//激活网络场景
	//	Second_AT_Command("AT+SAPBR=1,1","OK",8);//激活网络场景
		UART2_SendString("AT+CIFSR\r\n");//查模块IP，这个必须有
	//  Second_AT_Command("AT+CIPQSEND=1","OK",10);//快发
    CLR_Buf2();	
}


void ntp_init(void)
{
	Second_AT_Command("AT+CNTP=\"202.108.6.95\",8","OK",8);//连接ntp服务器
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
	
	GSM_send_cmd("AT+CCLK?","OK",10);//激活网络场景
	if((p1=(char*)strstr((const char*)Uart2_Buf,"+CCLK:")),(p1 != NULL))
	{	
		if((p2=(char*)strstr((const char*)p1,"+")),(p2!=NULL))//寻找结束符
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
* 函数名 : GSM_send_cmd
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：S)
* 输出   : 
* 返回   : 0:正常  1:错误
* 注意   : 
*******************************************************************************/

u8 GSM_send_cmd(u8 *cmd,u8 *ack,u8 wait_time)         
{
	u8 res=1;
	//u8 *c;
	//c = cmd;										//保存字符串地址到c
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
* 函数名 : Find
* 描述   : 判断缓存中是否含有指定的字符串
* 输入   : 
* 输出   : 
* 返回   : unsigned char:1 找到指定字符，0 未找到指定字符 
* 注意   : 
*******************************************************************************/

u8 Find(char *a)
{ 
  if(strstr((char*)Uart2_Buf,a)!=NULL)
	    return 1;
	else
			return 0;
}

/*******************************************************************************
* 函数名 : Second_AT_Command
* 描述   : 发送AT指令函数
* 输入   : 发送数据的指针、发送等待时间(单位：S)
* 输出   : 
* 返回   : 
* 注意   : 
*******************************************************************************/

void Second_AT_Command(char *b,char *a,u8 wait_time)         
{
	u8 i;
	char *c;
	c = b;										//保存字符串地址到c
	CLR_Buf2(); 
  i = 0;
	while(i == 0)                    
	{
		if(!Find(a)) 
		{
			if(Timer0_start == 0)
			{
				b = c;							//将字符串地址给b
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

