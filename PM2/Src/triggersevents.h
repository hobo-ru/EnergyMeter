#ifndef TRIGGERSEVENTS_H
#define TRIGGERSEVENTS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t time_event;
    uint8_t state;
} BUTTON_state;

extern void Triggers_Init(void);
uint8_t CheckPowerLine(void);
void Dispatcher_PowerLine(void);

extern BUTTON_state btn_red;
extern BUTTON_state btn_blue;

uint8_t TestButtonLong(BUTTON_state * btn, uint8_t other_state);

//extern BUTTON_STATES_E BlueButton_GetState( void );
//extern void BlueButton_SetState( BUTTON_STATES_E state );

//extern BUTTON_STATES_E RedButton_GetState( void );
//extern void RedButton_SetState( BUTTON_STATES_E state );

//extern void Button2ResetEvent(void);


#endif