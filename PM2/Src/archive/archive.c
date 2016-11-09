#include "archive.h"
//#include "adders.h"
#include "defines_HW.h"
#include "mqueue.h"
#include "dti.h"

//#if defined(AT25SF041)
#include "at25sf041.h"
//#elif defined (FT25C64)
//#include "ft25c64.h"
//#endif

#include "meter.h"
#include "modem.h"
//#include "modem_uart.h"

#include "timeevents.h"
#include "meterage.h"
#include "display.h"
#include "pwr_managment.h"
#include "td_rtc.h"
#include "systimer.h"
#include "em_wdog.h"
#include "tariff.h"

#include <stdio.h>
#include <stddef.h>
//#pragma pack(1)

ARCH_control arch_settings[] = {
    {0x01000, 0x15000, sizeof(METER_ARCH_DATA), 0, 0, -1, -1, 0},
    {0x15000, 0x29000, sizeof(METER_ARCH_PROFILE_DATA), 1, 0, -1, -1, 0},
    {0x29000, 0x2B000, sizeof(METER_ARCH_EVENT),0, 1, -1, -1, 0},
};

settings_s              Settings;

uint8_t need_send_profile = 1;

//uint8_t flash_error = 0;
/**
 * Return CRC-8 of the data, using x^8 + x^2 + x + 1 polynomial.  A table-based
 * algorithm would be faster, but for only a few bytes it isn't worth the code
 * size. */
uint8_t Crc8(const void *vptr, int len)
{
	const uint8_t *data = vptr;
	unsigned crc = 0;
	int i, j;
	for (j = len; j; j--, data++) {
		crc ^= (*data << 8);
		for(i = 8; i; i--) {
			if (crc & 0x8000)
				crc ^= (0x1070 << 3);
			crc <<= 1;
		}
	}
	return (uint8_t)(crc >> 8);
}

uint16_t CRC_calc(uint8_t *start, uint8_t *end)
{
  uint16_t crc = 0x0;
  const uint8_t  *data;

  for (data = start; data < end; data++)
  {
    crc  = (crc >> 8) | (crc << 8);
    crc ^= *data;
    crc ^= (crc & 0xff) >> 4;
    crc ^= crc << 12;
    crc ^= (crc & 0xff) << 5;
  }
  return crc;
}

void Archive_SaveSettings()
{
    uint32_t shift;
    FLASH_Init_SPI();
            
    WDOG_Feed();
    
    FLASH_EraseBlock_4K( 0 );
    DelayMs(100);
    FLASH_WriteBuffer( SETTINGS_ADR, (uint8_t*)&Settings, sizeof(Settings) );
    DelayMs(3);
    FLASH_WriteByte( SETTINGS_ADR + sizeof(Settings), Crc8((uint8_t*)&Settings, sizeof(Settings) - 1));
    DelayMs(3);
    FLASH_WriteBuffer( TARIF_ADR, (uint8_t*)&ttable, sizeof(ttable) );
    DelayMs(3);
    FLASH_WriteByte( TARIF_ADR + sizeof(ttable), Crc8((uint8_t*)&ttable, sizeof(ttable) - 1));
    DelayMs(3);

    WDOG_Feed();
//    FLASH_WriteBuffer( NEW_CAL_ADR, (uint8_t*)&creg[0], creg_size );
//    DelayMs(10);
//    FLASH_WriteByte( NEW_CAL_ADR + creg_size, Crc8((uint8_t*)&creg[0], creg_size - 1));
//    DelayMs(10);
    FLASH_WriteBuffer( NEW_CAL_ADR, (uint8_t*)ADE_Cal_phase, sizeof(ADE_Cal_phase) );
    DelayMs(3);
    FLASH_WriteByte( NEW_CAL_ADR + sizeof(ADE_Cal_phase), Crc8((uint8_t*)ADE_Cal_phase, sizeof(ADE_Cal_phase) - 1));
    DelayMs(3);
    // профиль мощности
    shift = NEW_CAL_ADR + sizeof(ADE_Cal_Reg) * 4 + 4;
    FLASH_WriteBuffer( shift, (uint8_t*)&meter->data->aDataSet[1], sizeof(METER_ARCH_DATA_SETTINGS) );
    shift += sizeof(METER_ARCH_DATA_SETTINGS);
    DelayMs(3);
    FLASH_WriteByte( shift, need_send_profile);
    shift += 4;
    FLASH_WriteByte( shift, Crc8((uint8_t*)&meter->data->aDataSet[1], sizeof(METER_ARCH_DATA_SETTINGS) - 1));
    DelayMs(3);
    
//    FLASH_WriteBuffer( NEW_CAL_ADR + sizeof(ADE_Cal_phase) + 1, (uint8_t*)meter->data->aDataSet[1], sizeof(METER_ARCH_DATA_SETTINGS) );
//    DelayMs(10);
//    FLASH_WriteByte( NEW_CAL_ADR + sizeof(ADE_Cal_phase) + 1 + sizeof(METER_ARCH_DATA_SETTINGS),
//                    Crc8((uint8_t*)meter->data->aDataSet[1], sizeof(METER_ARCH_DATA_SETTINGS) - 1));
    
    // отправка сообщения после завершения калибровок
    static uint8_t Settings_Calibration_completed_old = 0;
    if (Settings.Calibration_completed && !Settings_Calibration_completed_old) {
        Schedule_PutUniqe(SEND_CURR_PARAM_EVENT, __time32(0) + 5);
    }
    Settings_Calibration_completed_old = Settings.Calibration_completed;
}

bool ArchiveGetDataRecord( uint32_t Adress, uint8_t *Record, uint32_t OneEntrySize )
{
    if(FLASH_ReadBuffer(Adress, Record, OneEntrySize)) {
        if( Record[OneEntrySize - 1] == Crc8(Record, OneEntrySize - 2) ) {
            return true;
        }
    }
    return false;
}

bool ArchiveGetDataRecordCRC( uint32_t Adress, uint8_t *Record, uint32_t OneEntrySize )
{
    if(FLASH_ReadBuffer(Adress, Record, OneEntrySize)) {
        if( FLASH_ReadByte(Adress + OneEntrySize) == Crc8(Record, OneEntrySize - 1) ) {
            return true;
        }
    }
    return false;
}

bool Archive_Init( void )
{ 
    uint32_t shift;
    FLASH_Init_SPI(); 
    FLASH_ResumeFromDeepSleep();
  
    if(!ArchiveGetDataRecordCRC( SETTINGS_ADR, (uint8_t*)&Settings, sizeof(Settings) ) || (Settings.Password[0] == 0)) {
        memset(&Settings, 0, sizeof(Settings));
        Settings.DTI_address = 0;
        Settings.Power_limit = 0; //no limit
        Settings.Relay_OFF = 0; //no power off
        Settings.Pulse0_energy_type = ACTIVE_ENERGY;
        Settings.Pulse1_energy_type = ACTIVE_ENERGY;
        Settings.Pulse0_freq = 1000;
        Settings.Pulse1_freq = 1000;
        memcpy( Settings.Password, "strij", sizeof("strij"));

        ADE_Set_Default_Cal();

        Archive_SaveSettings();
        if(!ArchiveGetDataRecordCRC( SETTINGS_ADR, (uint8_t*)&Settings, sizeof(Settings) )) {
            Settings.Flash_error = 1;
        }
    }
#ifdef DD_ADE7953    
    Settings.Dev_type = 1;
#endif
#ifdef DD_ADE7868A    
    Settings.Dev_type = 2;
#endif
    if (!ArchiveGetDataRecordCRC( TARIF_ADR, (uint8_t*)&ttable, sizeof(TARIFF_2hour) * TARIFF_TABLE_ITEMS) ) {
        TARIFF_init();
    }
    
    shift = NEW_CAL_ADR + sizeof(ADE_Cal_Reg) * 4 + 4;
    METER_ARCH_DATA_SETTINGS mads;
    if (FLASH_ReadBuffer(shift, (uint8_t*)&mads, sizeof(METER_ARCH_DATA_SETTINGS))) {
        if( FLASH_ReadByte(shift + sizeof(METER_ARCH_DATA_SETTINGS) + 4) == Crc8((uint8_t*)&mads, sizeof(METER_ARCH_DATA_SETTINGS) - 1) ) {
            memmove(&meter->data->aDataSet[1], &mads, sizeof(METER_ARCH_DATA_SETTINGS));
        }
    }
    return true;
}

bool checkForUpdate(uint32_t address) 
{
  soft_update s_up; 
  
  FLASH_Init_SPI();
  
  FLASH_ReadBuffer(address  , (uint8_t*)&s_up, sizeof(s_up)); //чтение структуры описывающей обновление
  
  if(CRC_calc((void *)(&s_up), (void *)(&s_up.crc_of_this_struct)) != s_up.crc_of_this_struct) return false; 
  
  if(CRC_calc((void *)s_up.start_add,(void *)s_up.end_add) == s_up.crc) return false;  //обновление соответствует текущей версии
  
  if(FLASH_CRC_calc(address + s_up.start_add, address + s_up.end_add) != s_up.crc)  return false;// у обновления не сходится CRC
  
  return true;
}

bool copyFirmwareToFlash(uint32_t address)
{
  soft_update s_up; 
  
  FLASH_Init_SPI();
  
  FLASH_ReadBuffer(address  , (uint8_t*)&s_up, sizeof(s_up)); //чтение структуры описывающей прошивку
  
  if(CRC_calc((void *)(&s_up), (void *)(&s_up.crc_of_this_struct)) == s_up.crc_of_this_struct) return false; 
  
  FLASH_EraseBlock_64K(address);
  
  s_up.start_add = 0x800;
  s_up.end_add = 0xff00;
  s_up.ver = DTI_info.software_rev*256 + DTI_info.software_subrev;
  s_up.crc =   CRC_calc((void *)s_up.start_add,(void *)s_up.end_add);
  s_up.crc_of_this_struct = CRC_calc((void *)(&s_up), (void *)(&s_up.crc_of_this_struct));
  
  FLASH_WriteBuffer(address, (uint8_t*)&s_up, sizeof(s_up));
  
  uint8_t mas[64];
  
  for(uint32_t i = s_up.start_add; i < s_up.end_add; i += 64)
  {
  
    const uint8_t  *data = (void *)(i);
    for(uint32_t j = 0; j < 64; j++) 
    {
      mas[j] = *data; 
      data++;
    }
    
    FLASH_WriteBuffer(address + i, mas, 64);
    uint32_t delay = 5000;
    while(delay--);
    WDOG_Feed();
    
  }
  return true;
}
 
// ===========================================================================
uint8_t ARCH_crc8(const void *vptr, int len)
{
    const uint8_t *data = (uint8_t *)vptr;
    unsigned crc = 0;
    int i, j;
    for (j = len; j; j--, data++) {
        crc ^= (*data << 8);
        for(i = 8; i; i--) {
            if (crc & 0x8000)
                crc ^= (0x1070 << 3);
            crc <<= 1;
        }
    }
    return (uint8_t)(crc >> 8);
}

uint32_t ARCH_index_to_block(ARCH_control * acontrol, uint32_t index)
{
    return (index * acontrol->settings.rec_size + acontrol->settings.start_mem) / FLASH_BLOCK_4K_SIZE_BYTE;
}

uint32_t ARCH_max_count(ARCH_control * acontrol)
{
    return (acontrol->settings.end_mem - acontrol->settings.start_mem) / acontrol->settings.rec_size;
}

void ARCH_save_rec(ARCH_control * acontrol, uint8_t *buffer)
{
    if (acontrol->settings.start_mem % FLASH_BLOCK_4K_SIZE_BYTE || acontrol->settings.end_mem % FLASH_BLOCK_4K_SIZE_BYTE) { while(1); }// assert
    uint8_t flag_clear = 0;
    if (acontrol->index_head++ == (uint32_t)-1) {
        flag_clear = 1;
    } else
    if (acontrol->index_head >= ARCH_max_count(acontrol)) {
        acontrol->index_head = 0;
        flag_clear = 1;
    } else
    if (ARCH_index_to_block(acontrol, acontrol->index_head + 0) != ARCH_index_to_block(acontrol, acontrol->index_head + 1)) {
        flag_clear = 1;
    }
    FLASH_Init_SPI();
    WDOG_Feed();
    if (flag_clear) {
        FLASH_EraseBlock_4K(ARCH_index_to_block(acontrol, acontrol->index_head + 1) * FLASH_BLOCK_4K_SIZE_BYTE);
        DelayMs(10);
    }
    buffer[acontrol->settings.rec_size - 1] = ARCH_crc8(buffer, acontrol->settings.rec_size - 1);
    FLASH_WriteBuffer(acontrol->index_head * acontrol->settings.rec_size + acontrol->settings.start_mem, buffer, acontrol->settings.rec_size);
}

uint8_t * ARCH_load_rec(ARCH_control * acontrol, uint8_t *buffer, uint32_t index)
{
    uint32_t addr;
    if (index > (acontrol->settings.end_mem - acontrol->settings.start_mem) / acontrol->settings.rec_size) {
        return 0;
    }
    addr = index * acontrol->settings.rec_size + acontrol->settings.start_mem;
    FLASH_Init_SPI();
    WDOG_Feed();
    if (!FLASH_ReadBuffer(addr, buffer, acontrol->settings.rec_size)) {
        return 0;
    }
    if (buffer[acontrol->settings.rec_size - 1] != ARCH_crc8(buffer, acontrol->settings.rec_size - 1) ) {
        return 0;
    }
    return buffer;
}

void METER_event_arch_data(uint32_t data)
{
    if (data >= 2) return;
    //if (!arch->save) return;
    ARCH_save_rec(&arch_settings[data], (uint8_t*)&meter->data->aData[data]);
    
//    //todo send
//    if (data == 1) { // профиль мощности
//        uint8_t drbuffer[8];
//        uint8_t index;
//        //ModemState = MODEM_SEND_PAYLOAD;
//        for (index = 0; index < 4; index++) {
//            Electro_MakeProfilePower(drbuffer, index);
//            ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, drbuffer, sizeof(drbuffer));
//        }
//    }
//    //ARCH_emit_event(ME_power_on);
}

void ARCH_emit_event_ext(METER_EVENT me, uint32_t timestamp, uint16_t val)
{
    ARCH_control *ac = &arch_settings[2];
    METER_ARCH_EVENT mae;
    mae.timestamp = timestamp;
    mae.event = me;
    mae.value = val;
//    memmove(&mae.reserve[0], &val, sizeof(uint16_t));
    //if (ac->settings.rec_size != sizeof(mae)) { while(1); }// assert
    
    //if (!arch->save) return;
    ARCH_save_rec(ac, (uint8_t*)&mae);
}

void ARCH_emit_event(METER_EVENT me, uint16_t val)
{
    ARCH_emit_event_ext(me, NOW, val);
    
//    switch (me) {
//    case ME_btn_cover_up:               Event_Send( ARCH_EVENT_cover_open,  __time32(0) ); break;
//    case ME_btn_cover_down:             Event_Send( ARCH_EVENT_cover_close, __time32(0) ); break;
//    case ME_btn_body_up:                Event_Send( ARCH_EVENT_body_open,   __time32(0) ); break;
//    case ME_btn_body_down:              Event_Send( ARCH_EVENT_body_close,  __time32(0) ); break;
//    case ME_over_magnetic_field_start:  Event_Send( ARCH_EVENT_magnet_start,__time32(0) ); break;
//    case ME_over_magnetic_field_stop:   Event_Send( ARCH_EVENT_magnet_stop, __time32(0) ); break;
//    default :break;
//    }
}

void ARCH_clear(ARCH_control * acontrol)
{
    int i;
    int block_count = (acontrol->settings.end_mem - acontrol->settings.start_mem) / FLASH_BLOCK_4K_SIZE_BYTE;
    for (i = 0; i < block_count; i++) {
        FLASH_EraseBlock_4K(acontrol->settings.start_mem + i * FLASH_BLOCK_4K_SIZE_BYTE);
    }
}

void ARCH_update_indexs()
{
//    ARCH_clear(&arch_settings[0]);
//    ARCH_clear(&arch_settings[1]);
//    ARCH_clear(&arch_settings[2]);
//    while(1);
    
    ARCH_upd(&arch_settings[0]); 
    ARCH_upd(&arch_settings[1]); 
    ARCH_upd(&arch_settings[2]); 
    
    ARCH_upd_METER_INTEGRATORS_and_RTC(arch_settings);
}

void Archive_Send(uint8_t arch_index)
{
    uint8_t buf[256];
    uint8_t buf_arh[128];
    uint8_t buf_len = 0;
    ARCH_control * acontrol = &arch_settings[arch_index];
    switch(arch_index) {
    case 0: {
        if (++acontrol->index_send >= ARCH_max_count(acontrol)) {
            acontrol->index_send = 0;
        }
        METER_ARCH_DATA_SETTINGS * mads = &meter->data->aDataSet[arch_index];
        if (ARCH_load_rec(acontrol, (uint8_t*)buf_arh, acontrol->index_send)) {
            buf_len = Electro_MakeNbfiArch(buf, EL_V5_NBFI_DAILY, mads, (METER_ARCH_DATA *)buf_arh);
        }
        if (buf_len) ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, buf, buf_len);
        break;
    }
    case 1: {
        if (++acontrol->index_send >= ARCH_max_count(acontrol)) {
            acontrol->index_send = 0;
        }
        METER_ARCH_DATA_SETTINGS * mads = &meter->data->aDataSet[arch_index];
        if (ARCH_load_rec(acontrol, (uint8_t*)buf_arh, acontrol->index_send)) {
            buf_len = Electro_MakeNbfiArch(buf, EL_V5_NBFI_POWER_PROFILE, mads, (METER_ARCH_DATA *)buf_arh);
        }
        if (buf_len) ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, buf, buf_len);
        break;
    }
    case 2: {
//        uint32_t index_send = (acontrol->index_send+1) % ARCH_max_count(acontrol);
        while (acontrol->index_head != acontrol->index_send && buf_len < (128 - 8)) {
            if (++acontrol->index_send >= ARCH_max_count(acontrol)) {
                acontrol->index_send = 0;
            }
            if (ARCH_load_rec(acontrol, (uint8_t*)buf_arh, acontrol->index_send)) {
                buf_len = Electro_MakeNbfiEvent(buf, EL_V5_NBFI_EVENT, (METER_ARCH_EVENT *)buf_arh, buf_len);
            }
        }
        if (buf_len) ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, buf, buf_len);
        break;
    }
    default: break;
    }
}

uint8_t Archive_Send_Dispatcher_test_arch(uint8_t arch_index)
{
    ARCH_control * acontrol = &arch_settings[arch_index];
//    uint32_t index_send = (acontrol->index_send+1) % ARCH_max_count(acontrol);
    if (acontrol->index_head == acontrol->index_send) return 0;
    if (!acontrol->sec_before_send) {
        switch(arch_index) {
        case 0: acontrol->sec_before_send = GetRandomWithParametr((60 * 60), 0); break;
        case 1: acontrol->sec_before_send = GetRandomWithParametr((meter->data->aDataSet[1].timeout) >> 1, 0); break;
        case 2: acontrol->sec_before_send = GetRandomWithParametr((60 * 1), 0); break;
        default: break;
        }
        return 1;
    } else {
        if (--acontrol->sec_before_send) {
            return 1;
        }
    }
    Archive_Send(arch_index);
    return 1;
}

void Archive_Send_Dispatcher()
{
    if (DL_Mode == SLIP_FASTDL) return;
    if (AXSEM_packets_to_send) return; // есть данные в буфере отправки
    if (Archive_Send_Dispatcher_test_arch(0)) return;
    if (Archive_Send_Dispatcher_test_arch(2)) return;
    if (need_send_profile) {
        if (Archive_Send_Dispatcher_test_arch(1)) return;
    } else {
        arch_settings[1].index_send = arch_settings[1].index_head;
    }
}
