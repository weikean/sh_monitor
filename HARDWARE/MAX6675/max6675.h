#ifndef __MAX6675_H
#define __MAX6675_H

void SPI_MAX6675_Init(void);
unsigned char MAX6675_ReadByte(void);
float read_temp1(void);
float read_temp2(void);

#endif
