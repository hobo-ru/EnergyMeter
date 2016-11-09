#ifndef __ADE7953_H_
#define __ADE7953_H_

#include "defines_HW.h"
#ifdef DD_ADE7953

#include "em_device.h"

#include <stdint.h>
#include <stdbool.h>
#include "meter.h"

#define ADE_Cal_PHASE_MAX (1+1)

//мгновенные значения для дистанционного считывания
typedef struct {
  float         apower_calc;    // active power
  float         rpower_calc;    // reactive power
  float         aparpower_calc; //apparent power
  float         vrms;		// Voltage RMS
  float         irms;		// Current RMS
  float         pfreq;          // частота сети
  float         angle;          // Angle between I and U

  int32_t       aenergy;
  int32_t       renergy;
  int32_t       apenergy;
}ADE_Inst_Reg;


//структура калибровок(записывается во FLASH)
typedef struct {
  uint32_t   aigain;
  uint32_t   avgain;
  uint32_t   awgain;
  uint32_t   avargain;
  uint32_t   avagain;
  uint32_t   airmsos;
  uint32_t   vrmsos;
  uint32_t   awattos;
  uint32_t   avaros;
  uint32_t   avaos;
  uint32_t   reseved[4];
}ADE_Cal_Reg;


extern ADE_Cal_Reg     ADE_Cal_phase[ADE_Cal_PHASE_MAX];
//extern const  ADE_Reg_s *ADE_GetADE_Reg(void);
//extern ADE_Cal_Reg     ADE_Cal;
extern ADE_Inst_Reg    ADE_Inst;
extern float ADE_alfa;

//extern bool ADE_was_calibrated;
extern int8_t ADE_index_Cal;

void ADE_SPI_Init(void);
void ADE_SPI_Deinit(void);
uint32_t ADE_SPI_ReadReg(uint16_t reg);
void ADE_SPI_WriteReg(uint16_t reg, uint32_t data);


bool ADE_ReadReg();

void ADE_RegsInit(void);

void ADE_Read_Cal();
void ADE_Write_Cal();
void ADE_Set_Default_Cal();
void ADE_Configure_Outs();

//float ADE_data(int32_t val);

//METER_NEW_DATA * METER_new_data();

#endif

#endif
