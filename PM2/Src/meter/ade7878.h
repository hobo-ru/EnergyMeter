#ifndef __ADE7878_H_
#define __ADE7878_H_

#include "defines_HW.h"
#ifdef DD_ADE7878

#include "em_device.h"

#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include "meter.h"

//мгновенные значения для дистанционного считывания
typedef struct {
  float         apower_calc;    // active power
  float         rpower_calc;    // reactive power
  float         aparpower_calc; //apparent power
  float         vrms;		// Voltage RMS
  float         irms;		// Current RMS
  float         pfreq;          // частота сети
  float         angle;          // Angle between I and U

  uint32_t      aenergy;
  uint32_t      renergy;
  uint32_t      apenergy;
}ADE_Inst_Reg;
//typedef struct {
////  float         apower_calc;    // active power
////  float         rpower_calc;    // reactive power
////  float         aparpower_calc; //apparent power
////  float         vrms;		// Voltage RMS
////  float         irms;		// Current RMS
////  float         pfreq;          //Регистр периода 
////  float         angle;          // Angle between I and U
//
//  uint32_t      Penergy[PHASE_MAX];
//  uint32_t      Qenergy[PHASE_MAX];
//  uint32_t      Senergy[PHASE_MAX];
//}ADE_Inst_Reg;


//структура калибровок(записывается во FLASH)
typedef struct {
  uint32_t   aigain;    // current gain adjust
  uint32_t   avgain;    // voltage gain adjust
  uint32_t   awgain;    // total active power gain adjust
  uint32_t   avargain;  // total reactive power gain adjust
  uint32_t   avagain;   // apparent power gain adjust
  uint32_t   airmsos;   // current rms offset
  uint32_t   vrmsos;    // voltage rms offset
  uint32_t   awattos;   // total active power offset adjust
  uint32_t   avaros;    // total reactive power offset adjust
  uint32_t   avaos;     // apparent power offset adjust
//  uint32_t   reseved[4];
}ADE_Cal_Reg;   // 40 byte


extern ADE_Cal_Reg     ADE_Cal;
extern ADE_Inst_Reg    ADE_Inst;
extern float ADE_alfa;

//extern bool ADE_was_calibrated;
extern int8_t ADE_index_Cal;

//extern void ADE_Init(void);


extern void ADE_SPI_Init(void);
extern void ADE_SPI_Deinit(void);
extern uint32_t ADE_SPI_ReadReg(uint16_t reg);
extern void ADE_SPI_WriteReg(uint16_t reg, uint32_t data);


//extern uint8_t ADE_Check(void);

extern bool ADE_ReadReg();

extern void ADE_RegsInit(void);

void ADE_rd_Cal_A();
void ADE_wr_Cal_A();
void ADE_rd_Cal_B();
void ADE_wr_Cal_B();
void ADE_rd_Cal_C();
void ADE_wr_Cal_C();
void ADE_rd_Cal_N();
void ADE_wr_Cal_N();

void ADE_Set_Default_Cal();
void ADE_Configure_Outs();

#endif

#endif
