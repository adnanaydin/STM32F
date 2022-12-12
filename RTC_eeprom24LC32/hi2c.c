#include <stdio.h>
#include <string.h>

#include "io.h"
#include "system.h"
#include "hi2c.h"

enum { FALSE, TRUE };

#define HI2C_TIMEOUT    1000

static I2C_TypeDef *_i2c[] = {
  I2C1,
  I2C2,
};

void HI2C_Init(int nI2C, int bRate)
{
  I2C_InitTypeDef i2cInit;
  I2C_TypeDef *pI2C = _i2c[nI2C];
  
  if (nI2C == I2C_1) {
    // 1) I/O ucu konfigürasyonu
    IO_Init(IOP_I2C1_SCL, IO_MODE_ALTERNATE_OD);
    IO_Init(IOP_I2C1_SDA, IO_MODE_ALTERNATE_OD);
    
    // 2) I2C Çevresel birim clock iþareti aktive edilmeli
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  }
  else if (nI2C == I2C_2) {
    // 1) I/O ucu konfigürasyonu
    IO_Init(IOP_I2C2_SCL, IO_MODE_ALTERNATE_OD);
    IO_Init(IOP_I2C2_SDA, IO_MODE_ALTERNATE_OD);

    // 2) I2C Çevresel birim clock iþareti aktive edilmeli
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  }
  
  // 3) I2C baþlangýç parametreleri yapýlandýrýlacak
  i2cInit.I2C_Ack = I2C_Ack_Enable; // ??
  i2cInit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  i2cInit.I2C_ClockSpeed = bRate;
  i2cInit.I2C_DutyCycle = I2C_DutyCycle_2;
  i2cInit.I2C_Mode = I2C_Mode_I2C;
  i2cInit.I2C_OwnAddress1 = 0x55;
  
  I2C_Init(pI2C, &i2cInit);
  
  // 4) Çevresel birim aktive edilmeli
  I2C_Cmd(pI2C, ENABLE);;
}

// Belirtilen portta belirtilen olay belirlenmiþ
// zamanaþýmý öncesi gerçekleþirse true döner
static int HI2C_Event(int nI2C, int event)
{
  I2C_TypeDef *pI2C = _i2c[nI2C];
  int tmOut = HI2C_TIMEOUT;
  
  do {
    if (I2C_CheckEvent(pI2C, event) == SUCCESS)
      break;
  } while (--tmOut);
  
  return (tmOut != 0);
}

void HI2C_Stop(int nI2C)
{
  I2C_TypeDef *pI2C = _i2c[nI2C];
  
  I2C_GenerateSTOP(pI2C, ENABLE);
}

int HI2C_Start(int nI2C, unsigned char ctl)
{
  I2C_TypeDef *pI2C = _i2c[nI2C];
  int stat = FALSE, nTry;
  
  nTry = 100;
  do {
    I2C_GenerateSTART(pI2C, ENABLE);
    
    if (HI2C_Event(nI2C, I2C_EVENT_MASTER_MODE_SELECT)) {
      I2C_SendData(pI2C, ctl);
      
      stat = HI2C_Event(nI2C, ctl & 1 ? 
        I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED :
        I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
      
      if (stat)
        break;
    }
  } while (--nTry);
  
  return stat;
}

int HI2C_Check(int nI2C, unsigned char ctl)
{
  int stat;
  
  stat = HI2C_Start(nI2C, ctl);
  HI2C_Stop(nI2C);
  
  return stat;
}

// I2C bus üzerinden 1-byte veri gönderir
// baþarýlý ise true döner
int HI2C_SendByte(int nI2C, unsigned char val)
{
  I2C_TypeDef *pI2C = _i2c[nI2C];

  I2C_SendData(pI2C, val);
  
  return HI2C_Event(nI2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
}

int HI2C_RecvByte(int nI2C, unsigned char *pVal)
{
  I2C_TypeDef *pI2C = _i2c[nI2C];

  if (!HI2C_Event(nI2C, I2C_EVENT_MASTER_BYTE_RECEIVED))
    return FALSE;
  
  *pVal = I2C_ReceiveData(pI2C);
  return TRUE;
}

// Slave'e master tarafýndan gönderilecek olan 
// ack bitini yapýlandýrýr
void HI2C_Acknowledge(int nI2C, int ack)
{
  I2C_TypeDef *pI2C = _i2c[nI2C];
  
  I2C_AcknowledgeConfig(pI2C, ack ? DISABLE : ENABLE);
}

// I2C yazma cümlesi
// devAdr: Slave device hardware address (sola ayarlý)
// buf: Gönderilecek verinin baþlangýç adresi
// len: Gönderilecek veri uzunluðu
// Geri dönüþ true, false
int HI2C_Write(int nI2C, unsigned char devAdr, const void *buf, int len)
{
  const unsigned char *ptr = (const unsigned char *)buf;
  
  if (len < 0)
    return FALSE;
  
  devAdr &= 0xFE;       // 1111 1110
  
  // Yazma cümlesi
  if (!HI2C_Start(nI2C, devAdr)) 
    return FALSE;
  
  while (len--) {
    if (!HI2C_SendByte(nI2C, *ptr++)) {
      HI2C_Stop(nI2C);
      return FALSE;      
    }      
  }
  
  HI2C_Stop(nI2C);
  return TRUE;
}

// I2C okuma cümlesi
// devAdr: Slave device hardware address (sola ayarlý)
// buf: Okunacak verinin baþlangýç adresi
// len: Okunacak veri uzunluðu
// Geri dönüþ true, false
int HI2C_Read(int nI2C, unsigned char devAdr, void *buf, int len)
{
  unsigned char *ptr = (unsigned char *)buf;
  int ack;

  if (len <= 0)
    return FALSE;
  
  if (!HI2C_Start(nI2C, devAdr | 1))
    return FALSE;
  
  do {
    ack = (len == 1);
    HI2C_Acknowledge(nI2C, ack);
    
    if (!HI2C_RecvByte(nI2C, ptr++))
      break;
  } while (--len);

  HI2C_Stop(nI2C);
  return !len;
}

////////////////////////////////////////////////////////////////////////////////
// Device iç belleðinde (register alanýnda) 1-byte adres + data
// kullanan slave elemanlar için yazma okuma fonksiyonlarý

#define SZ_I2CBUF       256

int HI2C_WriteA(int nI2C, unsigned char devAdr, unsigned char regAdr, const void *buf, int len)
{
  static unsigned char i2cbuf[SZ_I2CBUF + 1];
  
  if (len > SZ_I2CBUF)
    return FALSE;
  
  i2cbuf[0] = regAdr;
  memcpy(i2cbuf + 1, buf, len);
  
  return HI2C_Write(nI2C, devAdr, i2cbuf, len + 1);
}

int HI2C_ReadA(int nI2C, unsigned char devAdr, unsigned char regAdr, void *buf, int len)
{
  if (!HI2C_Write(nI2C, devAdr, &regAdr, 1))
    return FALSE;
  
  return HI2C_Read(nI2C, devAdr, buf, len);  
}
