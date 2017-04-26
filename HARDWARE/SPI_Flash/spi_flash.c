  
#include "spi_flash.h"
#include "stm32f10x_spi.h"

/***********************************************************************************
       W25X系列SPI FLASH存储器存储结构介绍
- 每256bytes为一个Page，每4Kbytes为一个Sector，每16个扇区为1个Block
- W25X16容量为2M字节,共有32个Block, 512个Sector  , 8192个Page
- W25X32容量为4M字节,共有64个Block, 1024个Sector ,16384个Page
- W25X64容量为8M字节,共有128个Block,2048个Sector ,32768个Page
**********************************************************************************************/

// static u8_t SPIx_ReadWriteByte(u8_t TxData);
/* Private typedef -----------------------------------------------------------*/
//#define SPI_FLASH_PageSize      4096
#define SPI_FLASH_PageSize      256
#define SPI_FLASH_PerWritePageSize      256

/* Private define ------------------------------------------------------------*/
#define W25X_WriteEnable		      0x06 
#define W25X_WriteDisable		      0x04 
#define W25X_ReadStatusReg		    0x05 
#define W25X_WriteStatusReg		    0x01 
#define W25X_ReadData			        0x03 
#define W25X_FastReadData		      0x0B 
#define W25X_FastReadDual		      0x3B 
#define W25X_PageProgram		      0x02 
#define W25X_BlockErase			      0xD8 
#define W25X_SectorErase		      0x20 
#define W25X_ChipErase			      0xC7 
#define W25X_PowerDown			      0xB9 
#define W25X_ReleasePowerDown	    0xAB 
#define W25X_DeviceID			        0xAB 
#define W25X_ManufactDeviceID   	0x90 
#define W25X_JedecDeviceID		    0x9F 

#define WIP_Flag                  0x01  /* Write In Progress (WIP) flag */

#define Dummy_Byte                0xFF
/***************************************************************************************
*  STM32 SPI 初始化
*/
void SPI_FLASH_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable SPI1 and GPIO clocks */
  /*!< SPI_FLASH_SPI_CS_GPIO, SPI_FLASH_SPI_MOSI_GPIO, 
       SPI_FLASH_SPI_MISO_GPIO, SPI_FLASH_SPI_DETECT_GPIO 
       and SPI_FLASH_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);

  /*!< SPI_FLASH_SPI Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
 
  
  /*!< Configure SPI_FLASH_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* SPI1 configuration */
  // W25X16: data input on the DIO pin is sampled on the rising edge of the CLK. 
  // Data on the DO and DIO pins are clocked out on the falling edge of CLK.
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI2, ENABLE); 
}   



// /**************************************************************************************************************
// *  SPIx 读写一个字节        TxData:要写入的字节
// *  返回值:读取到的字节
// */
// static u8_t SPIx_ReadWriteByte(u8_t TxData)
// {		
// 	u8 retry=0;				 	
// 	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
// 		{
// 		retry++;
// 		if(retry>200)return 0;
// 		}			  
// 	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
// 	retry=0;

// 	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
// 		{
// 		retry++;
// 		if(retry>200)return 0;
// 		}	  						    
// 	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据					    
// }


// /*****************************************************
// * 读取SPI_FLASH的状态寄存器
// * BIT7  6   5   4   3   2   1   0
// * SPR   RV  TB BP2 BP1 BP0 WEL BUSY
// * SPR:默认0,状态寄存器保护位,配合WP使用
// * TB,BP2,BP1,BP0:FLASH区域写保护设置
// * WEL:写使能锁定
// * BUSY:忙标记位(1,忙;0,空闲)
// * 默认:0x00
// */
// static u8_t SPI_Flash_ReadSR(void)   
// {  
// 	u8 byte=0;   
// 	SPI_FLASH_CS_LOW();                            //使能器件   
// 	SPIx_ReadWriteByte(W25X_ReadStatusReg);    //发送读取状态寄存器命令    
// 	byte=SPIx_ReadWriteByte(0Xff);             //读取一个字节  
// 	SPI_FLASH_CS_HIGH();                            //取消片选     
// 	return byte;   
// } 

// /*********************************************************************
// * SPI_FLASH写使能	, 将WEL置位 
// */  
// static void SPI_FLASH_Write_Enable(void)   
// {
// 	SPI_FLASH_CS_LOW();                            //使能器件   
//   SPIx_ReadWriteByte(W25X_WriteEnable);      //发送写使能  
// 	SPI_FLASH_CS_HIGH();                            //取消片选     	      
// } 

// /*********************************************************************
// *  等待SPI FLASH 空闲
// */  
// static void SPI_Flash_Wait_Busy(void)   
// {   
// 	while ((SPI_Flash_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
// }  



// /**********************************************************************
// *  读取芯片ID W25X16的ID:0XEF14
// */
// u16_t SPI_Flash_ReadID(void)
// {
// // 	u16 Temp = 0;	  
// // 	SPI_FLASH_CS_LOW();				    
// // 	SPIx_ReadWriteByte(0x90);//发送读取ID命令	    
// // 	SPIx_ReadWriteByte(0x00); 	    
// // 	SPIx_ReadWriteByte(0x00); 	    
// // 	SPIx_ReadWriteByte(0x00); 	 			   
// // 	Temp|=SPIx_ReadWriteByte(0xFF)<<8;  
// // 	Temp|=SPIx_ReadWriteByte(0xFF);	 
// // 	SPI_FLASH_CS_HIGH();				    
// // 	return Temp;

// }   		    


// /********************************************************
// * 擦除整个芯片  W25X16:25s  W25X32:40s  W25X64:40s
// */
// void SPI_Flash_Erase_Chip(void)   
// {                                             
//     SPI_FLASH_Write_Enable();                  //SET WEL 
//     SPI_Flash_Wait_Busy();   
//   	SPI_FLASH_CS_LOW();                            //使能器件   
//     SPIx_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令  
// 	  SPI_FLASH_CS_HIGH();                            //取消片选     	      
// 	  SPI_Flash_Wait_Busy();   				   //等待芯片擦除结束
// }   



// /****************************************************************************
// *  在指定的起始地址处读取指定长度的数据 (文件系统接口函数)
// *  pBuffer:数据存储区   ReadAddr:开始读取的地址(24bit)
// *  NumByteToRead:要读取的字节数(最大65535)
// */
// s32_t SPI_Flash_Read(u32_t ReadAddr,u32_t NumByteToRead,u8_t * pBuffer)   
// { 
//  	u16 i;    												    
// 	SPI_FLASH_CS_LOW();                            //使能器件   
//     SPIx_ReadWriteByte(W25X_ReadData);         //发送读取命令   
//     SPIx_ReadWriteByte((u8_t)((ReadAddr)>>16));  //发送24bit地址    
//     SPIx_ReadWriteByte((u8_t)((ReadAddr)>>8));   
//     SPIx_ReadWriteByte((u8_t)ReadAddr);   
//     for(i=0;i<NumByteToRead;i++)
// 	{ 
//         pBuffer[i]=SPIx_ReadWriteByte(0XFF);   //循环读数  
//     }
// 	SPI_FLASH_CS_HIGH();                            //取消片选     

//   return 0;		
// } 


// /**************************************************************************************
// *  在指定的起始地址处开始写入最大256字节的数据 (文件系统接口函数)
// *  pBuffer:数据存储区  WriteAddr:开始写入的地址(24bit)
// *  NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
// */
// s32_t SPI_Flash_Write(u32_t WriteAddr,u32_t NumByteToWrite,u8_t * pBuffer)
// {
//  	u16 i;  
//   SPI_FLASH_Write_Enable();                  //SET WEL 
// 	SPI_FLASH_CS_LOW();                            //使能器件   
//   SPIx_ReadWriteByte(W25X_PageProgram);      //发送写页命令   
//   SPIx_ReadWriteByte((u8)((WriteAddr)>>16)); //发送24bit地址    
//   SPIx_ReadWriteByte((u8)((WriteAddr)>>8));   
//   SPIx_ReadWriteByte((u8)WriteAddr);   
//   for(i=0;i<NumByteToWrite;i++)SPIx_ReadWriteByte(pBuffer[i]);//循环写数  
// 	SPI_FLASH_CS_HIGH();                            //取消片选 
// 	SPI_Flash_Wait_Busy();					       //等待写入结束
// 	
// 	return 0;
// } 

// /*************************************************************************************
// *   擦除多个扇区 (文件系统接口函数)
// *   Addr ：擦除起始地址  
// *   Num ： 要擦除扇区个数
// */
// s32_t SPI_Flash_Erase(u32_t Addr,u32_t Num)   
// {   
// 	  
// 	  u32_t secpos;
// 	  u32_t Address;
// 	  u32_t i;
// 	  secpos = Addr/4096; //扇区地址 (计算给定的地址处于第几个扇区)
// 	
// 	  for(i=0;i<Num;i++) {
// 	  Address = secpos * 4096;        //根据扇区计算扇区的起始地址
// 	  SPI_FLASH_Write_Enable();                  //SET WEL 	 
//     SPI_Flash_Wait_Busy();   
//   	SPI_FLASH_CS_LOW();                        //使能器件   
//     SPIx_ReadWriteByte(W25X_SectorErase);      //发送扇区擦除指令 
//     SPIx_ReadWriteByte((u8_t)((Address)>>16));  //发送24bit地址    
//     SPIx_ReadWriteByte((u8_t)((Address)>>8));   
//     SPIx_ReadWriteByte((u8_t)Address);  
// 	  SPI_FLASH_CS_HIGH();                        //取消片选     	      
//     SPI_Flash_Wait_Busy();   				            //等待擦除完成
// 		secpos++;	
// 		}
// 	  
// 	return 0;
// }  


/*******************************************************************************
* Function Name  : SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
s32_t SPI_FLASH_SectorErase(u32_t SectorAddr,u32_t Num)
{
	
	u32_t secpos;
	u32_t Address;
	u32_t i;
	secpos = SectorAddr/4096; //扇区地址 (计算给定的地址处于第几个扇区)
  /* Send write enable instruction */
	for(i=0;i<Num;i++) {
	 Address = secpos * 4096;        //根据扇区计算扇区的起始地址
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  /* Sector Erase */
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send Sector Erase instruction */
  SPI_FLASH_SendByte(W25X_SectorErase);
  /* Send SectorAddr high nibble address byte */
  SPI_FLASH_SendByte((Address & 0xFF0000) >> 16);
  /* Send SectorAddr medium nibble address byte */
  SPI_FLASH_SendByte((Address & 0xFF00) >> 8);
  /* Send SectorAddr low nibble address byte */
  SPI_FLASH_SendByte(Address & 0xFF);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
	secpos++;		
	}
	return 0;
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BulkErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BulkErase(void)
{
  /* Send write enable instruction */
  SPI_FLASH_WriteEnable();

  /* Bulk Erase */
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send Bulk Erase instruction  */
  SPI_FLASH_SendByte(W25X_ChipErase);
  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  /* Enable the write access to the FLASH */
  SPI_FLASH_WriteEnable();

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();
  /* Send "Write to Memory " instruction */
  SPI_FLASH_SendByte(W25X_PageProgram);
  /* Send WriteAddr high nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /* Send WriteAddr medium nibble address byte to write to */
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /* Send WriteAddr low nibble address byte to write to */
  SPI_FLASH_SendByte(WriteAddr & 0xFF);

  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
     //printf("\n\r Err: SPI_FLASH_PageWrite too large!");
  }

  /* while there is data to be written on the FLASH */
  while (NumByteToWrite--)
  {
    /* Send the current byte */
    SPI_FLASH_SendByte(*pBuffer);
    /* Point on the next byte to be written */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  /* Wait the end of Flash writing */
  SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
s32_t SPI_FLASH_BufferWrite(u32_t WriteAddr, u32_t NumByteToWrite, u8_t* pBuffer)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % SPI_FLASH_PageSize;
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

  if (Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
  {
    if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    {
      if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      {
        temp = NumOfSingle - count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /* NumByteToWrite > SPI_FLASH_PageSize */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        WriteAddr +=  SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
      }

      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
	return 0;
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferRead
* Description    : Reads a block of data from the FLASH.
* Input          : - pBuffer : pointer to the buffer that receives the data read
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
s32_t SPI_FLASH_BufferRead(u32_t ReadAddr, u32_t NumByteToRead, u8_t* pBuffer)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send ReadAddr high nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte to read from */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte to read from */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);

  while (NumByteToRead--) /* while there is data to be read */
  {
    /* Read a byte from the FLASH */
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /* Point to the next location where the byte read will be saved */
    pBuffer++;
  }

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
	return 0;
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "RDID " instruction */
  SPI_FLASH_SendByte(W25X_JedecDeviceID);

  /* Read a byte from the FLASH */
  Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

  /* Read a byte from the FLASH */
  Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

  /* Read a byte from the FLASH */
  Temp2 = SPI_FLASH_SendByte(Dummy_Byte);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  return Temp;
}
/*******************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
u32 SPI_FLASH_ReadDeviceID(void)
{
  u32 Temp = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "RDID " instruction */
  SPI_FLASH_SendByte(W25X_DeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  
  /* Read a byte from the FLASH */
  Temp = SPI_FLASH_SendByte(Dummy_Byte);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();

  return Temp;
}
/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read from Memory " instruction */
  SPI_FLASH_SendByte(W25X_ReadData);

  /* Send the 24-bit address of the address to read from -----------------------*/
  /* Send ReadAddr high nibble address byte */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* Send ReadAddr medium nibble address byte */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* Send ReadAddr low nibble address byte */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
u8 SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_FLASH_SendByte(u8 byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI2, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendHalfWord
* Description    : Sends a Half Word through the SPI interface and return the
*                  Half Word received from the SPI bus.
* Input          : Half Word : Half Word to send.
* Output         : None
* Return         : The value of the received Half Word.
*******************************************************************************/
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send Half Word through the SPI1 peripheral */
  SPI_I2S_SendData(SPI2, HalfWord);

  /* Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Write Enable" instruction */
  SPI_FLASH_SendByte(W25X_WriteEnable);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WaitForWriteEnd
* Description    : Polls the status of the Write In Progress (WIP) flag in the
*                  FLASH's status  register  and  loop  until write  opertaion
*                  has completed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;

  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Read Status Register" instruction */
  SPI_FLASH_SendByte(W25X_ReadStatusReg);

  /* Loop as long as the memory is busy with a write cycle */
  do
  {
    /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);	 
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}


//进入掉电模式
void SPI_Flash_PowerDown(void)   
{ 
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_PowerDown);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();
}   

//唤醒
void SPI_Flash_WAKEUP(void)   
{
  /* Select the FLASH: Chip Select low */
  SPI_FLASH_CS_LOW();

  /* Send "Power Down" instruction */
  SPI_FLASH_SendByte(W25X_ReleasePowerDown);

  /* Deselect the FLASH: Chip Select high */
  SPI_FLASH_CS_HIGH();                   //等待TRES1
}  





