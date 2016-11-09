#include "meterage.h"
#include "defines_HW.h"
#include "main.h"
#include "archive.h"
#include "mqueue.h"
#include "tariff.h"
#include "meter.h"
#include "systimer.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

static bool EnableRelay = true;
static power_limit_e  PowerLimitSettings = PWR_LIMIT_OFF, PowerLimitState = PWR_LIMIT_OFF;
//static uint32_t RelayLimit = 0;
static float RelayLimit = 0;

inline bool GetRelayStatus ( void )                              {    return EnableRelay;                        }
inline power_limit_e GetPowerLimitSettings( void )               {    return PowerLimitSettings;                 } 
inline power_limit_e GetPowerLimitState( void )                  {    return PowerLimitState;                    }

bool Meterage_ADE(time_t time)
{
  if( PowerLimitSettings == PWR_LIMIT_ON ) 
  {
      if( meter->data->curr.to_display[0].Pi >= RelayLimit ) {
        SwitchRelayOff();
        mQueue_PutMsg(LIMITATION_EVENT, time);
        PowerLimitState = PWR_LIMIT_ON;
      }
  }
  return true;
}

void RepeatRelaySwitch(uint8_t t)
{
    static uint8_t t_try = 0;
    if (t) t_try = t;
    else {
        if (t_try) {
            if (EnableRelay) {
                SwitchRelayOn();
            } else {
                SwitchRelayOff();
            }
            t_try--;
        }
    }
}

void SwitchRelayOn( void )
{
  if(PowerLimitSettings == POWER_OFF) return;
  
  EnableRelay = true;
  gpio_INI_RELAYON;
  DelayMs(100);  //TD_RTC_Delay( 100 );
  gpio_OFF_RELAYON;
  PowerLimitState = PWR_LIMIT_OFF;
}

void SwitchRelayOff( void )
{
  EnableRelay = false;
  gpio_INI_RELAYOFF;
  DelayMs(100);  //TD_RTC_Delay( 100 );
  gpio_OFF_RELAYOFF;
}

void SetPowerLimit( uint32_t limit )
{
  if( !limit )  {
    PowerLimitSettings = PowerLimitState = POWER_OFF;
    SwitchRelayOff();
  } else  if( limit == 0xFFFF ) {
    PowerLimitSettings = PWR_LIMIT_OFF;
    SwitchRelayOn();
  } else {
    PowerLimitSettings = PWR_LIMIT_ON;
    RelayLimit = (float)limit;//(uint32_t)( ((float)limit/3600.0f)*Archive_GetCalibrateSettings()->WattH );
  }
}
