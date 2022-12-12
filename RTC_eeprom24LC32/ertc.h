#ifndef __ERTC_H
#define __ERTC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned char sec;
  unsigned char min;
  unsigned char hour;
  unsigned char day;
  unsigned char date;
  unsigned char mon;
  unsigned char year;
} DS_DATE;

#define I2C_ADR_DS3231  0xD0

#define I2C_ERTC        I2C_2

void ERTC_Init(void);
int ERTC_GetDate(DS_DATE *dt);
int ERTC_SetDate(DS_DATE *dt);

#ifdef __cplusplus
}
#endif
#endif
