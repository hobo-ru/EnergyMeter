#ifndef EEPROM_FT25C64_H
#define EEPROM_FT25C64_H


#include "em_device.h"
#include <stdint.h>
#include <stdbool.h>

#define EEPROM_TOTAL_SIZE_BYTE        8192
#define EEPROM_PAGE_SIZE_BYTE         31


extern void EEPROM_Init(void);
extern void EEPROM_Deinit(void);

extern bool EEPROM_WriteBuffer(uint16_t adress, const uint8_t *buffer, size_t lenght);
extern bool EEPROM_ReadBuffer(uint16_t adress, uint8_t *buffer, size_t lenght);


#endif