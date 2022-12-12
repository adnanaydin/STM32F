#include <stdio.h>
#include <string.h>

#include "io.h"
#include "system.h"
#include "si2c.h"

enum { FALSE, TRUE };

// �nce Atomik fonksiyonlar

void SI2C_Init(void)
{
  IO_Write(IOP_I2C_SCL, 1);
  IO_Init(IOP_I2C_SCL, IO_MODE_OUTPUT_OD);
  
  IO_Write(IOP_I2C_SDA, 1);
  IO_Init(IOP_I2C_SDA, IO_MODE_OUTPUT_OD);  
}

static void SDA_Wait(void)
{
  // DelayUs(1);
}

static void SCL_Wait(void)
{
  DelayUs(1);
}

static void SDA_Low(void)
{
  IO_Write(IOP_I2C_SDA, 0);
  SDA_Wait();
}

static void SDA_High(void)
{
  IO_Write(IOP_I2C_SDA, 1);
  SDA_Wait();
}

static void SCL_Low(void)
{
  IO_Write(IOP_I2C_SCL, 0);
  SCL_Wait();
}

static void SCL_High(void)
{
  IO_Write(IOP_I2C_SCL, 1);
  SCL_Wait();
}

// �n ko�ul SCL = 0
// ��k��ta SCL = 1, SDA = 1 (idle)
void SI2C_Stop(void)
{
  SDA_Low();   // SCL=0, SDA=0
  
  SCL_High();   // SCL=1, SDA=0
  SDA_High();    // STOP! SCL=1 -> SDA 0 -> 1
}

// Ko�ul: Giri�te ve ��k��ta SCL = 0 olmal�
static void SI2C_SendBit(int b)
{
  // 1) Data Setup
  if (b)
    SDA_High();
  else
    SDA_Low();
  
  // Data setup time
  
  // 2) Clock Generation
  SCL_High();
  SCL_Low();
}

// Ko�ul: Giri�te ve ��k��ta SCL = 0 olmal�
// Geri d�n�� de�eri true, false
static int SI2C_RecvBit(void)
{
  int b;
  
  SDA_High();   //OD'de hatti serbest biraktik ki slave veri gondersin
  
  // 1) Clock active
  SCL_High();
  
  // 2) Sampling (okuma)
  b = IO_Read(IOP_I2C_SDA);
  
  // 3) Clock inactive
  SCL_Low();
  
  return b;  
}

// I2C bus �zerinden 1-byte veri g�nderir
// slave'in ack cevab�na geri d�ner
int SI2C_SendByte(unsigned char val)
{
  int i;
  
  for (i = 0; i < 8; ++i) {
    SI2C_SendBit(val & 0x80);   // 1000 0000
    val <<= 1;
  }
  
  return SI2C_RecvBit();        // ACK bitini okuyoruz
}

int SI2C_SendByte2(unsigned char val)
{
  int i, mask;
  
  mask = 0x80;
  for (i = 0; i < 8; ++i) {
    SI2C_SendBit(val & mask);   // 1000 0000
    mask >>= 1;
  }
  
  return SI2C_RecvBit();   // ACK bitini okuyoruz
}

// I2C bus �zerinden se�ili slave device'tan
// 1-byte veri okur ve geri d�ner
unsigned char SI2C_RecvByte(void)
{
  int i;
  unsigned char val;
  
  for (i = 0; i < 8; ++i) {
    val <<= 1;
    
    if (SI2C_RecvBit())
      val |= 1;
  }
  
  return val;
}

// �n ko�ul SCL = 1
// ��k��ta SCL = 0
// Bu fonksiyon I2C bus �zerinden verilen device adresine 
// start ko�ulu olu�turur ve ctl byte'� g�nderir
// Geri d�n�� de�eri ctl byte'a slave'in ack cevab�
int SI2C_Start(unsigned char ctl)
{
  SDA_High();
  
  // SCL = 1, SDA = 1
  SDA_Low();    // START! SCL=1 iken SDA 1->0
  
  SCL_Low();
  
  // Control byte g�nderiyoruz ve cevab� geri d�n�yoruz
  return SI2C_SendByte(ctl);
}

// I2C yazma c�mlesi
// devAdr: Slave device hardware address (sola ayarl�)
// buf: G�nderilecek verinin ba�lang�� adresi
// len: G�nderilecek veri uzunlu�u
// Geri d�n�� true, false
int SI2C_Write(unsigned char devAdr, const void *buf, int len)
{
  const unsigned char *ptr = (const unsigned char *)buf;
  
  if (len < 0)
    return FALSE;
  
  devAdr &= 0xFE;       // 1111 1110
  
  // Yazma c�mlesi
  if (SI2C_Start(devAdr)) {
    SI2C_Stop();
    return FALSE;
  }
  
  while (len--) {
    if (SI2C_SendByte(*ptr++)) {
      SI2C_Stop();
      return FALSE;      
    }      
  }
  
  SI2C_Stop();
  return TRUE;
}

// I2C okuma c�mlesi
// devAdr: Slave device hardware address (sola ayarl�)
// buf: Okunacak verinin ba�lang�� adresi
// len: Okunacak veri uzunlu�u
// Geri d�n�� true, false
int SI2C_Read(unsigned char devAdr, void *buf, int len)
{
  unsigned char *ptr = (unsigned char *)buf;
  int ack;

  if (len <= 0)
    return FALSE;
  
  // Okuma c�mlesi
  if (SI2C_Start(devAdr | 1)) {
    SI2C_Stop();
    return FALSE;
  }
  
  do {
    *ptr++ = SI2C_RecvByte();
    
    ack = (len == 1);
    SI2C_SendBit(ack);
  } while (--len);
  
  SI2C_Stop();
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Device i� belle�inde (register alan�nda) 1-byte adres + data
// kullanan slave elemanlar i�in yazma okuma fonksiyonlar�

#define SZ_I2CBUF       256

int SI2C_WriteA(unsigned char devAdr, unsigned char regAdr, const void *buf, int len)
{
  static unsigned char i2cbuf[SZ_I2CBUF + 1];
  
  if (len > SZ_I2CBUF)
    return FALSE;
  
  i2cbuf[0] = regAdr;
  memcpy(i2cbuf + 1, buf, len);
  
  return SI2C_Write(devAdr, i2cbuf, len + 1);
}

int SI2C_ReadA(unsigned char devAdr, unsigned char regAdr, void *buf, int len)
{
  if (!SI2C_Write(devAdr, &regAdr, 1))
    return FALSE;
  
  return SI2C_Read(devAdr, buf, len);  
}
