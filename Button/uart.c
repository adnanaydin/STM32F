#include <stdio.h>
#include <stdarg.h>

#include "io.h"
#include "system.h"
#include "uart.h"

#define SZ_PRNBUF       256

typedef struct {
  IO_IDX ioRx;             // RX pin io index
  IO_IDX ioTx;             // TX pin io index

  uint32_t ckUART;      // Clock maske deðeri
  USART_TypeDef *pUSART;        // CMSIS periph. pointer to struct.
} UART_CFG;

static UART_CFG _uCfg[] = {
  { IOP_U1RX, IOP_U1TX, RCC_APB2Periph_USART1, USART1 },
  { IOP_U2RX, IOP_U2TX, RCC_APB1Periph_USART2, USART2 },
  { IOP_U3RX, IOP_U3TX, RCC_APB1Periph_USART3, USART3 },
};

// UART çevresel birimi baþlatýr
// 1. parametre: UART index (0, 1, 2)
// 2. parametre baudrate
// 8-N-1
void UART_Init(UART_PERIPH idx, int baud)
{
  USART_InitTypeDef uInit;
  
  // 1) I/O uçlarý yapýlandýrýlýr
  // RX ucu input floating olmalý
  IO_Init(_uCfg[idx].ioRx, IO_MODE_INPUT);  // input floating    
  
  // TX ucu alternate function push-pull olmalý
  IO_Init(_uCfg[idx].ioTx, IO_MODE_ALTERNATE);
  
  // 2) UART çevresel birim için clock saðlanýr
  if (idx == UART_1)
    // RCC_APB2Periph_USART1
    RCC_APB2PeriphClockCmd(_uCfg[idx].ckUART, ENABLE);
  else
    RCC_APB1PeriphClockCmd(_uCfg[idx].ckUART, ENABLE);
  
  
  // 3) Init yapýsý baþlatýlýr ve çevresel birime uygulanýr
  uInit.USART_BaudRate = baud;
  uInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  uInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  uInit.USART_Parity = USART_Parity_No;
  uInit.USART_StopBits = USART_StopBits_1;
  uInit.USART_WordLength = USART_WordLength_8b;
  
  USART_Init(_uCfg[idx].pUSART, &uInit);
  
  // 4) Çevresel birim aktive edilir (Periph->ON)
  USART_Cmd(_uCfg[idx].pUSART, ENABLE);
}

// UART çevresel birimine veri gönderir
void UART_Send(UART_PERIPH idx, unsigned char val)
{
  // 1) TDR yükleme için uygun mu?
  while (!USART_GetFlagStatus(_uCfg[idx].pUSART, USART_FLAG_TXE)) ;
  
  // 2) Yüklemeyi yap
  // Not: Verinin gitmesini beklemiyoruz, sadece yükleme yapýp çýkýyoruz
  USART_SendData(_uCfg[idx].pUSART, val);
}

// UART çevresel birimine veri gönderir
void UART_Send2(UART_PERIPH idx, unsigned char val)
{
  // 1) Yüklemeyi yap
  // Not: Verinin gitmesini beklemiyoruz, sadece yükleme yapýp çýkýyoruz
  USART_SendData(_uCfg[idx].pUSART, val);

  // 1) Veri gidene kadar bekle
  while (!USART_GetFlagStatus(_uCfg[idx].pUSART, USART_FLAG_TC)) ;  
}

// Belirtilen UART biriminin RDR'sinde veri var mý?
int UART_DataReady(UART_PERIPH idx)
{
    return USART_GetFlagStatus(_uCfg[idx].pUSART, USART_FLAG_RXNE);
}

int UART_Recv(UART_PERIPH idx)
{
  int ret;
  USART_TypeDef *pUART = _uCfg[idx].pUSART;
  
  while (!UART_DataReady(idx)) ;
  
  if (USART_GetFlagStatus(pUART, USART_FLAG_ORE | USART_FLAG_NE
                          | USART_FLAG_FE | USART_FLAG_PE))
  {
    if (USART_GetFlagStatus(pUART, USART_FLAG_NE))
      ret = UART_ERR_NOISE;
    else if (USART_GetFlagStatus(pUART, USART_FLAG_ORE))
      ret = UART_ERR_OVERRUN;
    if (USART_GetFlagStatus(pUART, USART_FLAG_FE))
      ret = UART_ERR_FRAMING;
    if (USART_GetFlagStatus(pUART, USART_FLAG_PE))
      ret = UART_ERR_PARITY;
    
    // Statü register'ýnda hata bitini sýfýrlamak amacýyla
    // boþ okuma yapýyoruz
    USART_ReceiveData(pUART);
  }
  else    
    ret = USART_ReceiveData(pUART);
  
  return ret;
}
                        
void UART_putch(UART_PERIPH idx, unsigned char c)
{
  if (c == '\n')
    UART_Send2(idx, '\r');
  
  UART_Send2(idx, c);
}

// str'deki stringi idx olarak belirtilen UART'a gönderir
// geri dönüþ deðeri: yazýlan karakter sayýsý
int UART_puts(UART_PERIPH idx, const char *str)
{
  int i = 0;
  
  while (str[i])
    UART_putch(idx, str[i++]);
  
  return i;
}

int UART_printf(UART_PERIPH idx, const char *fmt, ...)
{
  va_list args;
  char str[SZ_PRNBUF];
  
  va_start(args, fmt);
  vsnprintf(str, SZ_PRNBUF, fmt, args);
  
  return UART_puts(idx, str);
}



