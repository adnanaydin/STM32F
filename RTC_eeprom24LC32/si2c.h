#ifndef __SI2C_H
#define __SI2C_H

#ifdef __cplusplus
extern "C" {
#endif

#define IOP_I2C_SCL     IOP_I2C2_SCL    // B10 
#define IOP_I2C_SDA     IOP_I2C2_SDA    // B11

void SI2C_Init(void);

int SI2C_Start(unsigned char ctl);
void SI2C_Stop(void);
int SI2C_SendByte(unsigned char val);
unsigned char SI2C_RecvByte(void);
 
int SI2C_Write(unsigned char devAdr, const void *buf, int len);
int SI2C_Read(unsigned char devAdr, void *buf, int len);

int SI2C_WriteA(unsigned char devAdr, unsigned char regAdr, const void *buf, int len);
int SI2C_ReadA(unsigned char devAdr, unsigned char regAdr, void *buf, int len);


#ifdef __cplusplus
}
#endif

#endif
