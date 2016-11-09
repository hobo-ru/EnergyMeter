#ifndef ELECTRO_V5_H
#define ELECTRO_V5_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include "meter.h"

#define EL_V5_ACT_P_TOTAL               0x01       //Накопленная энергия активная прямая (A+) по сумме фаз суммарная по тарифам 
#define EL_V5_ACT_N_TOTAL               0x02       //Накопленная энергия активная обратная (A-) по сумме фаз суммарная по тарифам 
#define EL_V5_REACT_P_TOTAL             0x03       //Накопленная энергия реактивная прямая (R+) по сумме фаз суммарная по тарифам 
#define EL_V5_REACT_N_TOTAL             0x04       //Накопленная энергия реактивная обратная (R-) по сумме фаз суммарная по тарифам 

#define EL_V5_ACT_PN_TOTAL_T1           0x05       //Накопленная энергия активная (A+, A-) по сумме фаз тариф 1 
#define EL_V5_ACT_PN_PH1_T1             0x06       //Накопленная энергия активная (A+, A-) по фазе 1 тариф 1 
#define EL_V5_ACT_PN_PH2_T1             0x07       //Накопленная энергия активная (A+, A-) по фазе 2 тариф 1 
#define EL_V5_ACT_PN_PH3_T1             0x08       //Накопленная энергия активная (A+, A-) по фазе 3 тариф 1 

#define EL_V5_REACT_PN_TOTAL_T1         0x09       //Накопленная энергия реактивная (R+, R-) по сумме фаз тариф 1 
#define EL_V5_REACT_PN_PH1_T1           0x0A      //Накопленная энергия реактивная (R+, R-) по фазе 1 тариф 1 
#define EL_V5_REACT_PN_PH2_T1           0x0B      //Накопленная энергия реактивная (R+, R-) по фазе 2 тариф 1 
#define EL_V5_REACT_PN_PH3_T1           0x0C      //Накопленная энергия реактивная (R+, R-) по фазе 3 тариф 1 

#define EL_V5_ACT_PN_TOTAL_T2           0x0D      //Накопленная энергия активная (A+, A-) по сумме фаз тариф 2 
#define EL_V5_ACT_PN_PH1_T2             0x0E      //Накопленная энергия активная (A+, A-) по фазе 1 тариф 2 
#define EL_V5_ACT_PN_PH2_T2             0x0F      //Накопленная энергия активная (A+, A-) по фазе 2 тариф 2 
#define EL_V5_ACT_PN_PH3_T2             0x10      //Накопленная энергия активная (A+, A-) по фазе 3 тариф 2 

#define EL_V5_REACT_PN_TOTAL_T2         0x11      //Накопленная энергия реактивная (R+, R-) по сумме фаз тариф 2 
#define EL_V5_REACT_PN_PH1_T2           0x12      //Накопленная энергия реактивная (R+, R-) по фазе 1 тариф 2 
#define EL_V5_REACT_PN_PH2_T2           0x13      //Накопленная энергия реактивная (R+, R-) по фазе 2 тариф 2 
#define EL_V5_REACT_PN_PH3_T2           0x14      //Накопленная энергия реактивная (R+, R-) по фазе 3 тариф 2 

#define EL_V5_ACT_PN_TOTAL_T3           0x15      //Накопленная энергия активная (A+, A-) по сумме фаз тариф 3 
#define EL_V5_ACT_PN_PH1_T3             0x16      //Накопленная энергия активная (A+, A-) по фазе 1 тариф 3 
#define EL_V5_ACT_PN_PH2_T3             0x17      //Накопленная энергия активная (A+, A-) по фазе 2 тариф 3 
#define EL_V5_ACT_PN_PH3_T3             0x18      //Накопленная энергия активная (A+, A-) по фазе 3 тариф 3 

#define EL_V5_REACT_PN_TOTAL_T3         0x19      //Накопленная энергия реактивная (R+, R-) по сумме фаз тариф 3 
#define EL_V5_REACT_PN_PH1_T3           0x1A      //Накопленная энергия реактивная (R+, R-) по фазе 1 тариф 3 
#define EL_V5_REACT_PN_PH2_T3           0x1B      //Накопленная энергия реактивная (R+, R-) по фазе 2 тариф 3 
#define EL_V5_REACT_PN_PH3_T3           0x1C      //Накопленная энергия реактивная (R+, R-) по фазе 3 тариф 3 

#define EL_V5_ACT_PN_TOTAL_T4           0x1D      //Накопленная энергия активная (A+, A-) по сумме фаз тариф 4 
#define EL_V5_ACT_PN_PH1_T4             0x1E      //Накопленная энергия активная (A+, A-) по фазе 1 тариф 4 
#define EL_V5_ACT_PN_PH2_T4             0x1F      //Накопленная энергия активная (A+, A-) по фазе 2 тариф 4 
#define EL_V5_ACT_PN_PH3_T4             0x20      //Накопленная энергия активная (A+, A-) по фазе 3 тариф 4 

#define EL_V5_REACT_PN_TOTAL_T4         0x21      //Накопленная энергия реактивная (R+, R-) по сумме фаз тариф 4 
#define EL_V5_REACT_PN_PH1_T4           0x22      //Накопленная энергия реактивная (R+, R-) по фазе 1 тариф 4 
#define EL_V5_REACT_PN_PH2_T4           0x23      //Накопленная энергия реактивная (R+, R-) по фазе 2 тариф 4 
#define EL_V5_REACT_PN_PH3_T4           0x24      //Накопленная энергия реактивная (R+, R-) по фазе 3 тариф 4 

#define EL_V5_POWER_PROFILE             0x30      //Профиль мощности
#define EL_V5_NBFI_DAILY                0x31      //
#define EL_V5_NBFI_POWER_PROFILE        0x32      //Профиль мощности nbfi
#define EL_V5_NBFI_EVENT                0x33      //События

#define EL_V5_DTI_old                   0x3A      //Сообщения по близской связи
#define EL_V5_DTI                       0x3B      //Сообщения по близской связи
#define EL_V5_DTI_echo                  0x3C      //Сообщения по близской связи

#define EL_V5_SET_RADIO_COMMUNICATION   0xEF      //настройки связи
#define EL_V5_SET_TARIFF_EXT            0xF0      //настройки тарифа
#define EL_V5_SET_PROFILE               0xF1      //настройки профиля
#define EL_V5_SEND_TARIF                0xF2      // текущий тариф
#define EL_V5_SET_TARIF                 0xF3      //события 
#define EL_V5_SET_LIMIT                 0xF4      //события 
#define EL_V5_SET_TIME                  0xF5      //события 

#define EL_V5_EVENTS                    0xF6      //события 

#define EL_V5_DELTA_1_PH_TOTAL          0xF7     //Дельта потребления по сумме фаз - первое сообщение 
#define EL_V5_DELTA_2_PH_TOTAL          0xF8     //Дельта потребления по сумме фаз - второе сообщение 
#define EL_V5_DELTA_1_PH_1              0xF9     //Дельта потребления по фазе 1 - первое сообщение  
#define EL_V5_DELTA_2_PH_1              0xFA     //Дельта потребления по фазе 1 - второе сообщение 
#define EL_V5_DELTA_1_PH_2              0xFB     //Дельта потребления по фазе 2 - первое сообщение  
#define EL_V5_DELTA_2_PH_2              0xFC     //Дельта потребления по фазе 2 - второе сообщение 
#define EL_V5_DELTA_1_PH_3              0xFD     //Дельта потребления по фазе 2 - первое сообщение  
#define EL_V5_DELTA_2_PH_3              0xFE     //Дельта потребления по фазе 2 - второе сообщение 

#define EL_V5_HEADER                    0xFF     //Header 

#pragma pack(push, 1)
typedef struct {
    uint8_t timestamp_flag : 1;
    uint8_t obis_C : 7;
    uint8_t rezerv : 1;
    uint8_t tariff : 3;
    uint8_t obis_D : 4;
} PARAM_IDENTIFIER;

typedef struct {
    uint8_t EL_V5;
    uint8_t timeout_hi;
    uint8_t reserv     : 1;
    uint8_t need_send  : 1;
    uint8_t alignment  : 2;
    uint8_t timeout_lo : 4;
    PARAM_IDENTIFIER pi[4];
    uint8_t reserv1;
} V5_SET_PROFILE;

typedef struct {
    uint8_t mask_hi : 4;
    uint8_t header  : 4;
    uint8_t mask_lo;
} V5_TARIFF_HEADER;
#pragma pack(pop)
//typedef enum {
//    NO_ARCH_EVENT                   = 0x00,
//    PROGRAMM_START_ARCH_EVENT       = 0x01,
//    LINE_PWR_ON_ARCH_EVENT          = 0x02,
//    LINE_PWR_OFF_ARCH_EVENT         = 0x03,
////    COVER_OPEN_ARCH_EVENT           = 0x04,
////    COVER_CLOSE_ARCH_EVENT          = 0x05,
////    MAGNET_CLOSER_ARCH_EVENT        = 0x06,
//    PWR_LIMITATION_ON_ARCH_EVENT    = 0x07,
//    PWR_LIMITATION_OFF_ARCH_EVENT   = 0x08,
//    PWR_OFF_ARCH_EVENT              = 0x09,
//    TIME_CORRECTION_ARCH_EVENT      = 0x0A,
//    SET_NEW_TARIF_ARCH_EVENT        = 0x0B,
//    ARCH_EVENT_cover_open           = 0x10,
//    ARCH_EVENT_cover_close          = 0x11,
//    ARCH_EVENT_body_open            = 0x12,
//    ARCH_EVENT_body_close           = 0x13,
//    ARCH_EVENT_magnet_start         = 0x14,
//    ARCH_EVENT_magnet_stop          = 0x15,
//} ArchiveEvent_e;

//TODO delete after debug
extern uint8_t CurrentTarifsCnt;

//extern uint8_t Electro_GetCurrentAddersCnt(void);

extern size_t Electro_MakeEvent( uint8_t event, time_t time,  uint8_t *Buffer, size_t BufSize );
//extern size_t Electro_MakeDayRegular( uint8_t *Buffer, size_t BufferLenght );

//extern size_t Electro_MakeCurrentAdders( uint8_t *Buffer, size_t BufferLenght );
//extern void Electro_SetNextRegularDay( void );
//extern void Electro_SetNextAdders( void );
//extern void Electro_SetNextTarifs( void );

void Electro_MakeTarifs(uint8_t *buf, uint8_t tariff);
void Electro_MakeProfilePower(uint8_t *buf, uint8_t index);

uint8_t Electro_MakeNbfiArch(uint8_t *buf, uint8_t EL_V5, METER_ARCH_DATA_SETTINGS * mads, METER_ARCH_DATA * mad);
uint8_t Electro_MakeNbfiCurr(uint8_t *buf, uint8_t EL_V5, uint32_t timestamp);
uint8_t Electro_MakeNbfiEvent(uint8_t *buf, uint8_t EL_V5, METER_ARCH_EVENT * mae, uint8_t shift);
uint8_t Electro_MakeProfileSettings(uint8_t *buf);

//uint8_t Electro_MakeDayArch(uint8_t *buffer, uint32_t index_record);
//uint8_t Electro_MakeProfileArch(uint8_t *buffer, uint32_t index_record);

#endif
