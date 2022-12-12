#ifndef __HI2C_H
#define __HI2C_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
  I2C_1,
  I2C_2,
};

void HI2C_Init(int nI2C, int bRate);
int HI2C_Start(int nI2C, unsigned char ctl);
int HI2C_Check(int nI2C, unsigned char ctl);

int HI2C_Write(int nI2C, unsigned char devAdr, const void *buf, int len);
int HI2C_Read(int nI2C, unsigned char devAdr, void *buf, int len);

int HI2C_WriteA(int nI2C, unsigned char devAdr, unsigned char regAdr, const void *buf, int len);
int HI2C_ReadA(int nI2C, unsigned char devAdr, unsigned char regAdr, void *buf, int len);


#ifdef __cplusplus
}
#endif

#endif
