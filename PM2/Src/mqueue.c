#include "mqueue.h"

#include <string.h>
//#pragma pack(1)

mQueueMsg_t mQueue[MQUEUE_BUFFER_SIZE];
volatile uint8_t mQueueHead = 0, mQueueTail = 0;

void mQueue_Init(void)
{
  mQueueHead = mQueueTail = 0;
}

void mQueue_PutMsg(mQueueType_e type, uint32_t data)
{
  mQueue[mQueueHead].type = type;
  mQueue[mQueueHead].Data = data;
  mQueueHead ++; 
  if(mQueueHead >= MQUEUE_BUFFER_SIZE ) mQueueHead = 0;
}

bool mQueue_GetMsg(mQueueMsg_t *Msg)
{
  uint8_t mH = mQueueHead; // volatile fix
  if( mH == mQueueTail )    return false;

  memcpy(Msg, &mQueue[mQueueTail], sizeof(mQueueMsg_t));
  mQueueTail++;
  if(mQueueTail >= MQUEUE_BUFFER_SIZE)    mQueueTail = 0;
  return true;
}