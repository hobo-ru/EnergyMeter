#include "pwr_managment.h"
#include "scheduler.h"
#include "triggersevents.h"

#include <em_emu.h>



static pwr_sleep_depth_e SleepDepth = SLEEP_EM1;
static bool PwrStatus = false;


void PwrManager_Init( void )
{
  PwrStatus = false;
  SleepDepth = SLEEP_EM1;
  
}

bool PwrManager_GetStatus( void )
{
  return PwrStatus;
}
void PwrManager_SetStatus( bool status )
{
  PwrStatus = status;
}


void SetDeepPwrSleep( void )
{
  SleepDepth = SLEEP_EM2;
}
void SetTrivialPwrSleep( void )
{
  SleepDepth = SLEEP_EM1;
}



//void StopPwrSleepDelay( void )
//{
//  if( CheckPowerLine() )    SetTrivialPwrSleep();
//  else SetDeepPwrSleep();
//}

//bool StartPwrSleepDelay( time_t time )
//{
//  if( SleepDepth == SLEEP_EM1 ) return false;
//
//  Schedule_PutUniqe( MODEM_DROP_STATUS_EVENT,  time );
//
//  if( SleepDepth != SLEEP_EM1 )
//    SetTrivialPwrSleep();
//
//  return true;
//}



void EnterPwrSleep( void )
{
  switch(SleepDepth) {
    case SLEEP_EM1:  EMU_EnterEM1();
        break;
    case SLEEP_EM2:  EMU_EnterEM2(false);       break;
  }
}
