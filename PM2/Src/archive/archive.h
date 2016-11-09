#ifndef ARCHIVE_H
#define ARCHIVE_H

//#include "tarif.h"
#include "meterage.h"

#include "meter.h"
#include "meter_archive.h"

#include <stdbool.h>
#include <stdint.h>
#include <time.h>
//#pragma pack(1)


#define AT25SF041   1
//#define FT25C64    1

//начальные адреса EEPROM
#if defined(FT25C64) && defined(AT25SF041)
#error "Only AT25SF041 or FT25C64 must be defined !!!"
#elif defined(FT25C64)

#elif  defined(AT25SF041)

#define SETTINGS_ADR                    ( 0x00000 )                     
#define TARIF_ADR                       ( 0x00400 )                     //адрес начала хранения тарифов 
//#define TARIF_TITLE_ADR      (TARIF_ADR  + TARIF_SETTINGS_SIZE)
#define NEW_CAL_ADR                     ( 0x00800 )                     //адрес начала хранения новых калибровок

//#define DAILY_START_ADR                 ( 0x01000 )                     //адрес начала дневных сводок 
//#define QUALITY_START_ADR               ( 0x08000 )                     //адрес начала данных о качестве
//#define LD_PROFILE_START_ADR            ( 0x1F000 )                     //адрес начала профилей нагрузки
//#define EVENT_START_ADR                 ( 0x54000 )                     //адрес начала событий

#define BASE_FIRM_START_ADR             ( 0x60000 )                     //адрес "родной" прошивки
#define UPGRADE_START_ADR               ( 0x70000 )                     //адрес для загрузки обновлений

typedef struct {
  uint16_t      ver;
  uint16_t      start_add;
  uint16_t      end_add;
  uint16_t      crc;
  uint16_t      reserved[2];
  uint16_t      crc_of_this_struct;
}soft_update;

#endif

//Energy type defines
#define ACTIVE_ENERGY           0
#define REACTIVE_ENERGY         1
#define APPARENT_ENERGY         2

typedef struct {
    uint8_t     DTI_address;                // адрес RS485
    uint8_t     Relay_OFF;
    uint8_t     Dev_type;                   // 0 - old 1 - Amper1 2 - Amper3
    uint8_t     Calibration_completed : 1;  // калибровка завершена
    uint8_t     Flash_error           : 1;  // ошибка записи во флеш
    uint8_t     reserve_1             : 6;
    float       Power_limit;
    uint8_t     Pulse0_energy_type;
    uint8_t     Pulse1_energy_type;
    uint16_t    Pulse0_freq;
    uint16_t    Pulse1_freq;
    uint16_t    reserve_2;
    uint32_t    ProtC_ID;                   // id радиомодуля
    uint8_t     Password[10];
    uint16_t    reserve_3;
} settings_s;

extern settings_s       Settings;
extern uint8_t need_send_profile;
//extern uint8_t flash_error; // 1 = error. 0 = no error

extern bool Archive_Init( void );

void Archive_SaveSettings();
bool ArchiveGetDataRecord( uint32_t Adress, uint8_t *Record, uint32_t OneEntrySize );
bool ArchiveGetDataRecordCRC(uint32_t Adress, uint8_t *Record, uint32_t OneEntrySize);

bool checkForUpdate(uint32_t address);
bool copyFirmwareToFlash(uint32_t address);  

void ARCH_save_rec(ARCH_control * acontrol, uint8_t *buffer);
uint8_t * ARCH_load_rec(ARCH_control * acontrol, uint8_t *buffer, uint32_t index);

void ARCH_emit_event_ext(METER_EVENT me, uint32_t timestamp, uint16_t val);
void ARCH_emit_event(METER_EVENT me, uint16_t val);

void METER_event_arch_data(uint32_t data);
void ARCH_update_indexs();

void Archive_Send_Dispatcher();

#endif
