#ifndef ELECTRO_V5_H
#define ELECTRO_V5_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include "meter.h"

#define EL_V5_ACT_P_TOTAL               0x01       //����������� ������� �������� ������ (A+) �� ����� ��� ��������� �� ������� 
#define EL_V5_ACT_N_TOTAL               0x02       //����������� ������� �������� �������� (A-) �� ����� ��� ��������� �� ������� 
#define EL_V5_REACT_P_TOTAL             0x03       //����������� ������� ���������� ������ (R+) �� ����� ��� ��������� �� ������� 
#define EL_V5_REACT_N_TOTAL             0x04       //����������� ������� ���������� �������� (R-) �� ����� ��� ��������� �� ������� 

#define EL_V5_ACT_PN_TOTAL_T1           0x05       //����������� ������� �������� (A+, A-) �� ����� ��� ����� 1 
#define EL_V5_ACT_PN_PH1_T1             0x06       //����������� ������� �������� (A+, A-) �� ���� 1 ����� 1 
#define EL_V5_ACT_PN_PH2_T1             0x07       //����������� ������� �������� (A+, A-) �� ���� 2 ����� 1 
#define EL_V5_ACT_PN_PH3_T1             0x08       //����������� ������� �������� (A+, A-) �� ���� 3 ����� 1 

#define EL_V5_REACT_PN_TOTAL_T1         0x09       //����������� ������� ���������� (R+, R-) �� ����� ��� ����� 1 
#define EL_V5_REACT_PN_PH1_T1           0x0A      //����������� ������� ���������� (R+, R-) �� ���� 1 ����� 1 
#define EL_V5_REACT_PN_PH2_T1           0x0B      //����������� ������� ���������� (R+, R-) �� ���� 2 ����� 1 
#define EL_V5_REACT_PN_PH3_T1           0x0C      //����������� ������� ���������� (R+, R-) �� ���� 3 ����� 1 

#define EL_V5_ACT_PN_TOTAL_T2           0x0D      //����������� ������� �������� (A+, A-) �� ����� ��� ����� 2 
#define EL_V5_ACT_PN_PH1_T2             0x0E      //����������� ������� �������� (A+, A-) �� ���� 1 ����� 2 
#define EL_V5_ACT_PN_PH2_T2             0x0F      //����������� ������� �������� (A+, A-) �� ���� 2 ����� 2 
#define EL_V5_ACT_PN_PH3_T2             0x10      //����������� ������� �������� (A+, A-) �� ���� 3 ����� 2 

#define EL_V5_REACT_PN_TOTAL_T2         0x11      //����������� ������� ���������� (R+, R-) �� ����� ��� ����� 2 
#define EL_V5_REACT_PN_PH1_T2           0x12      //����������� ������� ���������� (R+, R-) �� ���� 1 ����� 2 
#define EL_V5_REACT_PN_PH2_T2           0x13      //����������� ������� ���������� (R+, R-) �� ���� 2 ����� 2 
#define EL_V5_REACT_PN_PH3_T2           0x14      //����������� ������� ���������� (R+, R-) �� ���� 3 ����� 2 

#define EL_V5_ACT_PN_TOTAL_T3           0x15      //����������� ������� �������� (A+, A-) �� ����� ��� ����� 3 
#define EL_V5_ACT_PN_PH1_T3             0x16      //����������� ������� �������� (A+, A-) �� ���� 1 ����� 3 
#define EL_V5_ACT_PN_PH2_T3             0x17      //����������� ������� �������� (A+, A-) �� ���� 2 ����� 3 
#define EL_V5_ACT_PN_PH3_T3             0x18      //����������� ������� �������� (A+, A-) �� ���� 3 ����� 3 

#define EL_V5_REACT_PN_TOTAL_T3         0x19      //����������� ������� ���������� (R+, R-) �� ����� ��� ����� 3 
#define EL_V5_REACT_PN_PH1_T3           0x1A      //����������� ������� ���������� (R+, R-) �� ���� 1 ����� 3 
#define EL_V5_REACT_PN_PH2_T3           0x1B      //����������� ������� ���������� (R+, R-) �� ���� 2 ����� 3 
#define EL_V5_REACT_PN_PH3_T3           0x1C      //����������� ������� ���������� (R+, R-) �� ���� 3 ����� 3 

#define EL_V5_ACT_PN_TOTAL_T4           0x1D      //����������� ������� �������� (A+, A-) �� ����� ��� ����� 4 
#define EL_V5_ACT_PN_PH1_T4             0x1E      //����������� ������� �������� (A+, A-) �� ���� 1 ����� 4 
#define EL_V5_ACT_PN_PH2_T4             0x1F      //����������� ������� �������� (A+, A-) �� ���� 2 ����� 4 
#define EL_V5_ACT_PN_PH3_T4             0x20      //����������� ������� �������� (A+, A-) �� ���� 3 ����� 4 

#define EL_V5_REACT_PN_TOTAL_T4         0x21      //����������� ������� ���������� (R+, R-) �� ����� ��� ����� 4 
#define EL_V5_REACT_PN_PH1_T4           0x22      //����������� ������� ���������� (R+, R-) �� ���� 1 ����� 4 
#define EL_V5_REACT_PN_PH2_T4           0x23      //����������� ������� ���������� (R+, R-) �� ���� 2 ����� 4 
#define EL_V5_REACT_PN_PH3_T4           0x24      //����������� ������� ���������� (R+, R-) �� ���� 3 ����� 4 

#define EL_V5_POWER_PROFILE             0x30      //������� ��������
#define EL_V5_NBFI_DAILY                0x31      //
#define EL_V5_NBFI_POWER_PROFILE        0x32      //������� �������� nbfi
#define EL_V5_NBFI_EVENT                0x33      //�������

#define EL_V5_DTI_old                   0x3A      //��������� �� �������� �����
#define EL_V5_DTI                       0x3B      //��������� �� �������� �����
#define EL_V5_DTI_echo                  0x3C      //��������� �� �������� �����

#define EL_V5_SET_RADIO_COMMUNICATION   0xEF      //��������� �����
#define EL_V5_SET_TARIFF_EXT            0xF0      //��������� ������
#define EL_V5_SET_PROFILE               0xF1      //��������� �������
#define EL_V5_SEND_TARIF                0xF2      // ������� �����
#define EL_V5_SET_TARIF                 0xF3      //������� 
#define EL_V5_SET_LIMIT                 0xF4      //������� 
#define EL_V5_SET_TIME                  0xF5      //������� 

#define EL_V5_EVENTS                    0xF6      //������� 

#define EL_V5_DELTA_1_PH_TOTAL          0xF7     //������ ����������� �� ����� ��� - ������ ��������� 
#define EL_V5_DELTA_2_PH_TOTAL          0xF8     //������ ����������� �� ����� ��� - ������ ��������� 
#define EL_V5_DELTA_1_PH_1              0xF9     //������ ����������� �� ���� 1 - ������ ���������  
#define EL_V5_DELTA_2_PH_1              0xFA     //������ ����������� �� ���� 1 - ������ ��������� 
#define EL_V5_DELTA_1_PH_2              0xFB     //������ ����������� �� ���� 2 - ������ ���������  
#define EL_V5_DELTA_2_PH_2              0xFC     //������ ����������� �� ���� 2 - ������ ��������� 
#define EL_V5_DELTA_1_PH_3              0xFD     //������ ����������� �� ���� 2 - ������ ���������  
#define EL_V5_DELTA_2_PH_3              0xFE     //������ ����������� �� ���� 2 - ������ ��������� 

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
