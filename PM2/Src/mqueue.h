#ifndef M_QUEUE_H
#define M_QUEUE_H


#include <stdbool.h>
#include <stdint.h>



#define MQUEUE_BUFFER_SIZE 32
//#pragma pack(1)




typedef enum {
  NO_EVENT                      = 0
  , RTC_SECOND_EVENT            = 1
  , SCHEDULE_EVENT              = 2
  , MODEM_RECV_EVENT            = 3
  , TIME_SYNC_EVENT             = 4
  , SET_PWR_LIMIT_EVENT         = 5
  , LIMITATION_EVENT            = 6

  , SEND_DAY_REGULAR_EVENT      = 7
  , SEND_CURR_PARAM_EVENT       = 8
  , SEND_POST_MSG_EVENT         = 9
    
  , PARSE_RECV_EVENT            = 10
  , DAILY_STATISTIC_EVENT       = 11
  , QUALITY_STATISTIC_EVENT     = 12
  , LD_PROFILE_STATISTIC_EVENT  = 13

  , PWR_ONLINE_EVENT            = 14
    
  , DISPLAY_EVENT               = 15
  , DISPLAY_NEXT_EVENT          = 16
  , DISPLAY_DRAW_EVENT          = 17
  , DISPLAY_OFF_EVENT           = 18
  , DISPLAY_ON_EVENT            = 19
  , DISPLAY_FULL_DISPLAY_EVENT  = 20
  , DISPLAY_NORMAL_DISPLAY_EVENT= 21
    
  , RED_BUTTON_EVENT            = 22
  , BLUE_BUTTON_EVENT           = 23
  , BLUE_BUTTON_RELEASE_EVENT   = 24
    
  , INIT_COVER_EVENT            = 25
  , INIT_MAGNET_EVENT           = 26
  , DEINIT_COVER_EVENT          = 27
  , DEINIT_MAGNET_EVENT         = 28
  , DEINIT_MAGNET_COVER_EVENT   = 29
    
  , STRIJ_LED_ON_EVENT          = 30
  , STRIJ_LED_SHORT_ON_EVENT    = 31
  , STRIJ_LED_OFF_EVENT         = 32
  
  , MODEM_DROP_STATUS_EVENT     = 33
    
  , AXEM_BUSY_EVENT             = 34
  , AXEM_ASK_EVENT              = 35
  , AXEM_NACK_EVENT             = 36
  , PULSE_CALIBRATION_EVENT     = 37
    
  , WATCH_DOG_EVENT             = 38

  , LEUART_RECV_EVENT           = 39
  , RS_485_CMD_EVENT            = 40
    
  , STORE_NEW_TARIF             = 41
    
  , SEND_TARIFS_EVENT           = 42
  , RELAY_ON                    = 43
  , RELAY_OFF                   = 44
  , SAVE_SETTINGS               = 45
  , REQUEST_ID                  = 46
  , RED_BUTTON_RELEASE_EVENT    = 47
  , SET_FAST_DL                 = 48    
  , RESET_FAST_DL               = 49
  , UPDATE_CHECK                = 50
  , CLEAR_UPDATE_AREA           = 51
//  , SEND_DLACK                  = 52
}mQueueType_e;

typedef struct {
  mQueueType_e type;       //destinator
  uint32_t Data;
}mQueueMsg_t;


extern void mQueue_Init(void);
extern void mQueue_PutMsg(mQueueType_e type, uint32_t data);
extern bool mQueue_GetMsg(mQueueMsg_t *Msg);


#endif
