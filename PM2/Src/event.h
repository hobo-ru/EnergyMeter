#ifndef _EVENT_H_
#define _EVENT_H_


#include "archive.h"

#include <stdbool.h>
#include <stdint.h>
#include <time.h>



extern bool Event_Send(ArchiveEvent_e event, time_t time);
extern bool Event_PostSend( void );
extern uint8_t Event_GetEventInBuffer(void);


extern size_t Event_GetEventFromBuffer( uint8_t *Buffer, size_t BufferSize );
extern void Event_SetNextEventInBuffer( void );



#endif