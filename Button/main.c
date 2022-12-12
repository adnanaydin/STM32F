#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "lcd.h"
#include "uart.h"
#include "button.h"

void init(void)
{
  Sys_IoInit();
  
  Sys_TickInit();
  
  Sys_ConsoleInit();
  
  UART_Init(UART_1, 115200);
  
  BTN_Init();
  
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
  
  printf("SAYI: %10lu\n", ++count);
  
  UART_printf(UART_1, "Count: %10lu\r", count);
}         
      
void Task_Buttons(void)
{
  static int iSet, iUp, iDn;
  
  if (g_sButtons[BTN_SET]) {
    UART_printf(UART_1, "\n*SET* = %d\n", ++iSet);
    //g_sButtons[BTN_SET] = 0;    // binary semaphore
    --g_sButtons[BTN_SET];        // counting semaphore
  }

  if (g_sButtons[BTN_UP]) {
    UART_printf(UART_1, "\n *UP* = %d\n", ++iUp);
    //g_sButtons[BTN_UP] = 0;     // binary semaphore
    --g_sButtons[BTN_UP];        // counting semaphore
  }

  if (g_sButtons[BTN_DN]) {
    UART_printf(UART_1, "\n *DN* = %d\n", ++iDn);
    //g_sButtons[BTN_DN] = 0;     // binary semaphore
    --g_sButtons[BTN_DN];        // counting semaphore
  }
}

int main()
{
  // Çalýþma zamaný yapýlandýrmalarý
  init();
    
  printf("Hello, world!\n");
  UART_printf(UART_1, "\n\nButton Test\n");
  
  while (1) 
  {
    Task_LED();     
    Task_Print();
    
    //DelayMs(5000);
  
    Task_Buttons();
    
    /*
    UART_printf(UART_1, "\rSET=%d UP=%d", 
           IO_Read(IOP_BTN_SET), 
           IO_Read(IOP_BTN_UP));
    */
  }
  
  //return 0;
}
