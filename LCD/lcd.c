#include "io.h"
#include "system.h"
#include "lcd.h"

// LCD modüle 4-bit veri gönderir
// (komut ya da karakter)
// Parametre deðerinin low 4-bitini gönderir
// Low nibble
static void LCD_SendDataL(unsigned char c)
{
  // Senkron 4-bit paralel haberleþme
  
  // 1) Data Setup: Verinin hazýrlanmasý
  // DB4 = c<0>
  // DB5 = c<1>
  // DB6 = c<2>
  // DB7 = c<3>
  
  IO_Write(IOP_LCD_DB4, (c & 1) != 0);
  IO_Write(IOP_LCD_DB5, (c & 2) != 0);
  IO_Write(IOP_LCD_DB6, (c & 4) != 0);
  IO_Write(IOP_LCD_DB7, (c & 8) != 0);
  // Veriyi hazýrladýktan sonra verinin stabil hale gelmesi
  // için (veri yollarýnda) bekleme gerekebilir
  // Data setup hold time
  // DelayUs(5)
  
  // 2) Clock Generation: Verinin onaylanmasý
  // Clock hattý 1 - 0 yapýlacak
  IO_Write(IOP_LCD_E, 1);
  DelayUs(3);   // Clock high time
  IO_Write(IOP_LCD_E, 0);
  
  DelayUs(100);   // Clock low time
}

// LCD modüle 4-bit komut gönderir
// Parametredeki deðeri low 4-biti komut
// olarak gönderilir
static void LCD_SendCmdL(unsigned char c)
{
  IO_Write(IOP_LCD_RS, 0); // RS = 0 (komut modu)
  DelayUs(1);              //me
  
  LCD_SendDataL(c);
}

// LCD modüle 8-bit veri gönderir
// Parametredeki deðeri 8-bit olarak gönderilir
// Gönderilen veri karakter kodu ya da komut olabilir
// RS daha önce set edilmeli
static void LCD_SendData(unsigned char c)
{
  LCD_SendDataL(c >> 4);        // High nibble
  LCD_SendDataL(c);             // Low nibble
}

// LCD modüle 8-bit komut gönderir
// Parametredeki deðeri 8-bit komut
// olarak gönderilir
void LCD_SendCmd(unsigned char c)
{
  IO_Write(IOP_LCD_RS, 0); // RS = 0 (komut modu)
  
  LCD_SendData(c);
  DelayUs(100);
}

// LCD modüle 8-bit karakter kodu gönderir
// Parametredeki deðeri 8-bit ASCII karakter
// olarak gönderilir
void LCD_PutChar(unsigned char c)
{
  IO_Write(IOP_LCD_RS, 1); // RS = 1 (karakter modu)
   
  LCD_SendData(c);
}

////////////////////////////////////////////////////////

// Ekraný temizler
void LCD_Clear(void)
{
  LCD_SendCmd(0x01);
  DelayMs(2);
}

void LCD_SetCursor(unsigned char curPos)
{
  LCD_SendCmd(0x80 | curPos);
}

void LCD_Enable(unsigned char ctl)
{
  ctl &= 0x07;
  LCD_SendCmd(0x08 | ctl);
}

// LCD modülü 4-bit arayüzle baþlatýr
void LCD_Init(void)
{
  IO_Init(IOP_LCD_RS, IO_MODE_OUTPUT);
  IO_Init(IOP_LCD_E, IO_MODE_OUTPUT);
  IO_Init(IOP_LCD_DB4, IO_MODE_OUTPUT);
  IO_Init(IOP_LCD_DB5, IO_MODE_OUTPUT);
  IO_Init(IOP_LCD_DB6, IO_MODE_OUTPUT);
  IO_Init(IOP_LCD_DB7, IO_MODE_OUTPUT);
  
  DelayMs(50);  // !! Startup problemleri için gerekiyor!
  
  LCD_SendCmdL(0x03);   // 0011
  DelayMs(5);
  LCD_SendCmdL(0x03);   // 0011
  //DelayUs(101);
  
  LCD_SendCmd(0x32);
  LCD_SendCmd(0x28);    // N=1 F=0
  LCD_Enable(0);        // Display off
  
  // Dökümanda bir eksiklik var!
  // Aslýnda komut 0x01 gönderildikten sonra 
  // 2 ms bekleme gerekiyor
  LCD_Clear();
  
  LCD_SendCmd(0x06);
  LCD_Enable(LCD_DISPLAY_ON);
}

// LCD modül printf destek fonksiyonu
void LCD_putch(unsigned char c)
{
  // Bazý özel karakterlere özel anlam kazandýracaðýz
  
  switch (c) {
  case '\r':
    LCD_SetCursor(0);
    break;

  case '\n':
    LCD_SetCursor(0x40);
    break;

  case '\f':
    LCD_Clear();
    break;
    
  default:
    LCD_PutChar(c);
    break;
  }
}

