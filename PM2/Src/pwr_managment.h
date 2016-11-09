#ifndef PWR_MANAGMENT_H
#define PWR_MANAGMENT_H


#include <stdbool.h>
#include <stdint.h>
#include <time.h>


typedef enum {
  SLEEP_EM1
  , SLEEP_EM2

}pwr_sleep_depth_e;


extern void PwrManager_Init( void );
extern bool PwrManager_GetStatus( void );
extern void PwrManager_SetStatus( bool status );

//extern void StopPwrSleepDelay( void );
//extern bool StartPwrSleepDelay( time_t time );
extern void EnterPwrSleep( void );
extern void SetDeepPwrSleep( void );
extern void SetTrivialPwrSleep( void );


#endif
