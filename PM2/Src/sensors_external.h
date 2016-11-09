#ifndef _SENSORS_EXTERNAL_H_
#define _SENSORS_EXTERNAL_H_

#include <stdint.h>

typedef struct {
    uint32_t adc_cover;
    uint32_t adc_body;
    uint32_t hall;
    
    uint8_t state_cover;
    uint8_t state_body;
    uint8_t state_hall;
} SENSORS_EXT_Current;

extern SENSORS_EXT_Current sensors_ext_curr;

void SENSORS_EXT_init();
void SENSORS_EXT_Dispatcher();

#endif
