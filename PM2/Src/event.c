#include "event.h"
#include "modem.h"
#include "scheduler.h"
#include "electro_v5.h"


#define EVENTS_BUFFER_SIZE 32



typedef struct {
  uint8_t event;
  time_t time;
}event_s;


__data event_s event_buf[EVENTS_BUFFER_SIZE];
uint8_t event_head = 0, event_tail = 0;



bool Event_Send( ArchiveEvent_e event, time_t time )
{
  int i;  
  //Archive_SaveEvent( event,  time );
  //todo add arch

    //put event uniqe
    i = event_tail;
    while( i != event_head ) {
      if( event_buf[i].event == event ) {
        event_buf[i].time = time;
        return true;
      }
      i++;
      if( i >= EVENTS_BUFFER_SIZE )       i = 0;
    }

    event_buf[event_head].event = event;
    event_buf[event_head].time = time;
    event_head++;
    if(event_head >= EVENTS_BUFFER_SIZE)      event_head = 0;

    Schedule_PutUniqe(SEND_POST_MSG_EVENT,  __time32(0));
//    Schedule_PutUniqeEventsScatterRndTime( SEND_POST_MSG_EVENT, 60*10/*60*60*/, 0/*200*/, 180 );

  return true;
}

uint8_t Event_GetEventInBuffer(void)
{
  if(event_head >= event_tail ) return event_head - event_tail;
  else  return ( EVENTS_BUFFER_SIZE - event_tail ) + event_head;
}

size_t Event_GetEventFromBuffer( uint8_t *Buffer, size_t BufferSize )
{
	if(event_tail == event_head) return 0;
	return Electro_MakeEvent( event_buf[event_tail].event, event_buf[event_tail].time, Buffer, BufferSize );
}

void Event_SetNextEventInBuffer( void )
{
	if(event_tail == event_head) return;
	event_tail++;
	if(event_tail >= EVENTS_BUFFER_SIZE)     event_tail = 0;
}
