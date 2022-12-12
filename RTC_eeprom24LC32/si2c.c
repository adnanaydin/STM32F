#include <stdio.h>
#include <string.h>

#include "io.h"
#include "system.h"
#include "si2c.h"

enum { FALSE, TRUE };

// Önce Atomik fonksiyonlar

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

// Ön koþul SCL = 0
// Çýkýþta SCL = 1, SDA = 1 (idle)
void SI2C_Stop(void)
{
  SDA_Low();   // SCL=0, SDA=0
  
  SCL_High();   // SCL=1, SDA=0
  SDA_High();    // STOP! SCL=1 -> SDA 0 -> 1
}

// Koþul: Giriþte ve çýkýþta SCL = 0 olmalý
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

// Koþul: Giriþte ve çýkýþta SCL = 0 olmalý
// Geri dönüþ deðeri true, false
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

// I2C bus üzerinden 1-byte veri gönderir
// slave'in ack cevabýna geri döner
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

// I2C bus üzerinden seçili slave device'tan
// 1-byte veri okur ve geri döner
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

// Ön koþul SCL = 1
// Çýkýþta SCL = 0
// Bu fonksiyon I2C bus üzerinden verilen device adresine 
// start koþulu oluþturur ve ctl byte'ý gönderir
// Geri dönüþ deðeri ctl byte'a slave'in ack cevabý
int SI2C_Start(unsigned char ctl)
{
  SDA_High();
  
  // SCL = 1, SDA = 1
  SDA_Low();    // START! SCL=1 iken SDA 1->0
  
  SCL_Low();
  
  // Control byte gönderiyoruz ve cevabý geri dönüyoruz
  return SI2C_SendByte(ctl);
}

// I2C yazma cümlesi
// devAdr: Slave device hardware address (sola ayarlý)
// buf: Gönderilecek verinin baþlangýç adresi
// len: Gönderilecek veri uzunluðu
// Geri dönüþ true, false
int SI2C_Write(unsigned char devAdr, const void *buf, int len)
{
  const unsigned char *ptr = (const unsigned char *)buf;
  
  if (len < 0)
    return FALSE;
  
  devAdr &= 0xFE;       // 1111 1110
  
  // Yazma cümlesi
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

// I2C okuma cümlesi
// devAdr: Slave device hardware address (sola ayarlý)
// buf: Okunacak verinin baþlangýç adresi
// len: Okunacak veri uzunluðu
// Geri dönüþ true, false
int SI2C_Read(unsigned char devAdr, void *buf, int len)
{
  unsigned char *ptr = (unsigned char *)buf;
  int ack;

  if (len <= 0)
    return FALSE;
  
  // Okuma cümlesi
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
// Device iç belleðinde (register alanýnda) 1-byte adres + data
// kullanan slave elemanlar için yazma okuma fonksiyonlarý

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
