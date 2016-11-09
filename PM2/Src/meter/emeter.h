//emeter.h
#ifndef EMETER_H
#define EMETER_H
#include "pm2_pinout.h"
#include <time.h>



void taskRadioSend (void *pParameters);
void taskDisplay (void *);
void taskTimeMonitor (void *);
void PowerMonitor ();
void taskPowerCount (void *);
void taskPowerCalc (void *);
uint16_t GetTarifFromTime(time_t);
void INPirqSetup (void);
void INPirqEnable (bool);
void DelayNOP (uint32_t);
void ext_write (uint8_t);
uint8_t ext_ctrl (uint8_t, uint8_t);
void ext_init (void);
#define _BV(x) (1<<x)
//#define PW_CALIBRATE

#define NAME_STR(x)	#x

#endif