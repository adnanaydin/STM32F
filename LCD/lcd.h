#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_DISPLAY_ON          0x04
#define LCD_CURSOR_ON           0x02
#define LCD_CURSOR_BLINK        0x01
  
void LCD_SendCmd(unsigned char c);  
void LCD_PutChar(unsigned char c);
void LCD_Init(void);

void LCD_Clear(void);
void LCD_SetCursor(unsigned char curPos);
void LCD_Enable(unsigned char ctl);

void LCD_putch(unsigned char c);

#ifdef __cplusplus
}
#endif

#endif
