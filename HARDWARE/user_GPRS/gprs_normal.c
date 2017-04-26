/**********************************************************************************
 * �ļ���  ��gprs_normal.c
 * ����    �����Ѽ�س���Э�鲿�֣���Ϊ���Ƚϴ����Ե�������       
 * ʵ��ƽ̨��Sim808
 * Ӳ�����ӣ� TXD(PA2)  -> �ⲿ����RXD     
 *           RXD(PA3) -> �ⲿ����TXD      
 *           GND	   -> �ⲿ����GND 
 * ���������˵���
**********************************************************************************/

#include "gprs_jiankong.h"
#include "string.h"
#include "usart.h"	  
#include "user_config.h"
#include "gsm.h"
#include "SysTick.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "flash.h"
#include "gprs_normal.h"
#include "radix.h"
#include "math.h"
#include "max6675.h" 
#include "pressure.h"


 u8 Uart3_temp[100]; //���������ݻ���
 u8 data_sensor[50] ;
 char *string = "AT+CIPSTART=\"TCP\",\"183.14.152.95\",33314";

 u16 heart_time= 75;//����ʱ��
 char device_id[50];
 char phone[50];
 char fac[50];
 char check_string[60];
 char ftemp[20];
 char btemp[20];
 char fpress[20];
 char bpress[20];
 char midtemp[20];
 char midpress[20];
 
 
 
 char press[10];
 char phone_len[2];
 char fac_len[2];
 char speed[10];
	
 char status_car = 0;
 char status_device = 0;
 char netstatus = 0;
 
 char he_longi[20];
 char he_lati[20];
 char he_time[30];
 char he_speed[10];
 char heftemp[20];
 char hebtemp[20];
 char hefpress[20];
 char hebpress[20];
 
 //�����ź�
 char  hand[100];
 char *hand_first = "4141500000000000ABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABAB";
 char *hand_reverse = "B1";
 char *hand_check = "AN";
 char *hand_crc = "ANAN";
 char *hand_last = "C2C2C2C2";
 
 //������
 char heart[250];
 char *heart_first  = "4141540000000000ABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABAB";
 char *heart_reverse = "B1";
 char *heart_check = "AN";
 char *heart_crc = "ANAN";
 char *heart_last = "C2C2C2C2";
 
 
 //���ݰ�
 char data[Buf2_Max];
 char *data_first = "414157";
 char data_length[2];
 char *expand_fill = "0000";//����չ����ǰ��λ���
 char *fill_data = "ABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABABAB";
 char *data_reverse = "B1";
 char *data_check = "AN";
 char *data_crc = "ANAN";
 char *data_last = "C2C2C2C2";
 char Device_id[50] ;

 
 /*******************************************************************************
* ������ : gprs_chejia_mode
* ����   : GPRS������ģʽ
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
u8 gprs_chejia_mode()
{
	//���flash���Ƿ���ip ��Ϣ��û������ʾ���빤��ģʽ������У������ӷ��������ȷ��������ź� ��ʱ�������������ȴ�����������ָ����ʽ���ݺ��ٽ�GPS�ȴ��͵����������������ش�ok�����ͳɹ�?
	//u32 s = 7200000;
	u8 mode; 
	u8 conn_flag = 0;
	u8 heart_fail = 0;
	u8 ip_fail = 0;
	u8 ip_fail_flag = 0;
	u8 hand_flag = 0;
	u8 henan_flag = 0;
	u8 cjip[60];
	u8 cj_len[2];
	u8 id_len[2];
	
	
	memset(cjip,0,sizeof(cjip));
	memset(device_id,0,sizeof(device_id));
	
	//��FLASH,�鿴����ȡip ,�˿�ֵ
	
	
	STMFLASH_Read(FLASH_SAVE_ID,(u16*)id_len,1);
	STMFLASH_Read(FLASH_SAVE_ID,(u16*)device_id,id_len[0]);

	STMFLASH_Read(FLASH_SAVE_CJET,(u16*)cj_len,1);
 	STMFLASH_Read(FLASH_SAVE_CJET,(u16*)cjip,cj_len[0]);

	if(strlen((const char*)device_id) > 5)
	{
		 UART1_SendString("��ȡid :");
     UART1_SendString((char *)device_id);
		 UART1_SendString("\r\n");
  }
  if(strlen((const char*)cjip) >10)
	{
      UART1_SendString("��ȡ��ip :");
		  UART1_SendString((char*)cjip);
		  UART1_SendString("\r\n");
  }
	
	
	while(1)
	{
		if(conn_flag == 0){mode = 1;}
		else if(hand_flag == 1) {hand_flag = 0; mode = 5;} //һ���Ӿͷ������ź�
		else if(ip_fail == 1) {UART1_SendString("�뵽����ģʽ��������ip\r\n");break;}	
		else if(heart_fail == 2){conn_flag = 0;mode = 1;}
		else if(((strstr((const char*)Uart2_Buf,"CLOSED"))&&(Time_miao>10)))//����ʧ�ܻ�ʱ
		{
			UART1_SendString("����ʧ��\r\n");		
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); 
		  USART_SendData(USART1 ,Time_miao);//���͵�ǰ�ַ�			
			Time_miao=0;
			mode = 1;
			CLR_Buf2();
		}	
		
		else if(henan_flag == 1){henan_flag = 0; mode = 6;}//�л�����һ��ģʽ
		else if((conn_flag==1)&&(Time_miao > heart_time))//ÿn��һ��������
		{
			Time_miao=1;
			mode=3;
		}
		else mode = 0;
    	
	  switch(mode)
		{
			
			
		  case 0: 
	         if(strstr((const char*)Uart2_Buf,"+RECEIVE,0,"))//�жϳ��ѷ������Ƿ��������·�
		       {
			       UART1_SendString("���յ����ѷ���������\r\n");
				     if(strstr((const char*)Uart2_Buf,"4141560"))
				   { chejia_protocol();henan_flag = 1;}
				   else if(strstr((const char*) Uart2_Buf,"OK"))
				     UART1_SendString("�����ϴ��ɹ�\r\n");
			     else UART1_SendString("������Ϣ��ƥ��\r\n");
					
						CLR_Buf2();
					 }	
			break;
					
					
			case 1: 
						GSM_send_cmd("AT+CIPCLOSE=0","CLOSE OK",2);	//�ر�����
						GSM_send_cmd("AT+CIPSHUT","SHUT OK",2);		//�ر��ƶ�����
						GSM_send_cmd("AT+CGCLASS=\"B\"","OK",2);//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ���
						GSM_send_cmd("AT+CGATT=1","OK",2);//����GPRSҵ��
						GSM_send_cmd("AT+CIPMUX=1","OK",2);//����Ϊ��·����
						GSM_send_cmd("AT+CSTT=\"CMNET\"","OK",2);//����ΪGPRS����ģʽ
						Second_AT_Command("AT+CIICR","OK",4);//����������·					
						UART2_SendString("AT+CIFSR\r\n");//��ģ��IP�����������
            if(GSM_send_cmd((u8 *)cjip,"OK",10) == 0)//��������
						{
               UART1_SendString("���ڷ�������\r\n");
							 Time_miao=1;
						
						}
						else
						{
							 if(ip_fail_flag == 3){ip_fail_flag = 0; ip_fail = 1;}
							 ip_fail_flag++;
							 UART1_SendString("ip��ַ��Ч\r\n");
							 conn_flag = 0;
							 Time_miao=0;
						}
							 Delay_nMs(3000);
						if(strstr((const char*)Uart2_Buf,"0, CONNECT OK"))//���ӳɹ�
						{
							 UART1_SendString("�ź�0���ӳɹ�\r\n");
							 hand_flag = 1;
							 conn_flag = 1;
							
						}
								
						else if(strstr((const char*)Uart2_Buf,"0, ALREAY CONNECT"))//���ӳɹ�
						{
							 UART1_SendString("�ź�0�Ѿ�����\r\n");
							 conn_flag = 1;
							 CLR_Buf2();
							 break;
						}
						else if(strstr((const char*)Uart2_Buf,"0, CONNECT FAIL"))//����ʧ��
						{
							 UART1_SendString("�ź�0����ʧ��\r\n");
							 conn_flag = 0;
						}
						else if(strstr((const char*)Uart2_Buf,"ERROR"))//���ӳ���
						{
							 UART1_SendString("�ź�0���ӳ���\r\n");
							 conn_flag = 0;
						}
						else{}
						CLR_Buf2();
			break;
			case 2:
            if(conn_flag==1)
					  {
              UART1_SendString("��ʼ����...........\r\n");
						 if(GSM_send_cmd("AT+CIPSEND=0",">",8)==0)
							{
								if(GSM_send_cmd((u8 *)string,"SEND OK",8)==0)
								{ 								
								  UART1_SendString("���ͳɹ�\r\n");		
								  Time_miao=1;
							  }
								else
									UART1_SendString("����ʧ��\r\n");
										 
								}
								else
								{
								  UART2_Data(0X1B);//ESC,ȡ������
                  UART1_SendString("����ʧ��\r\n");
								}
						  }else UART1_SendString("��û����\r\n");
							 mode=0;
			         break;
							
							
			case 3:
				  if(conn_flag==1)
					{
						if(GSM_send_cmd("AT+CIPSEND=0",">",8)==0)
						{ 
							memset((char*)heart,NULL,sizeof(heart));
							strcat(heart,heart_first);
				      strcat(heart,device_id);
 						  strcat(heart,heart_reverse);
              strcat(heart,heart_check);
              strcat(heart,heart_crc);
              strcat(heart,heart_last);								
							UART2_SendString(heart);
							UART2_Data(0x00);
							UART2_Data(0X1A);//CTRL+Z,�������ݷ���,����һ�δ���								
							UART1_SendString("�����ɹ�\r\n");
							memset(heart,0,sizeof(heart));
						  CLR_Buf2();
						}
						else
						{   
							heart_fail ++;
							UART2_Data(0X1B);//ESC,ȡ������
				      UART1_SendString("����ʧ��\r\n");
						}
						mode = 0;
					}
					else   mode=1;
					
			break;
			case 4:
						 GSM_send_cmd("AT+CIPCLOSE=0","CLOSE OK",5);	//�ر�����
						 GSM_send_cmd("AT+CIPSHUT","SHUT OK",5);		//�ر��ƶ�����
			       conn_flag = 0;
						 UART1_SendString("�ر�socket����\r\n");
						 return 1;
			
			case 5:
					  UART1_SendString("���ӳɹ�\r\n");
					  heart_fail = 0;
				    mode = 3;
					  Time_miao=1;
					  CLR_Buf2();
				    memset(hand,NULL,sizeof(hand));//�������
  				  strcat(hand,hand_first);
				    strcat(hand,device_id);
 						strcat(hand,hand_reverse);
				    strcat(hand,hand_check);
				    strcat(hand,hand_crc);
				    strcat(hand,hand_last);	
			      if(GSM_send_cmd("AT+CIPSEND=0",">",8)==0)
					 {
									
					  UART2_SendString(hand); //�����������ݰ�
					  UART2_Data(0x00);
					  UART2_Data(0X1A);//CTRL+Z,�������ݷ���,����һ�δ���																 
				   }  
				    CLR_Buf2();
				    memset(hand,0,sizeof(hand));//���hand
				    Delay_nMs(3100);
			      mode = 0;
			      break;

      case 6:
				    henan_protocol();
			      mode = 0;
						break;
				
		   } 
  }
	    return 1;
}




u8 henan_protocol(void)
{
	
	
	double code = 508;
	int check_int;
	int i,j;
	int hmode;                                                                                        
	char phonecode[50];
	
	char ip_fail = 1;//ip��ַ����
	char conn_fail = 1;//����ʧ��
	char sendok_flag = 0;//���������յ�����
	char error_flag = 0;//�����κδ���
	char exit_flag = 0;
	char conn_flag = 0;//���ӳɹ���־
	
	char heip_len[2];
	char heip[60];

	
	char paper[256];
	char *p;
	p = paper;
	
	
  UART1_SendString("@@@@@@@@@@@@@@���ѽ������ģʽ@@@@@@@@@@@@@@@@@@@@@\r\n");
  Delay_nMs(3100);
	
  STMFLASH_Read(FLASH_SAVE_HADDR,(u16*)heip_len,1);
	STMFLASH_Read(FLASH_SAVE_HADDR,(u16*)heip,heip_len[0]);	
	if(strlen((const char*)heip) > 12)
	{
		 UART1_SendString("��ȡip :");
     UART1_SendString((char *)heip);
		 UART1_SendString("\r\n");
  }

	

	while(1)
	{
	if(ip_fail != 0 || conn_fail == 1){ hmode = 1;}
	else if(conn_flag == 1){conn_flag = 0;hmode = 0;}
	else if(error_flag == 1 || sendok_flag == 1){hmode = 2;}
  else if(exit_flag == 1)break;
	else if(strstr((const char*)Uart2_Buf,"+RECEIVE,1,"))//�жϽ����������Ƿ��������·�
	{
		UART1_SendString("@@@@@���յ���������������@@@@\r\n");
		
	  if(strstr((const char*)Uart2_Buf,"0@@"))
		{
			UART1_SendString("�����������յ�����\r\n");
			sendok_flag = 1;
			
		}else  henan_protocol();
	}     
	else hmode = 3;

	switch(hmode)
  {
		case 0:
			    STMFLASH_Read(FLASH_SAVE_PHONE,(u16*)phone_len,1);
  	      STMFLASH_Read(FLASH_SAVE_PHONE,(u16*)phone,phone_len[0]);
		
					if(strlen((const char*)phone) >10)
					{
            UART1_SendString("��ȡ���绰 :");
		        UART1_SendString((char*)phone);
		        UART1_SendString("\r\n");
					}
					
					STMFLASH_Read(FLASH_SAVE_FACTORY,(u16*)fac_len,1);
					STMFLASH_Read(FLASH_SAVE_FACTORY,(u16*)fac,fac_len[0]);
					if(strlen((const char*)fac) >2)
					{
						UART1_SendString("��ȡ�����ұ��� :");
						UART1_SendString((char*)fac);
						UART1_SendString("\r\n");
					}
					Delay_nMs(3000);
					for(i = 3,j = 0;i<11;i++,j++)
				  phonecode[j] = phone[i];
					phonecode[j] = '\0';
	
					UART1_SendString(phonecode);
					UART1_SendString("\r\n");
					check_int = abs(cos(code + atoi(phonecode)) *10000);
					itoa(check_int,check_string,10);
					UART1_SendString(check_string);
					UART1_SendString("\r\n");
					p=makeprotocol();
					
					
				  if(GSM_send_cmd("AT+CIPSEND=1",">",5)==0){
									
					UART2_SendString((char*)p);					  
					UART2_Data(0x00);
					UART2_Data(0X1A);//CTRL+Z,�������ݷ���,����һ�δ���	
									
					//���������Ϣ
						
					CLR_Buf2();
					CLR_Buf3();	
				
								
				}
// 		      if(flag == 10){error_flag = 1;}
//           flag++;
					
		      break;
		case 1:
 					  GSM_send_cmd("AT+CIPCLOSE=1","CLOSE OK",2);	//�ر�����
// 					GSM_send_cmd("AT+CGCLASS=\"B\"","OK",2);//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ���
// 					GSM_send_cmd("AT+CGATT=1","OK",2);//����GPRSҵ��
// 					GSM_send_cmd("AT+CIPMUX=1","OK",2);//����Ϊ��·����
// 					GSM_send_cmd("AT+CSTT=\"CMNET\"","OK",2);//����ΪGPRS����ģʽ
// 					Second_AT_Command("AT+CIICR","OK",4);//����������·					
// 					UART2_SendString("AT+CIFSR\r\n");//��ģ��IP�����������
          if(GSM_send_cmd((u8 *)heip,"OK",10) == 0)//��������
					{
						ip_fail = 0;
            UART1_SendString("���ڷ�������\r\n");
						CLR_Buf2();
						Delay_nMs(3000);
					if(strstr((const char*)Uart2_Buf,"1, CONNECT OK"))//���ӳɹ�
					{
					  UART1_SendString("�ź�1���ӳɹ�\r\n");
						conn_flag = 1;
						conn_fail = 0;
						
					}
								
		      else if(strstr((const char*)Uart2_Buf,"1, ALREAY CONNECT"))//���ӳɹ�
					{
						UART1_SendString("�ź�1�Ѿ�����\r\n");
						conn_flag = 1;
						conn_fail = 0;
						break;
					}
				  else if(strstr((const char*)Uart2_Buf,"1, CONNECT FAIL"))//����ʧ��
					{
						if(conn_fail == 5){exit_flag=1;}
						UART1_SendString("�ź�1����ʧ��\r\n");
						conn_fail ++;
					}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
				  else if(strstr((const char*)Uart2_Buf,"ERROR"))//���ӳ���
					{
						if(conn_fail == 5){exit_flag=1;}
						UART1_SendString("�ź�1���ӳ���\r\n");
						conn_fail ++;
					}
				  else ;
					}
					else
				  {
						if(ip_fail == 5){exit_flag=1;}
						ip_fail++;
						UART1_SendString("ip��ַ��Ч\r\n");
					}						
						
				
			   break;
		case 2:
					exit_flag = 1;
					GSM_send_cmd("AT+CIPCLOSE=1","CLOSE OK",5);	//�ر�����
					//GSM_send_cmd("AT+CIPSHUT","SHUT OK",5);		//�ر��ƶ�����
					UART1_SendString("�ر�socket����\r\n");
		      return 1;
			   // break;
		case 3:
			    break;
			
	}
}
	return 1;	
}





u8 chejia_protocol(void)
{
	char expand_str[5];
	char slength[2];
	char expand_length[4];
	char hex_ftemp[10];
	char hex_btemp[10];
	char hex_fpress[10];
	char hex_bpress[10];
	char hex_midpress[10];
	char hex_midtemp[10];
	
	char hex4_ftemp[10];
	char hex4_btemp[10];
	char hex4_fpress[10];
	char hex4_bpress[10];
	char hex4_midpress[10];
	char hex4_midtemp[10];
	itoa((int)(read_temp2()),btemp,10);//��ȡ�豸�¶�
	memcpy(hebtemp,btemp,strlen(btemp));
	itoa(atoi(btemp),hex_btemp,16);
	UART1_SendString("����ֵ");
	UART1_SendString(btemp);
  UART1_SendString("\r\n");	
	
  itoa(get_pressure_1(),fpress,10);     //��ȡǰ��ѹ��ֵ
	memcpy(hefpress,fpress,strlen(fpress));
	itoa(atoi(fpress),hex_fpress,16);	
	UART1_SendString("ǰѹֵ");
	UART1_SendString(fpress);
	UART1_SendString("\r\n");
					
					
	itoa(get_pressure_2(),bpress,10);     //��ȡǰ��ѹ��ֵ
	memcpy(hebpress,bpress,strlen(bpress));
	itoa(atoi(bpress),hex_bpress,16);;
	UART1_SendString("��ѹֵ");
	UART1_SendString(bpress);
	UART1_SendString("\r\n");						 
	
	strcat((char*)data_sensor,"4141070C");
	
	if(atoi(fpress) > 100)
	{
		fix_zero(hex4_fpress,hex_fpress);
		strcat((char*)data_sensor,hex4_fpress);
		
	}
		
	else strcat((char*)data_sensor,"0000");
	
	
	if(atoi(bpress) > 100)
	{
		fix_zero(hex4_bpress,hex_bpress);
		strcat((char*)data_sensor,hex4_bpress);
		
	}
		
	else strcat((char*)data_sensor,"0000");
	
	
	if(atoi(midpress) > 100)
	{
		fix_zero(hex4_midpress,hex_midpress);
		strcat((char*)data_sensor,hex4_midpress);
	}

	else strcat((char*)data_sensor,"0000");
	
	
	if(atoi(ftemp) > 10)
	{
		fix_zero(hex4_ftemp,hex_ftemp);
		strcat((char*)data_sensor,hex4_ftemp);
	}
		
	else strcat((char*)data_sensor,"0000");
	
	
	if(atoi(btemp) > 10)
	{
		fix_zero(hex4_btemp,hex_btemp);
		strcat((char*)data_sensor,hex4_btemp);
	}
		
	else strcat((char*)data_sensor,"0000");
	
	
	if(atoi(midtemp) > 10)
	{
		fix_zero(hex4_midtemp,hex_midtemp);
		strcat((char*)data_sensor,hex4_midtemp);
	
	}
	else strcat((char*)data_sensor,"0000");
  
	strcat((char*)data_sensor,"FF");
	
	UART1_SendString("���������ݻ�ȡ�ɹ�\r\n");

	itoa(strlen((char *)data_sensor),slength,16);
	
	  get_GPS_data();
							
	  expand2hex(strlen(( char*)gps_data),expand_length);	
    fix_zero(expand_str,expand_length);
	
		strcat(data,data_first);
		strcat(data,slength);
  	strcat(data,expand_fill);
		strcat(data,expand_str);
	  strcat(data,(char*)data_sensor);
		strcat(data,fill_data);
	  strcat(data,device_id);
 		strcat(data,data_reverse);
		strcat(data,data_check);
 		strcat(data,data_crc);
		strcat(data,data_last);
		strcat(data,"\r\n");
		strcat(data,(char*)gps_data);
		CLR_Buf2();
		if(GSM_send_cmd("AT+CIPSEND=0",">",5)==0){
									
		UART2_SendString((char*)data);					  
	  UART2_Data(0x00);
		UART2_Data(0X1A);//CTRL+Z,�������ݷ���,����һ�δ���	
									
		//���������Ϣ
		CLR_Buf2();
		CLR_Buf3();	
		memset(data_length,0,sizeof(data_length));//���������
		memset(expand_length,0,sizeof(expand_length));
		memset(expand_str,0,sizeof(expand_str));
		memset(data_sensor,0,sizeof(data_sensor));
		memset(data,0,sizeof(data));
		memset(data_length,0,sizeof(data_length));
		memset(gps_data,0,sizeof(gps_data));
								
	}
	  return 1;
								
}


char* makeprotocol(void)
{
	
	char paper_temp[256];
	char *p;
	memset(paper_temp,0,sizeof(paper_temp));
	UART1_SendString(hebtemp);
	UART1_SendString(hefpress);
	UART1_SendString(hebpress);
	p = paper_temp;;
	if(strlen(check_string) > 3)
	strcat(paper_temp,check_string);
	strcat(paper_temp,"@");
	if(atoi(heftemp) > 0)
		strcat(paper_temp,heftemp);
	strcat(paper_temp,"@");
	if(atoi(hebtemp) > 0)
		strcat(paper_temp,hebtemp);
	strcat(paper_temp,"@");
	if(atoi(hefpress) > 0)
	  strcat(paper_temp,hefpress);
	strcat(paper_temp,"@");
	if(atoi(hebpress) > 0)
		strcat(paper_temp,hebpress);
	strcat(paper_temp,"@");
	itoa(((int)(atoi(hebpress)-atoi(hefpress))),press,10);
	strcat(paper_temp,(const char*)press);
	strcat(paper_temp,"@");
	if(atoi((const char*)he_longi) > 0)
		strcat(paper_temp,(const char*)he_longi);
	strcat(paper_temp,"@");
	if(atoi((const char*)he_lati) > 0)
		strcat(paper_temp,(const char*)he_lati);
	strcat(paper_temp,"@");
	if(atoi((const char*)he_speed) >= 0)
		strcat(paper_temp,(const char*)he_speed);
	strcat(paper_temp,"@");
	
	if(strlen(device_id) > 0)
	strcat(paper_temp,device_id);
	strcat(paper_temp,"@");
	
	if(strlen(fac) > 0)
	strcat(paper_temp,fac);
	strcat(paper_temp,"@");
	if(strlen(phone) > 0)
	strcat(paper_temp,phone);
	strcat(paper_temp,"@");
	
	if(status_car == 0)
	strcat(paper_temp,"0");
	if(status_car == 1)
	strcat(paper_temp,"1");
	
	strcat(paper_temp,"@");
	
	if(status_car == 0)
	strcat(paper_temp,"0");
	if(status_car == 1)
	strcat(paper_temp,"1");
	
	strcat(paper_temp,"@");
	
	
	strcat(paper_temp,"1");
	strcat(paper_temp,"@");
	strcat(paper_temp,(const char*)he_time);
	
	
  //�����������
	memset(heftemp,0,sizeof(heftemp));
	memset(hebtemp,0,sizeof(hebtemp));
	memset(hefpress,0,sizeof(hefpress));
	memset(hebpress,0,sizeof(hebpress));
	memset(he_speed,0,sizeof(he_speed));
	memset(he_time,0,sizeof(he_time));
  memset(he_lati,0,sizeof(he_lati));
  memset(he_longi,0,sizeof(he_longi));
	
	return p;
	
	
}


char* itoa(int num,char*str,int radix)
{/*������*/
char index[]="0123456789ABCDEF";
unsigned unum;/*�м����*/
int i=0,j,k;
char temp;
/*ȷ��unum��ֵ*/
if(radix==10&&num<0)/*ʮ���Ƹ���*/
{
unum=(unsigned)-num;
str[i++]='-';
}
else unum=(unsigned)num;/*�������*/
/*ת��*/
do{
str[i++]=index[unum%(unsigned)radix];
unum/=radix;
}while(unum);
str[i]='\0';
/*����*/
if(str[0]=='-')k=1;/*ʮ���Ƹ���*/
else k=0;

for(j=k;j<=(i-1)/2;j++)
{
temp=str[j];
str[j]=str[i-1+k-j];
str[i-1+k-j]=temp;
}
return str;
}


char* fix_zero(char *expand_str,char *expand_length)
{
	  switch((u8)strlen(expand_length))
	{
		case 0: {expand_str[0] = '0';expand_str[1] = '0';expand_str[2] = '0';expand_str[3] = '0';expand_str[4] = '\0';break;}							 
    case 1: {expand_str[0] = '0';expand_str[1] = '0';expand_str[2] = '0';expand_str[3] = expand_length[0];expand_str[4] = '\0';break;}
		case 2: {expand_str[0] = '0';expand_str[1] = '0';expand_str[2] = expand_length[0];expand_str[3] = expand_length[1];expand_str[4] = '\0';break;}						
	  case 3: {expand_str[0] = '0';expand_str[1] = expand_length[0];expand_str[2] = expand_length[1];expand_str[3] = expand_length[2];expand_str[4] = '\0';break;}
		case 4: {expand_str[0] = expand_length[0];expand_str[1] = expand_length[1];expand_str[2] = expand_length[2];expand_str[3] = expand_length[3];expand_str[4] = '\0';break;}								
		default:{ break;}					
  }
	return expand_str;
}
