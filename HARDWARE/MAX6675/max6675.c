/*********************************************************************************
 * 文件名  ：main.c
 * 描述    ：通过stm32的spi读取max6675的温度值.
 *          
 * 实验平台：STM32开发板
 * 库版本  ：ST3.0.0
 * 硬件连接： ------------------------------------
 *           |SPI1-MISO：MAX6675-SO          |
 *           |SPI1-MOSI：MAX6675-SI          |
 *           |SPI1-SCK ：MAX6675-SCK         |
 *           |SPI1-NSS ：MAX6675-CS          |
 *            ------------------------------------
**********************************************************************************/
#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "max6675.h"
#include "usart.h"

#define 	MAX6675_CS_2	 	GPIO_Pin_7
#define 	MAX6675_CS	 		GPIO_Pin_4
#define 	MAX6675_CSL()		GPIOA->BRR = MAX6675_CS;
#define 	MAX6675_CSH()		GPIOA->BSRR = MAX6675_CS;
#define 	MAX6675_CSL2()	GPIOA->BRR = MAX6675_CS_2;
#define 	MAX6675_CSH2()	GPIOA->BSRR = MAX6675_CS_2;



/*
 * 函数名：SPI1_Init
 * 描述  MAX6675 接口初始化
 * 输入  ：无 
 * 输出  ：无
 * 返回  ：无
 */																						  
void SPI_MAX6675_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;	
	
	/* 使能 SPI1 时钟 */                         
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);

	/* ---------通信I/O初始化----------------
	 * PA5-SPI1-SCK :MAX6675_SCK
	 * PA6-SPI1-MISO:MAX6675_SO
	 * PA7-SPI1-MOSI:MAX6675_SI	 
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// 复用输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* ---------控制I/O初始化----------------*/
	/* PA4-SPI1-NSS:MAX6675_CS */							// 片选
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// 推免输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);						  
	GPIO_SetBits(GPIOA, GPIO_Pin_4);						// 先把片选拉高，真正用的时候再拉低
	
		/* PA4-SPI1-NSS:MAX6675_CS_2 */							// 片选
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// 推免输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);						  
	GPIO_SetBits(GPIOA, GPIO_Pin_7);						// 先把片选拉高，真正用的时候再拉低

	/* SPI1 配置 */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	
	/* 使能 SPI1  */
	SPI_Cmd(SPI1, ENABLE); 
}

unsigned char MAX6675_ReadByte(void)
{
	
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, 0xff);
	
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}

float read_temp1(void)
{
	
	unsigned int t,i;
	unsigned char c;
	unsigned char flag;
	float temprature;
	SPI_MAX6675_Init();	
	MAX6675_CSL();
	c = MAX6675_ReadByte();
	i = c;
	i = i<<8;
	c = MAX6675_ReadByte();
	MAX6675_CSH();
		
	i = i|((unsigned int)c);			//i是读出来的原始数据
	flag = i&0x04;						//flag保存了热电偶的连接状态
	t = i<<1;
	t = t>>4;
	temprature = t*0.25;
	if(i!=0)							//max6675有数据返回
	{
		if(flag==0)						//热电偶已连接
	 {
		// printf("原始数据是：%04X,  当前温度是：%4.2f。\n",i,temprature);
	 }	
		else							//热电偶掉线
	 {
		 UART1_SendString("未检测到热电偶，请检查。\r\n");
	 }
		
	 }
	  else								//max6675没有数据返回
		{
			 UART1_SendString("max6675没有数据返回，请检查max6675连接。\r\n");
		}
		for(i=0;i<0x3fffff;i++);			//max6675的转换时间是0.2秒左右，所以两次转换间隔不要太近
		return temprature;
}

float read_temp2(void)
{
	
	unsigned int t,i;
	unsigned char c;
	unsigned char flag;
	float temprature;
	SPI_MAX6675_Init();	
	MAX6675_CSL2();
	c = MAX6675_ReadByte();
	i = c;
	i = i<<8;
	c = MAX6675_ReadByte();
	MAX6675_CSH2();
		
	i = i|((unsigned int)c);			//i是读出来的原始数据
	flag = i&0x04;						//flag保存了热电偶的连接状态
	t = i<<1;
	t = t>>4;
	temprature = t*0.25-3;
	if(i!=0)							//max6675有数据返回
	{
		if(flag==0)						//热电偶已连接
	 {
		// printf("原始数据是：%04X,  当前温度是：%4.2f。\n",i,temprature);
	 }	
		else							//热电偶掉线
	 {
		 UART1_SendString("未检测到热电偶，请检查。\r\n");
	 }
		
	 }
	  else								//max6675没有数据返回
		{
			 UART1_SendString("max6675没有数据返回，请检查max6675连接。\r\n");
		}
		for(i=0;i<0x3fffff;i++);			//max6675的转换时间是0.2秒左右，所以两次转换间隔不要太近
		return temprature;
}

