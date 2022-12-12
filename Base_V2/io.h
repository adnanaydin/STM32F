#ifndef __IO_H
#define __IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"  

// I/O MODLARI
enum {  
  IO_MODE_INPUT = GPIO_Mode_IN_FLOATING,
  IO_MODE_OUTPUT = GPIO_Mode_Out_PP,
  IO_MODE_ALTERNATE = GPIO_Mode_AF_PP,
  IO_MODE_ANALOG = GPIO_Mode_AIN,
  
  IO_MODE_INPUT_PD = GPIO_Mode_IPD,
  IO_MODE_INPUT_PU = GPIO_Mode_IPU,
  IO_MODE_OUTPUT_OD = GPIO_Mode_Out_OD,
  IO_MODE_ALTERNATE_OD = GPIO_Mode_AF_OD
};
  
enum {
  IO_PORT_A,
  IO_PORT_B,
  IO_PORT_C,
  IO_PORT_D,
  IO_PORT_E,
  IO_PORT_F,
  IO_PORT_G,
};

typedef struct {
  GPIO_TypeDef *port;
  int pin;
} IO_PIN;

enum {
  IOP_LED,
  IOP_BUTTON,
  
  IOP_DRIVE,
};

///////////////////////////////////////////
// UYGULAMA I/O PIN LÝSTESÝ
#ifdef _IOS_
GPIO_TypeDef  *GPIO_Ports[] = {
  GPIOA,
  GPIOB,
  GPIOC,
  GPIOD,
  GPIOE,
  GPIOF,
  GPIOG,
};

IO_PIN _ios[] = {
  { GPIOC, 13 },        // IOP_LED
  { GPIOA, 0 },         // IOP_BUTTON
  
  { GPIOB, 15 },        // IOP_DRIVE
};
#else
extern IO_PIN   _ios;
extern GPIO_TypeDef *GPIO_Ports[];
#endif


void IO_Init(int idx, int mode);
void IO_Write(int idx, int val);
int IO_Read(int idx);

#ifdef __cplusplus
}
#endif

#endif
