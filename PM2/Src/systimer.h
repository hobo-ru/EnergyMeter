
#ifndef SYSTIMER_H
#define SYSTIMER_H

extern volatile uint16_t Stime;


typedef void    (*SysTimer_CALLBACK)(void);

#define MAXCALLBACKS    10

extern uint8_t  callbacks_num;
extern volatile SysTimer_CALLBACK Sys_Callback[MAXCALLBACKS];
extern uint16_t Callback_periods[MAXCALLBACKS];

void InitSystemTimer(void);
unsigned char SystemTimerInterrupt(void);
void DelayMs(uint8_t);
void Delay100Ms(uint8_t);
void SysTimer_SetCallback(SysTimer_CALLBACK cb, uint16_t period);

#endif
