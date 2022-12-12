#include <stdio.h>

#include "io.h"
#include "system.h"
#include "lcd.h"
#include "uart.h"

#define _STDIN  0
#define _STDOUT 1
#define _STDERR 2

UART_PERIPH _conUART = UART_1;

static const int GPIO_Clocks[] = {
  RCC_APB2Periph_GPIOA,
  RCC_APB2Periph_GPIOB,
  RCC_APB2Periph_GPIOC,
  RCC_APB2Periph_GPIOD,
  RCC_APB2Periph_GPIOE,
  RCC_APB2Periph_GPIOF,
  RCC_APB2Periph_GPIOG,
};

#define N_PORTS (sizeof(GPIO_Clocks) / sizeof(int))

volatile clock_t _TmTick = 0;

void Sys_IoInit(void)
{
  int i;
  
  for (i = 0; i < N_PORTS; ++i)
    RCC_APB2PeriphClockCmd(GPIO_Clocks[i], ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // JTAG devre dýþý
}

void Sys_TickInit(void)
{
  // I) Core tick timer'ý çalýþtýrýr ve
  // II) Core tick timer kesmesini aktive eder
  SysTick_Config(SystemCoreClock / CLOCKS_PER_SEC);
}

void Sys_ClockTick(void)
{
  ++_TmTick;
}

void Sys_ConsoleInit(void)
{
  LCD_Init();
  //UART_Init(_conUART, 115200);
  
#ifndef __IAR_SYSTEMS_ICC__
  setvbuf(stdout, NULL, _IONBF, 0);
#endif  
}

void _putch(unsigned char c)
{
  LCD_putch(c);
  //UART_putch(_conUART, c);
}

#ifdef __IAR_SYSTEMS_ICC__
size_t __write(int handle, unsigned char *buffer, size_t size)
#else
size_t _write(int handle, unsigned char *buffer, size_t size)
#endif
{
  size_t nChars = 0;
  
  if (buffer == NULL)
    return 0;
  
  if (handle != _STDOUT && handle != _STDERR) 
    return 0;
  
  while (size--) {
    _putch(*buffer++);
    ++nChars;
  }
  
  return nChars;
}

clock_t clock(void)
{
  return _TmTick;
}
  



