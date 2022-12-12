#ifndef __IO_H
#define __IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"  

// I/O MODLARI
typedef enum {  
  IO_MODE_INPUT = GPIO_Mode_IN_FLOATING,
  IO_MODE_OUTPUT = GPIO_Mode_Out_PP,
  IO_MODE_ALTERNATE = GPIO_Mode_AF_PP,
  IO_MODE_ANALOG = GPIO_Mode_AIN,
  
  IO_MODE_INPUT_PD = GPIO_Mode_IPD,
  IO_MODE_INPUT_PU = GPIO_Mode_IPU,
  IO_MODE_OUTPUT_OD = GPIO_Mode_Out_OD,
  IO_MODE_ALTERNATE_OD = GPIO_Mode_AF_OD
} IO_MODE;
  
typedef enum {
  IO_PORT_A,
  IO_PORT_B,
  IO_PORT_C,
  IO_PORT_D,
  IO_PORT_E,
  IO_PORT_F,
  IO_PORT_G,
} IO_PORTS;

typedef struct {
  int port;
  int pin;
} IO_PIN;

typedef enum {
  IOP_LED,
  
  // Alfanümerik LCD modül
  IOP_LCD_RS,
  IOP_LCD_E,
  IOP_LCD_DB4,
  IOP_LCD_DB5,
  IOP_LCD_DB6,
  IOP_LCD_DB7,
  
  // UART uçlarý
  IOP_U1RX,
  IOP_U1TX,
  IOP_U2RX,
  IOP_U2TX,
  IOP_U3RX,
  IOP_U3TX,  
  
  // Buttons
  IOP_BTN_SET,
  IOP_BTN_UP,
  IOP_BTN_DN,
} IO_IDX;

///////////////////////////////////////////
// UYGULAMA I/O PIN LÝSTESÝ
#ifdef _IOS_
IO_PIN _ios[] = {
  { IO_PORT_C, 13 },
  
  // Alfanümerik LCD modül
  { IO_PORT_B, 9 },
  { IO_PORT_B, 8 },
  { IO_PORT_B, 7 },
  { IO_PORT_B, 6 },
  { IO_PORT_B, 5 },
  { IO_PORT_B, 4 },  

  // UART uçlarý
  { IO_PORT_A, 10 },
  { IO_PORT_A, 9 },
  { IO_PORT_A, 3 },
  { IO_PORT_A, 2 },
  { IO_PORT_B, 11 },
  { IO_PORT_B, 10 },

  // Buttons
  { IO_PORT_A, 0 },
  { IO_PORT_A, 1 },
  { IO_PORT_A, 2 },
};

GPIO_TypeDef  *GPIO_Ports[] = {
  GPIOA,
  GPIOB,
  GPIOC,
  GPIOD,
  GPIOE,
  GPIOF,
  GPIOG,
};
#else
extern IO_PIN   _ios[];
extern GPIO_TypeDef *GPIO_Ports[];
#endif


void IO_Init(IO_IDX idx, IO_MODE mode);
void IO_Write(IO_IDX idx, int val);
int IO_Read(IO_IDX idx);

#ifdef __cplusplus
}
#endif

#endif
