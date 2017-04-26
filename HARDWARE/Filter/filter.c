#include "filter.h"
#include "string.h"
#include "gprs_jiankong.h"	  
#include "user_config.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <usart.h>
#include <flash.h>
#include <gsm.h>
#include <delay.h>


extern gps_info gps;
char lati[10];
char longi[10];

/*******************************
* 菲尔特协议
********************************/
void filter_platform(void)
{
	char filter_buf[150] = "";
//	char data_len = 0;
  char ip_fail = 0;//ip地址错误
	char conn_fail = 1;//连接失败
	char sendok_flag = 0;//服务器接收到数据
	char exit_flag = 0;
	char conn_flag = 0;//连接成功标志
	int shmode = 1;
//  u8 cjip[60] = "AT+CIPSTART=0,\"TCP\",\"139.224.1.160\",9998";
	while (exit_flag == 0)
	{ 
	 if(conn_flag == 0){shmode = 1;}
	 else if(conn_flag == 1 && sendok_flag == 0){shmode = 0;}
	 else if(sendok_flag == 1){shmode = 2;}
   
	switch(shmode)
	{
		case 0:
		      filter_data_request(filter_buf);
		      if(GSM_send_cmd("AT+CIPSEND=1",">",8)==0){				
					UART2_SendString(filter_buf);
				  printf("log is = %s\r\n",filter_buf);
					UART2_Data(0X1A);//CTRL+Z,结束数据发送,启动一次传输
					UART1_SendString("已发送\r\n");
					CLR_Buf2();						
					}
          else
					{
						printf("filter sendcmd error\n");
						GSM_send_cmd("AT+CIPCLOSE=1","CLOSE OK",4);	//关闭连接
						GSM_send_cmd("AT+CIPSHUT","SHUT OK",4);		//关闭移动场景，关闭后状态为ip_initial
						GSM_send_cmd("AT+CGCLASS=\"B\"","OK",6);//设置GPRS移动台类别为B,支持包交换和数据交换
						GSM_send_cmd("AT+CGATT=1","OK",6);//附着GPRS业务
						GSM_send_cmd("AT+CIPMUX=1","OK",6);//设置为多路连接
						GSM_send_cmd("AT+CSTT=\"CMNET\"","OK",6);//设置为GPRS连接模式，将ip_initial变为ipstart
						GSM_send_cmd("AT+CIICR","OK",6);// 激活移动场景,ip_start状态下有效
						UART2_SendString("AT+CIFSR\r\n");//查模块IP，这个必须有
					}						
													
		      sendok_flag = 1;
			break;
		case 1:
			    sim808_conn_init();
	        if(conn_fail >= 10)
					{
						Time_filter = 1;
						GSM_send_cmd("AT+CIPCLOSE=1","CLOSE OK",8);	//关闭连接
						GSM_send_cmd("AT+CIPSHUT","SHUT OK",6);		//关闭移动场景，关闭后状态为ip_initial
		        GSM_send_cmd("AT+CGCLASS=\"B\"","OK",8);//设置GPRS移动台类别为B,支持包交换和数据交换
		        GSM_send_cmd("AT+CGATT=1","OK",8);//附着GPRS业务
		        GSM_send_cmd("AT+CIPMUX=1","OK",10);//设置为多路连接
		        GSM_send_cmd("AT+CSTT=\"CMNET\"","OK",10);//设置为GPRS连接模式，将ip_initial变为ipstart
		        GSM_send_cmd("AT+CIICR","OK",10);// 激活移动场景,ip_start状态下有效
		        UART2_SendString("AT+CIFSR\r\n");//查模块IP，这个必须有
					}
          if(GSM_send_cmd("AT+CIPSTART=1,\"TCP\",\"139.224.1.160\",9998","OK",10) == 0)//发起连接
					{
					ip_fail = 0;
          UART1_SendString("正在发起连接\r\n");
					CLR_Buf2();
					Conn_miao=1;
					while(Conn_miao < 6)
					{
					if(strstr((const char*)Uart2_Buf,"1, CONNECT OK"))//连接成功
					{
					UART1_SendString("信号1连接成功\r\n");
					conn_flag = 1;
					conn_fail = 0;
					CLR_Buf2();
					break;	
					}
								
		      else if(strstr((const char*)Uart2_Buf,"1, ALREAY CONNECT"))//连接成功
					{
						UART1_SendString("信号1已经连接\r\n");
						conn_flag = 1;
						conn_fail = 0;
						CLR_Buf2();
						break;
					}
				  else if(strstr((const char*)Uart2_Buf,"1, CONNECT FAIL"))//连接失败
					{
						if(conn_fail == 5){exit_flag=1;}
						UART1_SendString("信号1连接失败\r\n");
						conn_fail ++;
						CLR_Buf2();
						break;	
					}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
				  else if(strstr((const char*)Uart2_Buf,"ERROR"))//连接出错
					{
						if(conn_fail == 5){exit_flag=1;}
						UART1_SendString("信号1连接出错\r\n");
						conn_fail ++;
						CLR_Buf2();
						break;	
					}
				  
					}
					conn_fail ++;
				 }
					else
				  {
							if(strstr((const char*)Uart2_Buf,"1, ALREADY CONNECT"))//连接成功
							{
							GSM_send_cmd("AT+CIPCLOSE=1","CLOSE OK",6);	//关闭连接
							}
						if(ip_fail == 2){exit_flag=1;}
						ip_fail++;
						UART1_SendString("ip1地址无效\r\n");
					}	
 					
		break;
		case 2:
					exit_flag = 1;
					GSM_send_cmd("AT+CIPCLOSE=1","CLOSE OK",8);	//关闭连接
					UART1_SendString("关闭本地socket连接\r\n");
		break;

	}
}
CLR_Buf1();
}


int filter_data_request(char* filter_buf)
{
	char speed_temp[5];
	char press1_temp[5];
	char press2_temp[5];
  char press3_temp[5];
	char lati_degree[2];
	char lati_min[7];
	char long_degree[3];
	char long_min[7];
	char filter_time[15];
	memset(filter_buf,0,sizeof(filter_buf));
	memset(speed_temp,0,sizeof(speed_temp));
	memset(filter_time,0,sizeof(filter_time));
	
	strcat(filter_buf,"00000");
	strcat(filter_buf,",");
	if(strlen(device)  == 13)
	{
	UART1_SendString((char *)device);
	strcat(filter_buf,device);
	}
	else
	{
	printf("failed id: %s\r\n",device);
	STMFLASH_Read(FLASH_SAVE_ID,(u16*)id_len,1);
	STMFLASH_Read(FLASH_SAVE_ID,(u16*)device,id_len[0]);
	if(strlen((const char*)device) > 5)
	{
		 UART1_SendString("读取id :");
     UART1_SendString((char *)device);
	   UART1_SendString("\r\n");
	}
	if(strlen(device) > 8 && strlen(device) < 20)
	strcat(filter_buf,device);
	}
	strcat(filter_buf,",");
	//pa - kp
	if(atoi(press1) >=0 && atoi(press1) <= 100000)
	{
//	sprintf(press1_temp,"%d",atoi(press1)/1000);
 	sprintf(press1_temp,"%.3f",atoi(press1)/1000.0f);
	strcat(filter_buf,press1_temp);
	}
	else 
	strcat(filter_buf,"0");
	strcat(filter_buf,",");
	if(atoi(press2) >=0 && atoi(press2) <= 100000)
	{
	//	sprintf(press2_temp,"%d",atoi(press2)/1000);
	sprintf(press2_temp,"%.3f",atoi(press2)/1000.0f);	
	strcat(filter_buf,press2_temp);
	}
	else 
	strcat(filter_buf,"0");
	strcat(filter_buf,",");
	if(atoi(press3) >= 0 && atoi(press3) <= 100000)
	{
	//	sprintf(press3_temp,"%d",atoi(press3)/1000);
		sprintf(press3_temp,"%.3f",atoi(press3)/1000.0f);
		strcat(filter_buf,press3_temp);
	}
	else 
	strcat(filter_buf,"0");
	strcat(filter_buf,",");
  
	if(atoi(temp1) >= 0 && atoi(temp1) <= 2000)
	strcat(filter_buf,temp1);
	else 
	strcat(filter_buf,"0");
	strcat(filter_buf,",");
	if(atoi(temp2) >= 0 && atoi(temp2) <= 2000)
	strcat(filter_buf,temp2);
	else 
	strcat(filter_buf,"0");
	strcat(filter_buf,",");
	if(atoi(temp3) >= 0 && atoi(temp3) <= 2000)
	strcat(filter_buf,temp3);
	else 
	strcat(filter_buf,"0");
	strcat(filter_buf,",");

  if(strlen((char*)gps.gps_time) == 12)
	{
	  utc_to_bj(filter_time);	
		printf("filter_time = %s",filter_time);		
	}
	
  if(atoi(gps.gps_speed) >= 0  && atoi(gps.gps_speed) < 1000)
	{
	sprintf(speed_temp,"%d",atoi(gps.gps_speed));
	strcat(filter_buf,speed_temp);
	}
	else 
	strcat(filter_buf,"0");
	strcat(filter_buf,",");
  if(atoi(filter_time) > 2016)
	strcat(filter_buf,filter_time);
	strcat(filter_buf,",");
	if(strlen(gps.gps_latitude) == 9)
	{
    memcpy(lati_degree, gps.gps_latitude, 2);
    memcpy(lati_min, gps.gps_latitude + 2, strlen(gps.gps_latitude) - 2);
    sprintf(lati, "%.5f", (atoi(lati_min) / 60.0f) + atoi(lati_degree));
	  strcat(filter_buf,(char*)lati);
	}
  else 
	strcat(filter_buf,"0");
	strcat(filter_buf,",");
	strcat(filter_buf,"N");
	strcat(filter_buf,",");
	if(strlen(gps.gps_longitude) == 10)
	{
    memcpy(long_degree, gps.gps_longitude, 3);
    memcpy(long_min, gps.gps_longitude + 3, strlen(gps.gps_longitude) - 3);
    sprintf(longi, "%.5f", (atoi(long_min) / 60.0f)+ atoi(long_degree));
	  strcat(filter_buf,(char*)longi);
	}
	else 
	strcat(filter_buf,"0");
	
	strcat(filter_buf,",");
	strcat(filter_buf,"E");
	strcat(filter_buf,",");
	strcat(filter_buf,"10000");
	strcat(filter_buf,",");
	strcat(filter_buf,"3000");
	strcat(filter_buf,",");
	strcat(filter_buf,",");
	strcat(filter_buf,",");
	strcat(filter_buf,"*");
	strcat(filter_buf,"00");
	strcat(filter_buf,"\r\n");

  return strlen(filter_buf);
}


void utc_to_bj(char *filter_time)
{

	char UTCYear[2];
	char UTCMonth[2];
	char UTCDay[2];
	char UTCHour[2];
	char UTCMinutes[2];
	char UTCSeconds[2];
  int ChinaYear = 0;
	int ChinaMonth = 0;
	int ChinaDay = 0;
	int ChinaSeconds = 0;
	int ChinaMinutes = 0;
	int ChinaHour = 0;
//memcpy(year,"20",2);
  memcpy(UTCYear,gps.gps_time,2);
	memcpy(UTCMonth,gps.gps_time+2,2);
	memcpy(UTCDay,gps.gps_time+4,2);
	memcpy(UTCHour,gps.gps_time+6,2);
	memcpy(UTCMinutes,gps.gps_time+8,2);
	memcpy(UTCSeconds,gps.gps_time+10,2);
	
     ChinaYear = atoi(UTCYear);  
     ChinaMonth = atoi(UTCMonth);  
     ChinaDay = atoi(UTCDay);  
     ChinaSeconds = atoi(UTCSeconds);  
     ChinaMinutes = atoi(UTCMinutes);  
     ChinaHour = atoi(UTCHour)+8;  
     if (ChinaHour>23)  
        {  
        ChinaHour-=24;  
        ChinaDay++;  
        switch (ChinaMonth)  
               {  
               case 1:  
                    if (ChinaDay>31)  
                       {  
                       ChinaDay=1;  
                       ChinaMonth++;  
                       }  
                    break;  
               case 2:  
                    if ((0==ChinaYear%4 && 0!=ChinaYear%100) || 0==ChinaYear%400)  
                       {  
                       if (ChinaDay>29)  
                          {  
                          ChinaDay=1;  
                          ChinaMonth++;  
                          }  
                       }  
                    else  
                       {  
                       if (ChinaDay>28)  
                          {  
                          ChinaDay=1;  
                          ChinaMonth++;  
                          }  
                       }  
                   break;  
               case 3:  
                    if (ChinaDay>31)  
                       {  
                       ChinaDay=1;  
                       ChinaMonth++;  
                       }  
                   break;  
               case 4:  
                    if (ChinaDay>30)  
                       {  
                       ChinaDay=1;  
                       ChinaMonth++;  
                       }  
                    break;  
               case 5:  
                    if (ChinaDay>31)  
                       {  
                       ChinaDay=1;  
                       ChinaMonth++;  
                       }  
                   break;  
               case 6:  
                    if (ChinaDay>30)  
                       {  
                       ChinaDay=1;  
                       ChinaMonth++;  
                       }  
                   break;  
               case 7:  
                    if (ChinaDay>31)  
                       {  
                       ChinaDay=1;  
                       ChinaMonth++;  
                       }  
                   break;  
               case 8:  
                    if (ChinaDay>31)  
                       {  
                       ChinaDay=1;  
                       ChinaMonth++;  
                       }  
                    break;  
               case 9:  
                    if (ChinaDay>30)  
                       {  
                       ChinaDay=1;  
                       ChinaMonth++;  
                       }  
                   break;  
               case 10:  
                    if (ChinaDay>31)  
                       {  
                       ChinaDay=1;  
                       ChinaMonth++;  
                       }  
                   break;  
               case 11:  
                    if (ChinaDay>30)  
                       {  
                       ChinaDay=1;  
                       ChinaMonth++;  
                       }  
                   break;  
               case 12:  
                    if (ChinaDay>31)  
                       {  
                       ChinaDay=1;  
                       ChinaMonth=1;  
                       ChinaYear++;  
                       }  
                    break;  
               default:break;  
               }  
        }  
              sprintf(filter_time,"%02d",ChinaYear);
							sprintf(filter_time+2,"%02d",ChinaMonth);
		          sprintf(filter_time+4,"%02d",ChinaDay);
		          sprintf(filter_time+6,"%02d",ChinaHour);
		          sprintf(filter_time+8,"%02d",ChinaMinutes);
		          sprintf(filter_time+10,"%02d",ChinaSeconds); 				
	 
}
