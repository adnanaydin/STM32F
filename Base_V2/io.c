#define _IOS_
#include "io.h"

void IO_Init(int idx, int mode)
{
  GPIO_InitTypeDef ioInit;
  
  ioInit.GPIO_Pin = (1 << _ios[idx].pin); 
  ioInit.GPIO_Mode = (GPIOMode_TypeDef)mode;
  ioInit.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(_ios[idx].port, &ioInit);
}

void IO_Write(int idx, int val)
{
  GPIO_TypeDef *GPIOx;
  int mask;
  
  GPIOx = _ios[idx].port;
  
  mask = (1 << _ios[idx].pin);
  if (val)
    GPIOx->BSRR = mask;
  else
    GPIOx->BRR = mask;    
}

int IO_Read(int idx)
{
  GPIO_TypeDef *GPIOx;
  int mask;
  
  GPIOx = _ios[idx].port;
  
  mask = (1 << _ios[idx].pin);
  return (GPIOx->IDR & mask) != 0;
}



