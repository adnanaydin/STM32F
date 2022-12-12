#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "system.h"
#include "io.h"

void init(void)
{
  Sys_IoInit();
  
  IO_Write(IOP_LED, 1);
  IO_Init(IOP_LED, IO_MODE_OUTPUT);
}

int c;

int main()
{
  
  // Çalýþma zamaný yapýlandýrmalarý
  init();
    
  IO_Write(IOP_LED, 0);
  c = IO_Read(IOP_LED);
  
  IO_Write(IOP_LED, 1);
  c = IO_Read(IOP_LED);
  IO_Write(IOP_LED, 0);
  c = IO_Read(IOP_LED);
  IO_Write(IOP_LED, 1);
  c = IO_Read(IOP_LED);
  
  // Görev çevrimi
  while (1) 
  {
  }
  
  //return 0;
}
