#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "system.h"
#include "io.h"


void init(void)
{
  Sys_IoInit();
  
  Sys_TickInit();
  
  Sys_ConsoleInit();
  
  IO_Write(IOP_LED, 1);
  IO_Init(IOP_LED, IO_MODE_OUTPUT);
}

void Task_LED(void)
{
  static enum {
    I_LED_ON,
    S_LED_ON,
    
    I_LED_OFF,
    S_LED_OFF
  } state = I_LED_ON;
  
  static clock_t t0, t1;
  
  t1 = clock();
  
  switch (state) {
  case I_LED_ON:
      IO_Write(IOP_LED, 0);     // LED on

      t0 = t1;     // Son ON olma baþlangýç zamaný
      state = S_LED_ON;
      //break;
    
  case S_LED_ON:
    if (t1 - t0 >= CLOCKS_PER_SEC / 10) 
      state = I_LED_OFF;
    break;
    
  case I_LED_OFF:
      IO_Write(IOP_LED, 1);     // LED off

      t0 = t1;     // Son OFF olma baþlangýç zamaný
      state = S_LED_OFF;
      //break;

  case S_LED_OFF:
    if (t1 - t0 >= 9 * CLOCKS_PER_SEC / 10) 
      state = I_LED_ON;
    break;
  }
}

void Task_Print(void)
{
  static unsigned long count;
  
  printf("\nSAYI: %10lu", ++count);
}         
         
int main()
{
  
  // Çalýþma zamaný yapýlandýrmalarý
  init();
  
  
  
  printf("Hello, world!");
  
  
  // Görev çevrimi
  while (1) 
  {
    Task_LED(); 
    
    Task_Print();
  }
  
  //return 0;
}
