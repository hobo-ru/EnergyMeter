#include "em_timer.h"
#include "em_cmu.h"
#include "em_wdog.h"

#include "systimer.h"


volatile uint16_t Stime;


uint8_t  callbacks_num;
volatile SysTimer_CALLBACK Sys_Callback[MAXCALLBACKS];
uint16_t Callback_periods[MAXCALLBACKS];


/* 13761 Hz -> 14Mhz (clock frequency) / 1024 (prescaler)
  Setting TOP to 13 results in an overflow each 1 mseconds */
#define TOP 13 //1 ms

void InitSystemTimer(void)
{

  // Enable clock for TIMER0 module 
  CMU_ClockEnable(cmuClock_TIMER0, true);
  
  TIMER_Init_TypeDef timerInit =
  {
    .enable     = true, 
    .debugRun   = true, 
    .prescale   = timerPrescale1024, 
    .clkSel     = timerClkSelHFPerClk, 
    .fallAction = timerInputActionNone, 
    .riseAction = timerInputActionNone, 
    .mode       = timerModeUp, 
    .dmaClrAct  = false,
    .quadModeX4 = false, 
    .oneShot    = false, 
    .sync       = false, 
  };
  
  TIMER_IntEnable(TIMER0, TIMER_IF_OF);
  
  // Enable TIMER0 interrupt vector in NVIC 
  NVIC_EnableIRQ(TIMER0_IRQn);
  
  // Set TIMER Top value 
  TIMER_TopSet(TIMER0, TOP);
  
  // Configure TIMER 
  TIMER_Init(TIMER0, &timerInit);
  
  Stime = 0;
  callbacks_num = 0;
  
  for(uint8_t i = 0; i < MAXCALLBACKS; i++) Sys_Callback[i] = 0;
} 

void TIMER0_IRQHandler(void)
{  
   // Clear flag for TIMER0 overflow interrupt 
  TIMER_IntClear(TIMER0, TIMER_IF_OF);
  Stime++;
  for(uint8_t i = 0; i < callbacks_num; i++)
  {
    if(!(Stime%Callback_periods[i])) 
    {
      if(Sys_Callback[i]) Sys_Callback[i]();
  
    }
  }
}

void DelayMs(unsigned char D)
{
 unsigned int Time;
 Time=Stime;
 while(D) if(Time^Stime) {D--;Time=Stime;}
}

void Delay100Ms(unsigned char D)
{
 while(D--) {DelayMs(100);WDOG_Feed();}
}

void SysTimer_SetCallback(SysTimer_CALLBACK cb, uint16_t period)
{
    Sys_Callback[callbacks_num%MAXCALLBACKS] = cb;
    Callback_periods[callbacks_num%MAXCALLBACKS] = period;
    callbacks_num++;
}

