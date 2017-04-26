 #include "pressure.h"
 #include "stdio.h"
 #include "usart.h"
 #include "stm32f10x_adc.h"
 #include "delay.h"
 #include "user_config.h"
/*/////////////////////////////////////////////////////////////////////////////////	 
�ɼ�
//All rights reserved									  
*////////////////////////////////////////////////////////////////////////////////// 
	   
		   
//��ʼ��ADC
//�������ǽ��Թ���ͨ��Ϊ��
//����Ĭ�Ͻ�����ͨ��0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1	, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����
	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������

}				  
//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
}


u32 get_pressure_3(void)
{
		u16 adcx;
		float temp;
		char tem[20];
// 	  UART1_SendString("adc_3 :");
		adcx=Get_Adc_Average(ADC_Channel_10,30);
		temp=(float)adcx*(3.3/4096);
	  sprintf(tem,"%2.2f",temp * 11);
	  temp = (float)((temp*11-0.5)/4.6368) *100000;
// 		UART1_SendString(tem);
// 		UART1_SendString("\r\n");	
	  return temp;
}


u32 get_pressure_2(void)
{
		u16 adcx;
		float temp;
		char tem[20];
// 	  UART1_SendString("adc_2 :");
		adcx=Get_Adc_Average(ADC_Channel_11,30);
		temp=(float)adcx*(3.3/4096);
	  sprintf(tem,"%2.2f",temp * 11);
	  temp = (float)((temp*11-0.5)/4.6368) *100000;
// 		UART1_SendString(tem);
// 		UART1_SendString("\r\n");	
	  return temp;
}
u32 get_pressure_1(void)
{
		u16 adcx;
		float temp;
		char tem[20];
// 	  UART1_SendString("adc_1 :");
		adcx=Get_Adc_Average(ADC_Channel_12,30);
		temp=(float)adcx*(3.3/4096.0);
	  sprintf(tem,"%2.2f",temp * 11);
	  temp = (float)((temp*11-0.5)/4.6368) *100000;
// 		UART1_SendString(tem);
// 		UART1_SendString("\r\n");	
	  return temp;
}
























