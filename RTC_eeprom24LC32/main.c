#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "system.h"
#include "io.h"
#include "lcd.h"
#include "uart.h"
#include "button.h"

#include "hi2c.h"
#include "ertc.h"

void init(void)
{
  Sys_IoInit();
  
  Sys_TickInit();
  
  Sys_ConsoleInit();
  
  UART_Init(UART_1, 115200);
  
  BTN_Init();
  
  IO_Write(IOP_LED, 1);
  IO_Init(IOP_LED, IO_MODE_OUTPUT);

  ERTC_Init();
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

void Task_ERTC(void)
{
  DS_DATE dt;
  int8_t temp;
  
  ERTC_GetDate(&dt);

  printf("\r%02X:%02X:%02X", dt.hour, dt.min, dt.sec);
  printf("\n%02X/%02X/%02X", dt.date, dt.mon, dt.year);

  if (HI2C_ReadA(I2C_ERTC, I2C_ADR_DS3231, 0x11, &temp, 1)) {
    printf(" %3d C", temp);
  }
}

int main()
{
  // Çalýþma zamaný yapýlandýrmalarý
  init();
  
  /*
  HI2C_Init(I2C_2, 400000);
  
  int adr, stat;
  
  adr = 0xD0;
  stat = HI2C_Check(I2C_2, adr);
  printf("\nDevice(%2X): %d", adr, stat);
    
  adr = 0xA0;
  stat = HI2C_Check(I2C_2, adr);
  printf("\nDevice(%2X): %d", adr, stat);
    
  adr = 0xAE;
  stat = HI2C_Check(I2C_2, adr);
  printf("\nDevice(%2X): %d", adr, stat);
  */
    
  while (1) 
  {
    Task_LED();     
    Task_ERTC();
  }
  
  //return 0;
}
