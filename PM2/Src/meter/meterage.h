#ifndef METERAGE_H
#define METERAGE_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef enum {
  PWR_LIMIT_OFF
  , PWR_LIMIT_ON
  , POWER_OFF
}power_limit_e;

extern bool GetRelayStatus(void);
extern power_limit_e GetPowerLimitSettings(void);
extern power_limit_e GetPowerLimitState(void); 

bool Meterage_ADE(time_t time);
extern void SetPowerLimit(uint32_t limit);

void RepeatRelaySwitch(uint8_t t);
void SwitchRelayOff(void);
void SwitchRelayOn(void);

#endif
