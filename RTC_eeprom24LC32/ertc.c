#include <stdio.h>
#include <string.h>

#include "io.h"
#include "system.h"
#include "hi2c.h"
#include "ertc.h"

void ERTC_Init(void)
{
  HI2C_Init(I2C_ERTC, 400000);
}

int ERTC_GetDate(DS_DATE *dt)
{
  return HI2C_ReadA(I2C_ERTC, I2C_ADR_DS3231, 0, dt, sizeof(DS_DATE));
}

int ERTC_SetDate(DS_DATE *dt)
{
  return HI2C_WriteA(I2C_ERTC, I2C_ADR_DS3231, 0, dt, sizeof(DS_DATE));
}
