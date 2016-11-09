#include "scheduler.h"
#include "timeevents.h"
#include "archive.h"
//#include "event.h"

#include <time.h>
#include <string.h>
#include <stdlib.h>

//#pragma pack(1)


bool Schedule_GetEvent( Schedule_s *event );
//void Schedule_PutEvent( const Schedule_s *event );
bool Schedule_IsEvent( time_t time );


Schedule_s schedule[SCHEDULE_BUFFER_SIZE];
volatile uint8_t ScheduleHead = 0;

static time_t DayOffsetTime = 0;
//time_t GetRandomDayOffset(void) {return RandomDayOffset; }


void Scheduler_Init( void )
{
  uint32_t *serial = (uint32_t *)0xFE081F0; 
//  time_t time;
  ScheduleHead = 0;
  //time = __time32(0);
  //srand( time );
  srand(*serial);
  do {
    DayOffsetTime = rand() % ( ( 20*60*60 ) );
  } while( DayOffsetTime < 600 );
}

void Scheduler_Start( void )
{
  //Schedule_PutNextDayStart( DAILY_STATISTIC_EVENT,  __time32(0) );
  //Schedule_PutUniqe( QUALITY_STATISTIC_EVENT,  GetStartNextHour( __time32(0)) );  
  //Schedule_PutUniqe( LD_PROFILE_STATISTIC_EVENT,  GetStartNextHalfHour( __time32(0)) + 1 );
  //Schedule_PutUniqeEventsScatterRndTime( SEND_POST_MSG_EVENT, 60*60, 200, 200 );
  Schedule_PutUniqe( UPDATE_CHECK,   __time32(0) + 10); 
  Schedule_PutUniqeEventsScatterRndTime( SEND_TARIFS_EVENT, 3600*6, 3600, 200 );
}

bool Schedule_PutNextDayRegular( mQueueType_e  event )
{
  return Schedule_PutUniqe( event, GetStartNextDay( __time32(0) ) + DayOffsetTime );
}

bool Schedule_PutNextDayStart( mQueueType_e  event,  time_t  time )
{
  return Schedule_PutUniqe( event,  GetStartNextDay( time ) - 1 );
}

void Scheduler_ClearAll(void)
{
  ScheduleHead = 0;
  memset( &schedule, 0, sizeof(schedule) );
}

void Scheduler(void)
{
  time_t time;
  Schedule_s event;

  if( !ScheduleHead ) return;
  time = __time32(0);
  while( Schedule_IsEvent( time ) ) {
    if( Schedule_GetEvent( &event ) ) {
      if( ( event.event ) ) {
        mQueue_PutMsg(event.event, 0);
      }
    }
  }
}

static int Schedule_cmp (const void* p1, const void* p2)
{
  if ( ((Schedule_s*)p1)->time <  ((Schedule_s*)p2)->time ) return -1;
  if ( ((Schedule_s*)p1)->time == ((Schedule_s*)p2)->time ) return 0;
  //if ( ((Schedule_s*)p1)->time >  ((Schedule_s*)p2)->time ) 
      return 1;
}

bool Schedule_Put(mQueueType_e  event,  time_t  time)
{
  Schedule_s task;
  task.event = event;
  task.time = time;
  return Schedule_PutEvent( &task );
}

bool Schedule_PutUniqe( mQueueType_e  event,  time_t  time )
{
  int i;
  if( ScheduleHead ) {
    for( i=0; i < ScheduleHead; i++ ) {
      if( schedule[i].event == event ) {
        schedule[i].time = time;
        return true;
      }
    }
  }
  return Schedule_Put(event, time);
}
void Schedule_CorrectTime( time_t time )
{
  int i;
  int32_t delta;
  delta = time - __time32(0);

  if( ScheduleHead ) {
    for( i = 0; i < ScheduleHead; i++ ) {
      if( schedule[i].event == MODEM_DROP_STATUS_EVENT
         || schedule[i].event == STRIJ_LED_OFF_EVENT
           || schedule[i].event == DISPLAY_NORMAL_DISPLAY_EVENT
             || schedule[i].event ==  DISPLAY_OFF_EVENT
               || schedule[i].event == DISPLAY_NEXT_EVENT
                || schedule[i].event == SEND_CURR_PARAM_EVENT
                  || schedule[i].event == SEND_DAY_REGULAR_EVENT) {
        schedule[i].time = (time_t)(schedule[i].time + delta);
      }
    }
  }
}
void Schedule_DeleteEvent(mQueueType_e  event)
{
  int i;
  if( ScheduleHead ) {
    for( i=0; i < ScheduleHead; i++ ) {
      if( schedule[i].event == event) {
        if(i < (ScheduleHead-1)) {
          memmove(&schedule[i], &schedule[i+1],  (ScheduleHead-i)*sizeof(Schedule_s) );
        }
        ScheduleHead--;
      }
    }
  }

}
bool Schedule_PutEvent(const Schedule_s *event)
{
  if( ScheduleHead >= SCHEDULE_BUFFER_SIZE ) return false;
  //TODO qsort event by time
  memcpy( &schedule[ScheduleHead], event, sizeof(Schedule_s) );
  ScheduleHead++;
  if( ScheduleHead > 1 )
    qsort ( &schedule[0], ScheduleHead,  sizeof(Schedule_s), &Schedule_cmp );  
  return true;
}

bool Schedule_IsEvent(time_t time)
{
  return ( (time >= schedule[0].time ) && schedule[0].event );
}

bool Schedule_GetEvent(Schedule_s *event)
{
  if( !ScheduleHead ) return false;

  memcpy( event, &schedule[0], sizeof(Schedule_s) );
  memmove( &schedule[0], &schedule[1], ScheduleHead*sizeof(Schedule_s) );
  ScheduleHead--;
  if( !ScheduleHead )    memset( &schedule[0], 0, sizeof(Schedule_s) );
  return true;
}

uint32_t GetRandomWithParametr( uint32_t bound, uint32_t bottom )
{ 
  uint32_t rnd; 

  if( bound > RAND_MAX ) bound = RAND_MAX;

  do {
    rnd = rand() % bound;
  } while( rnd < bottom );
  
  return rnd;
}

// Put Event into sheduller
// where: type - type of event
//        bound - upper bound of random time
//        bottom - limit bottom line of random time
//return false if bound or bottom > RAND_MAX or schedule is full
bool Schedule_PutEventRndTime( mQueueType_e type, uint32_t bound, uint32_t bottom )
{
  if( ( bound > RAND_MAX ) || ( bottom > RAND_MAX ) ) return false;
  return Schedule_PutUniqe( type, __time32(0) + GetRandomWithParametr(bound, bottom) );
}

// Put Event into sheduller
// where: type - type of event
//        bound - upper bound of random time
//        bottom - limit bottom line of random time
//        scatter - minimal scatter of element one value event type
//return false if bound or bottom > RAND_MAX or schedule is full
bool Schedule_PutEventsScatterRndTime( mQueueType_e event, uint32_t bound, uint32_t bottom, uint32_t scatter )
{
  uint32_t i, diff, delta;
  if( !Schedule_Put( event, __time32(0) + GetRandomWithParametr( bound, bottom ) ) ) return false;

  if( ScheduleHead ) {
    delta = 0;    
    for( i = 0; i < ScheduleHead; i++ ) {
      if( ( schedule[i].event == SEND_POST_MSG_EVENT ) || ( schedule[i].event == SEND_DAY_REGULAR_EVENT ) ) {
        if( delta ) {
          if( schedule[i].time >= delta ) {
            diff = schedule[i].time - delta;
            if ( diff  < scatter ) {
             schedule[i].time += ( scatter - diff );
            }
          } else {
            diff = delta - schedule[i].time;
            if ( diff  < scatter ) {
             schedule[i].time = delta + scatter;
            }
          }
        }
        delta = schedule[i].time;
      }
    }
  } 
  return true;
}
bool Schedule_PutUniqeEventsScatterRndTime( mQueueType_e event, uint32_t bound, uint32_t bottom, uint32_t scatter )
{
  uint32_t i, diff, delta;
  if( !Schedule_PutUniqe( event, __time32(0) + GetRandomWithParametr( bound, bottom ) ) ) return false;

  if( ScheduleHead ) {
    delta = 0;    
    for( i = 0; i < ScheduleHead; i++ ) {
      if( ( schedule[i].event == SEND_POST_MSG_EVENT ) || ( schedule[i].event == SEND_DAY_REGULAR_EVENT ) ) {
        if( delta ) {
          if( schedule[i].time >= delta ) {
            diff = schedule[i].time - delta;
            if ( diff  < scatter ) {
             schedule[i].time += ( scatter - diff );
            }
          } else {
            diff = delta - schedule[i].time;
            if ( diff  < scatter ) {
             schedule[i].time = delta + scatter;
            }
          }
        }
        delta = schedule[i].time;
      }
    }
  } 
  return true;
}