#define _IOS_
#include "io.h"

void IO_Init(IO_IDX idx, IO_MODE mode)
{
  GPIO_InitTypeDef ioInit;
  int i;
  
  ioInit.GPIO_Pin = (1 << _ios[idx].pin); 
  ioInit.GPIO_Mode = (GPIOMode_TypeDef)mode;
  ioInit.GPIO_Speed = GPIO_Speed_50MHz;
  
  i = _ios[idx].port;
  GPIO_Init(GPIO_Ports[i], &ioInit);
}

void IO_Write(IO_IDX idx, int val)
{
  GPIO_TypeDef *GPIOx;
  int i, mask;
  
  i = _ios[idx].port;
  GPIOx = GPIO_Ports[i];
  
  mask = (1 << _ios[idx].pin);
  if (val)
    GPIOx->BSRR = mask;
  else
    GPIOx->BRR = mask;    
}

int IO_Read(IO_IDX idx)
{
  GPIO_TypeDef *GPIOx;
  int i, mask;
  
  i = _ios[idx].port;
  GPIOx = GPIO_Ports[i];
  
  mask = (1 << _ios[idx].pin);
  return (GPIOx->IDR & mask) != 0;
}



