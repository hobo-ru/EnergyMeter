#ifndef METER_ARCHIVE_H
#define METER_ARCHIVE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)
typedef struct {
    uint32_t start_mem;
    uint32_t end_mem;
    uint16_t rec_size;
    uint8_t  type_1;
    uint8_t  type_2;
} ARCH_settings;
#pragma pack(pop)

typedef struct {
    const ARCH_settings settings;
    uint32_t index_head;        // индекс последней записи с данными
    uint32_t index_send;        // индекс последней отправленной записи
    uint32_t sec_before_send;   // врем€ до отправки
} ARCH_control;

void ARCH_upd(ARCH_control * acontrol);
void ARCH_upd_METER_INTEGRATORS_and_RTC(ARCH_control * acontrol);

#ifdef __cplusplus
}
#endif

#endif // METER_ARCHIVE_H
