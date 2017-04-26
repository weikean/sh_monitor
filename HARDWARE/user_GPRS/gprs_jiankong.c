/**********************************************************************************
 * �ļ���  ��gprs_jiankong.c
 * ����    ����س��򲿷�ʵ�ַ���������ģ����Ϣ��GPS�����Լ�����ģʽ         
 * ʵ��ƽ̨��Sim808
 * Ӳ�����ӣ� TXD(PA2)  -> �ⲿ����RXD     
 *           RXD(PA3) -> �ⲿ����TXD      
 *           GND	   -> �ⲿ����GND 
 * ���������˵���
**********************************************************************************/

#include "gprs_jiankong.h"

int idcheck(char * p, int num);
gps_info gps;
u8 gps_flag;
char my_clock[13];
u8 id_len[2];


static uint32_t le32_bytes_to_uint32(uint8_t *p)
{
	uint32_t rv;
	rv = (uint32_t)p[0] << 24  |
		 (uint32_t)p[1] << 16  |
		 (uint32_t)p[2] << 8 |
		 (uint32_t)p[3] ;
	return rv;
}

/*******************************************************************************
* ������ : GSM_gprs_test
* ����   : GPRS����ģʽ

***************************************** **************************************/
u8 GSM_gprs_test() {
    //Ŀ�꣺����޸�ip,�˿�,�޸��豸id���˳�����ģʽ�Ĺ���
    u8 mode = 0;
    u8 exit = 0;
    u8 i,j;
    u8 end = 0,iflag = 10; //��ȡip,portλ�õı���
    char data_he[40];
    char ip_data_he[40];
    char data_len_he[2];
    char chejia[40];
    char chejia_len[2];
	  u8 ip_data[50];
	  u16 len = 0;
		u8 ip_length; //ip port����ֵ
		char device_num[20];
		char phone_num[20];
		char ip[50];
		char port[5];
	  char vid_info[25];
	  char vid_s[9];
	  uint8_t vid_hex_s[8];
		bind_vid b_vid;
		u8 ret;
	  u8 err;
    UART1_SendString("@@@@@@@@@@@@@@���ѽ��빤��ģʽ@@@@@@@@@@@@@@@@@@@@@\r\n");
    UART1_SendString("����0,\"TCP\",\"125.89.18.79\",12345�س�\r\n");
    UART1_SendString("ƽ̨,\"TCP\",\"125.89.18.79\",12345�س�\r\n");
    UART1_SendString("�豸��239828378927�س�\r\n");
    UART1_SendString("�ֻ���18576422291�س�\r\n");
    UART1_SendString("���ұ���+cj01+�س�\r\n");
    UART1_SendString("�˳�����ģʽ���˳�+�س�\r\n");

    while (exit == 0) {
        if (USART_RX_STA & 0x8000) {
            len = USART_RX_STA & 0X3FFF;
            if (len > Buf2_Max - 2) len = Buf2_Max - 2;
            USART_RX_STA = 0;
            if (strstr((const char * ) Uart1_Buf, "�˳�")) mode = 3; //�˳�����ģʽ
            else if (strstr((const char * ) Uart1_Buf, "����")) mode = 1; //����ip�Ͷ˿�
            else if (strstr((const char * ) Uart1_Buf, "�豸��")) mode = 2; //�޸��豸id
            else if (strstr((const char * ) Uart1_Buf, "�ֻ���")) mode = 4; //�޸��ֻ���
            else if (strstr((const char * ) Uart1_Buf, "���ұ���")) mode = 5; //�޸ĳ��ұ���
            else if (strstr((const char * ) Uart1_Buf, "����")) mode = 6; //����ip�Ͷ˿�
					  else if (strstr((const char * ) Uart1_Buf, "�Ϻ�")) mode = 7; //����ip�Ͷ˿�
            else mode = 0;
        }
        switch (mode) {
        case 0:
            break;
        case 1:
            if (strstr((const char * ) Uart1_Buf, "0,\"TCP\"") || strstr((const char * ) Uart1_Buf, "UDP")) {
                strcpy((char * ) & ip_data, (const char * )"AT+CIPSTART=");
                memcpy((char * ) & ip_data[12], (const char * )( & Uart1_Buf[4]), len - 4);
                ip_data[len + 12] = '\0';
                ip_length = strlen((char * ) ip_data);
                for (i = 0; i < len + 12; i++) {
                    if (ip_data[i] == ',')
                    {
                        iflag++;
                    }
                    if (iflag == 13) {
                        end = i;
                        iflag = 10;
                    }
                }
                for (j = 0, i = 21; i < end - 1; i++, j++) {
                    ip[j] = ip_data[i];
                }
                ip[j + 1] = '\0';

                for (j = 0, i = end + 1; i < len + 12; i++, j++) {
                    port[j] = ip_data[i];
                }
                port[j + 1] = '\0';
                UART1_SendString((char * ) ip_data);
                STMFLASH_Write(FLASH_SAVE_CJET, (u16 * ) ip_data, strlen((char * ) ip_data));
                //	Test_Write(FLASH_SAVE_LEN,(u16)ip_length);
                //��ipд��Flash
                STMFLASH_Read(FLASH_SAVE_CJET, (u16 * ) chejia_len, 1);
                STMFLASH_Read(FLASH_SAVE_CJET, (u16 * ) chejia, chejia_len[0]);
                UART1_SendString("\r\nflash�������ģ�");
                UART1_SendString((char * ) chejia);
                UART1_SendString("\nip��ַ: ");
                UART1_SendString(ip);
                UART1_SendString("\r\n");
                UART1_SendString("\n�˿�: ");
                UART1_SendString(port);
                UART1_SendString("\r\n");
                CLR_Buf1();
                memset(ip_data, 0, strlen((char * ) ip_data));
                memset(chejia_len, 0, strlen(chejia_len));
                memset(chejia, 0, strlen(chejia));
            } else UART1_SendString("����ip��ʽ����\r\n");
            memset(ip_data, 0, sizeof(ip_data));
            mode = 0;
            break;

        case 2:
            memcpy((char * )( & device_num[0]), (const char * )( & Uart1_Buf[6]), len - 6); //�洢�豸��
            device_num[len - 6] = '\0';
            //���豸�ŵ�У��
            STMFLASH_Write(FLASH_SAVE_ID, (u16 * ) device_num, len - 5);
            UART1_SendString("�豸���ѱ���\r\n");
            memset(Uart1_Buf, 0, sizeof(Uart1_Buf));
            mode = 0;
            break;

        case 3:
            exit = 1;
            break;

        case 4:
            memcpy((char * )( & phone_num[0]), (const char * )( & Uart1_Buf[6]), len - 6); //�洢�ֻ���
            phone_num[len - 6] = '\0';
            //���ֻ��ŵ�У��
            if (idcheck(phone_num, 11)) {
                STMFLASH_Write(FLASH_SAVE_PHONE, (u16 * ) phone_num, len - 5);
                UART1_SendString("�ֻ����ѱ���\r\n");
                memset(Uart1_Buf, 0, sizeof(Uart1_Buf));
            } else {
                UART1_SendString("�ֻ�����������,����������\r\n");
                memset(Uart1_Buf, 0, sizeof(Uart1_Buf));
            }
            mode = 0;
            break;

        case 5:
            break;
        case 6:
            if (strstr((const char * ) Uart1_Buf, "1,\"TCP\"") || strstr((const char * ) Uart1_Buf, "UDP")) {
                strcpy((char * ) & data_he, (const char * )"AT+CIPSTART=");
                memcpy((char * ) & ip_data_he[12], (const char * )( & Uart1_Buf[4]), len - 4);
                ip_data_he[len + 12] = '\0';
                ip_length = strlen((char * ) ip_data_he);
                for (i = 0; i < len + 12; i++) {
                    if (ip_data_he[i] == ',')

                    {
                        iflag++;
                    }

                    if (iflag == 13) {
                        end = i;
                        iflag = 10;
                    }

                }

                for (j = 0, i = 21; i < end - 1; i++, j++) {
                    ip[j] = ip_data_he[i];
                }
                ip[j + 1] = '\0';

                for (j = 0, i = end + 1; i < len + 12; i++, j++) {
                    port[j] = ip_data_he[i];
                }
                port[j + 1] = '\0';
                UART1_SendString((char * ) ip_data_he);
                STMFLASH_Write(FLASH_SAVE_HADDR, (u16 * ) ip_data_he, ip_length);
                //	Test_Write(FLASH_SAVE_LEN,(u16)ip_length);
                //��ipд��Flash
                STMFLASH_Read(FLASH_SAVE_HADDR, (u16 * ) data_len_he, 1);
                STMFLASH_Read(FLASH_SAVE_HADDR, (u16 * ) data_he, data_len_he[0]);
                UART1_SendString("\r\nflash�������ģ�");
                UART1_SendString((char * ) data_he);
                UART1_SendString("\nip��ַ: ");
                UART1_SendString(ip);
                UART1_SendString("\r\n");
                UART1_SendString("\n�˿�: ");
                UART1_SendString(port);
                UART1_SendString("\r\n");
                memset(Uart1_Buf, 0, sizeof(Uart1_Buf));
                memset(ip_data_he, 0, sizeof(ip_data_he));
                memset(data_he, 0, sizeof(data_he));
                memset(data_len_he, 0, sizeof(data_len_he));
                CLR_Buf1();
            } 
						else UART1_SendString("����ip��ʽ����\r\n");
            mode = 0;
            break;
				case 7:
						memcpy((char * )( & vid_info[0]), (const char * )( & Uart1_Buf[4]), len - 4); //�洢vid device_id
						vid_info[len - 4] = '\0';
						memcpy(b_vid.save_id,vid_info,13);
				    b_vid.save_id[13] = '\0';
				    
				    memcpy(vid_s,vid_info+14,8);
				    vid_s[8] = '\0';
				    HexStringToBytes(vid_s,8,(char*)vid_hex_s);
				    printf("vid =%s\n",vid_s);
				    b_vid.vid = le32_bytes_to_uint32(vid_hex_s);
					//	printf("deviceid_flash_write=%s\n",b_vid.save_id);
					//	printf("vid_write=0x%08x\r",b_vid.vid);
				
            if (vid_id_check(&b_vid)) 
						{
							f_lseek(vid_log,0);
							ret = f_write(vid_log,&b_vid,sizeof(b_vid),&bw);
							f_sync(vid_log);
							if(ret != FR_OK)
							{
								printflog(ret);
								printf("write vid log failed\n");
								return 1;
							}
              //STMFLASH_Write(FLASH_SAVE_VID, (u16*)&b_vid, sizeof(bind_vid));
              b_vid.vid = 0;
							memset(b_vid.save_id,0,strlen(b_vid.save_id));
							
							f_lseek(vid_log,0);
							ret = f_read(vid_log,&b_vid,sizeof(b_vid),&bw);
							
							if(ret != FR_OK)
							{
								printflog(ret);
								printf("read vid log failed\n");
								return 1;
							}
							
							//STMFLASH_Read(FLASH_SAVE_VID, (u16*)&b_vid.vid, sizeof(bind_vid));
						  printf("�豸 id=%s\r\n",b_vid.save_id);
							//printf("vid_read=0x%08x\n",b_vid.vid);
              memset(Uart1_Buf, 0, sizeof(Uart1_Buf));
						  STMFLASH_Read(FLASH_SAVE_ID,(u16*)id_len,1);
						  STMFLASH_Read(FLASH_SAVE_ID,(u16*)device,id_len[0]);
							device[13] = '\0';
							if(strlen((const char*)device) > 5)
							{
								UART1_SendString("\nԭ�� id=");
								UART1_SendString((char *)device);
								UART1_SendString("\r\n");
							}
							UART1_SendString("�����ѱ���\r\n");
            } 
						else 
						{
              UART1_SendString("vid ����ID ��������,����������\r\n");
              memset(Uart1_Buf, 0, sizeof(Uart1_Buf));
            }
            mode = 0;
				break;
        }
    }
		return 1;
}

//*******************************************************************************
// * ������ : GSM_mtest
// * ����   : GSM/GPRS�����Կ��Ʋ���
// * ����   : 
// * ���   : 
// * ����   : 
// * ע��   : 
// *******************************************************************************/
void GSM_mtest() {
    u8 * p1;
    CLR_Buf2();
    UART1_SendString("������:�����г��ѿƼ����޹�˾\r\n");
    if (GSM_send_cmd("AT+CGMM", "OK", 5) == 0) //��ѯģ������
    {
        UART1_SendString("ģ���ͺ�:");
        p1 = (u8 * ) strstr((const char * )(Uart2_Buf + 2), "\r\n");
        UART1_Send_Len((char * ) Uart2_Buf + 2, p1 - Uart2_Buf);
        CLR_Buf2();
    }
    if (GSM_send_cmd("AT+CGSN", "OK", 5) == 0) //��ѯ��Ʒ���к�
    {
        UART1_SendString("��Ʒ���к�:");
        p1 = (u8 * ) strstr((const char * )(Uart2_Buf + 2), "\r\n");
        UART1_Send_Len((char * ) Uart2_Buf + 2, p1 - Uart2_Buf);
        CLR_Buf2();
    }
    if (GSM_send_cmd("AT+CNUM", "+CNUM", 2) == 0) //��ѯ�������� 
    {
        u8 * p2;
        UART1_SendString("��������:");
        p1 = (u8 * ) strstr((const char * )(Uart2_Buf), "\"");
        p2 = (u8 * ) strstr((const char * )(p1 + 1), "\"");
        p1 = (u8 * ) strstr((const char * )(p2 + 1), "\"");
        UART1_Send_Len((char * )(p1 + 1), 11);
        CLR_Buf2();
    }

}

/*******************************************************************************
* ������ : GSM_gsminfo_gsminfo
* ����   : GSM/GPRS״̬��Ϣ���(�ź�����,��ص���,����ʱ��)
* ����   : 
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/
u8 GSM_gsminfo() {
    u8 * p1;
    u8 * p2;
    u8 res = 0;
    CLR_Buf2();
    if (GSM_send_cmd("AT+CPIN?", "OK", 3)) {
        UART1_SendString("�����ֻ����Ƿ��в���\r\n");
        res |= 1 << 0; //��ѯSIM���Ƿ���λ 
    }

    if (GSM_send_cmd("AT+COPS?", "OK", 3) == 0) //��ѯ��Ӫ������ //ST87����
    {
        p1 = (u8 * ) strstr((const char * )(Uart2_Buf), "\"");
        if (p1) //����Ч����
        {
            p2 = (u8 * ) strstr((const char * )(p1 + 1), "\"");
            UART1_SendString("��Ӫ��:");
            UART1_Send_Len((char * ) p1 + 1, p2 - p1 - 1);
            UART1_SendString("\r\n");
        }
    } else res |= 1 << 1;

    CLR_Buf2();

    if (GSM_send_cmd("AT+CSQ", "OK", 3) == 0) //��ѯ�ź�����
    {
        p1 = (u8 * ) strstr((const char * )(Uart2_Buf), ":");
        if (p1) {
            p2 = (u8 * ) strstr((const char * )(p1 + 1), ",");
            UART1_SendString("�ź�����:");
            UART1_Send_Len((char * ) p1 + 2, p2 - p1 - 2);
            UART1_SendString("\r\n");
        }
    } else res |= 1 << 2;
    CLR_Buf2();

    return res;
}

int get_GPS_data() 
{
    char *p1 = NULL,*p2 = NULL;
    char time_1[11] = "";
    char time_2[7] = "";
	  char long_degree[3];
	  char lati_degree[2];
  	u8 gps_fail = 0,retry = 0;
	  Gps_time = 1;
	  gps_flag = 1;
	  char *outer_ptr=NULL;
	  char gps_longitude[11];
    char gps_latitude[10];
	  static char long_sync = 0;
		static char lati_sync = 0;
	
    if(GSM_send_cmd("AT+CGNSTST=1", "OK", 10) == 0)
		{
	  while(Gps_time <= 3)
	  { 	
		if((p1=(char*)strstr((const char*)Uart2_Buf,"GPRMC")),(p1!=NULL))//Ѱ�ҿ�ʼ��
		{			
				if((p2=(char*)strstr((const char*)p1,"*")),(p2!=NULL))//Ѱ�ҽ�����
				{
					*p2=0;//��ӽ�����	
					p2=strtok_r((p1),",",&outer_ptr);
					p2=(char*)strtok_r(NULL,",",&outer_ptr);
						
					if(strlen((const char*)p2) == 10)
					{
					strcpy(time_1,p2);
					time_1[10] = '\0';
//   					UART1_SendString("ʱ��:");UART1_SendString(time_1);
					}
					     p2 = (char * ) strtok_r(NULL, ",",&outer_ptr);
               if (strstr((const char * ) p2, "A")) 
								{
									//printf("GPS valuable\n");
									gps_fail = 0;
									p2 = (char * ) strtok_r(NULL, ",",&outer_ptr);
									if (strlen((const char * ) p2) == 9 || strlen((const char * ) p2) == 10) 
									{
										memcpy(lati_degree,p2,2);
										if(atoi(lati_degree) > 2 && atoi(lati_degree) < 54)
										{
											long_sync = 1;
											//printf("latitude is ok\n");
											memset(gps_latitude,0,sizeof(const char)* 10);
											memcpy(gps_latitude,p2,9);
											gps_latitude[9] = '\0';
											
										}							 
									}								 
										p2 = (char * ) strtok_r(NULL, ",",&outer_ptr);
										p2 = (char * ) strtok_r(NULL, ",",&outer_ptr);
										if (strlen((char * ) p2) == 10 || strlen((char * ) p2) == 11) 
										{
											memcpy(long_degree,p2,3);
											if(atoi(long_degree) >= 73 && atoi(long_degree) <= 136)
											{
												lati_sync = 1;
											 // printf("longitude is ok\n");
												memset(gps_longitude,0,sizeof(char)* 10);
												memcpy(gps_longitude,p2,10);
												gps_longitude[10] = '\0';
											
											}									
										}
										
										if(strlen(gps_longitude) == 10 && strlen(gps_latitude) == 9 && lati_sync == 1 && long_sync == 1)
										{
											memset(gps.gps_longitude,0,10);
											memset(gps.gps_latitude,0,10);
											memcpy(gps.gps_longitude,gps_longitude,10);
											memcpy(gps.gps_latitude,gps_latitude,9);
											printf("longitude:%s\n",gps_longitude);
											printf("latitude:%s\n", gps_latitude);
										}
										long_sync = 0;
										lati_sync = 0;
										
										p2 = (char * ) strtok_r(NULL, ",",&outer_ptr);
										p2 = (char * ) strtok_r(NULL, ",",&outer_ptr);
										if (strlen((const char * ) p2) <= 5 && strstr((const char*) p2,".")) 
										{
											memset(gps.gps_speed,0,sizeof(gps.gps_speed));
											memcpy(gps.gps_speed_not,p2,strlen(p2));
											gps.gps_speed_not[strlen(p2)] = '\0';
											sprintf(gps.gps_speed, "%d", atoi((char * ) p2) * 2);
											gps.gps_speed[strlen(gps.gps_speed)] = '\0';
											printf("speed = %s kn\n",gps.gps_speed_not);
										}
										p2 = (char * ) strtok_r(NULL, ",",&outer_ptr);
										if(strlen(p2) > 0 && strlen(p2) <= 6 && strstr((const char*) p2,"."))
										{
											memset(gps.gps_course,0,sizeof(gps.gps_course));
											memcpy(gps.gps_course, p2,strlen(p2));
											gps.gps_course[strlen(gps.gps_course)] = '\0';
										  printf("course = %s��\n",gps.gps_course);
										}
	
										p2 = (char * ) strtok_r(NULL, ",",&outer_ptr);
										if (strlen((char * ) p2) == 6) {
											strcpy(time_2, p2);
											time_2[6] = '\0';
										}
								if(strlen(time_2) == 6 && strlen(time_1) == 10)
								{
								memset(gps.gps_time,0,sizeof(gps.gps_time));	
                memcpy(gps.gps_time, time_2 + 4, 2);
                memcpy(gps.gps_time + 2, time_2 + 2, 2);
                memcpy(gps.gps_time + 4, time_2, 2);
                memcpy(gps.gps_time + 6, time_1, 2);
                memcpy(gps.gps_time + 8, time_1 + 2, 2);
                memcpy(gps.gps_time + 10,time_1 + 4, 2);
								gps.gps_time[12] = '\0';
								printf("gps_time: %s\n",gps.gps_time);
								}	
			          CLR_Buf2();
								break;
					}
					else if(strstr((const char*)p2,"V"))
					{
					if(gps_fail == 3)
					{
						gps_fail = 0;
            						
					}
					gps_fail++;
					UART1_SendString("\r\n��λʧ��\r\n");
					CLR_Buf2();
					break;
          }
 				}
		}
		
	}
	  Gps_time = 1;
	  gps_flag = 0;
		do
		{
			if(retry >= 10)
			{
				retry = 0;
				printf("GPS CLOSE FAILED\n");
				break;
			}
			retry++;
		}while(GSM_send_cmd("AT+CGNSTST=0", "OK", 10) != 0);
	}
    return 1;
}

/*******************************************************************************
* ������ : 
* ����   : 
* ����   : �������ip����У��
* ���   : 
* ����   : 
* ע��   : 
*******************************************************************************/

int idcheck(char * p, int num) {
    int i,
    fflag = 1;
    for (i = 0; i < strlen(p); i++) {
        if (p[i] >= '0' && p[i] <= '9') continue;
        else fflag = 0;
    }
    if (num == strlen(p) && fflag != 0) {
        return 1;
    } else return 0;
}

void gps_init(void) {
	GSM_send_cmd("AT+CGNSTST=0", "OK", 10);
  GSM_send_cmd("AT+CGNSPWR=1\r\n", "OK", 10); //��GPS��Դ
  UART1_SendString("gps init....\r\n");
  delay_ms(100);
  GSM_send_cmd("AT+CGNSSEQ=\"RMC\"\r\n", "OK", 10); //����NMEA����
  CLR_Buf2();
}

//GPS ȥ�����Ƚϴ������
int gps_check(char *longitude,char *latitude)
{
	char long_degree[3];
	char lati_degree[2];
	char long_min[2];
	char lati_min[2];
	
	char long_degree_last[3];
	char lati_degree_last[2];
	char long_min_last[2];
	char lati_min_last[2];
	
	char longitude_t[10];
	char latitude_t[9];
	int  long_dec = 0;
	int  lati_dec = 0;
	memcpy(longitude_t,longitude,10);
	memcpy(latitude_t,latitude,9);
	if(strlen(gps.gps_longitude) == 10 && strlen(gps.gps_latitude) == 9)
	{
		 memcpy(long_degree,longitude,3);
		 memcpy(lati_degree,latitude,2);
		 memcpy(long_min,longitude+3,2);
		 memcpy(lati_min,latitude+2,2);
		
		 memcpy(long_degree_last,gps.gps_longitude,3);
		 memcpy(lati_degree_last,gps.gps_latitude,2);
		 memcpy(long_min_last,gps.gps_longitude+3,2);
		 memcpy(lati_min_last,gps.gps_latitude+2,2);
		
		 if(atoi(long_degree) == atoi(long_degree_last))
		 {
		  long_dec = abs(atoi(long_min)-atoi(long_min_last));
		 }
		 else if(abs(atoi(long_degree)-atoi(long_degree_last)) <= 2)
		 {
			long_dec = 60 - abs(atoi(long_min)-atoi(long_min_last));
		 }
		 else 
		 {
			 printf("GPS ERROR\n");
        return 0;
			}
		 
		  if(atoi(lati_degree) == atoi(lati_degree_last))
		 {
		  lati_dec = abs(atoi(lati_min)-atoi(lati_min_last));
		 }
		 else if(abs(atoi(lati_degree) - atoi(lati_degree_last)) <= 2 )
		 {
			lati_dec = 60 - abs(atoi(lati_min)-atoi(lati_min_last));
		 }
		 else 
		 {
			 printf("GPS ERROR\n");
				return 0;
		 }

		if(long_dec >= 3 || lati_dec >= 3)
		{
			printf("GPS ERROR\n");
// 			printf("former longi is %s\n ",gps.gps_longitude);
// 			printf("former lati is %s\n ",gps.gps_latitude);
// 			
// 			printf("now longi is %s\n",longitude_t);
// 			printf("now lati is %s\n",latitude_t);
			return 0;
		}
		else
		{
			printf("GPS DATA OK\n");
			memset(gps.gps_longitude,0,sizeof(gps.gps_longitude));
			memset(gps.gps_latitude,0,sizeof(gps.gps_latitude));
 		  memcpy(gps.gps_longitude,longitude,10);
 		  memcpy(gps.gps_latitude,latitude,9);
			return 1;
		}

	}
	else 
	{
		  printf("gps_reset\n");
		  memset(gps.gps_longitude,0,sizeof(gps.gps_longitude));
			memset(gps.gps_latitude,0,sizeof(gps.gps_latitude));
 		  memcpy(gps.gps_longitude,longitude,10);
 		  memcpy(gps.gps_latitude,latitude,9);
	 }
  return 1;
}


int vid_id_check(bind_vid *v)
{
	if(strstr(v->save_id,"CJET") && strlen(v->save_id) == 13)
	{
		if(v->vid > 0 & v->vid < 0xffffffff)
		{
			return 1;
		}
		else 
			return 0;
	}
	else
		return 0;

}	
