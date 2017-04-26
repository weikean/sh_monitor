#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f10x.h"
#include "spiffs.h"


#define SPI_FLASH_SPI                           SPI2
#define SPI_FLASH_SPI_CLK                       RCC_APB2Periph_SPI2
#define SPI_FLASH_SPI_SCK_PIN                   GPIO_Pin_13                  /* PA.05 */
#define SPI_FLASH_SPI_SCK_GPIO_PORT             GPIOB                       /* GPIOA */
#define SPI_FLASH_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOB
#define SPI_FLASH_SPI_MISO_PIN                  GPIO_Pin_14                  /* PA.06 */
#define SPI_FLASH_SPI_MISO_GPIO_PORT            GPIOB                       /* GPIOA */
#define SPI_FLASH_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOB
#define SPI_FLASH_SPI_MOSI_PIN                  GPIO_Pin_15                  /* PA.07 */
#define SPI_FLASH_SPI_MOSI_GPIO_PORT            GPIOB                       /* GPIOA */
#define SPI_FLASH_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOB
#define SPI_FLASH_CS_PIN                        GPIO_Pin_12                  /* PC.04 */
#define SPI_FLASH_CS_GPIO_PORT                  GPIOB                       /* GPIOA */
#define SPI_FLASH_CS_GPIO_CLK                   RCC_APB2Periph_GPIOB



#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_12)


void SPI_FLASH_Init(void);
s32_t SPI_FLASH_SectorErase(u32_t SectorAddr,u32_t Num);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
s32_t SPI_FLASH_BufferWrite(u32_t WriteAddr, u32_t NumByteToWrite, u8_t* pBuffer);
s32_t SPI_FLASH_BufferRead(u32_t ReadAddr, u32_t NumByteToRead, u8_t* pBuffer);
u32 SPI_FLASH_ReadID(void);
u32 SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);


u8 SPI_FLASH_ReadByte(void);
u8 SPI_FLASH_SendByte(u8 byte);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif /* __SPI_FLASH_H */

