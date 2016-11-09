#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "mqueue.h"
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

//#pragma pack(1)

#define SCHEDULE_BUFFER_SIZE (16)


typedef struct {
  time_t        time;  
  mQueueType_e  event;
}Schedule_s;


extern void Scheduler_Init(void);
extern void Scheduler(void);
extern bool Schedule_PutUniqe(mQueueType_e  event,  time_t  time);
extern bool Schedule_Put(mQueueType_e  event,  time_t  time);
extern bool Schedule_PutEvent(const Schedule_s *event);
extern bool Schedule_PutNextDayStart(mQueueType_e  event,  time_t  time);
extern bool Schedule_PutNextDayRegular(mQueueType_e  event);
extern uint32_t GetRandomWithParametr( uint32_t bound, uint32_t bottom );
//extern bool Schedule_GetEvent(Schedule_s *event);
extern time_t GetRandomDayOffset(void);
extern bool Schedule_PutEventRndTime(mQueueType_e type, uint32_t bound, uint32_t bottom);
extern bool Schedule_PutEventsScatterRndTime(mQueueType_e event, uint32_t bound, uint32_t bottom, uint32_t scatter);
extern bool Schedule_PutUniqeEventsScatterRndTime( mQueueType_e event, uint32_t bound, uint32_t bottom, uint32_t scatter );

extern void Scheduler_ClearAll(void);
extern void Scheduler_Start( void );

extern void Schedule_CorrectTime( time_t time );
extern void Schedule_DeleteEvent(mQueueType_e  event);
#endif
