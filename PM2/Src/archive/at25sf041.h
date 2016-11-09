#ifndef FLASH_AT25SF041_H
#define FLASH_AT25SF041_H



#include "em_device.h"
#include <stdint.h>
#include <stdbool.h>


#define FLASH_TOTAL_SIZE_BYTE          0x7FFFF  //524287
#define FLASH_PAGE_SIZE_BYTE           0xFF
#define FLASH_BLOCK_4K_SIZE_BYTE       0x1000//0xFFF
//#define FLASH_BLOCK_32K_SIZE_BYTE      0x7FFF
//#define FLASH_BLOCK_64K_SIZE_BYTE      0xFFFF



extern void FLASH_Init_SPI( void );
extern void FLASH_Deinit_SPI( void );
extern void FLASH_CreateNew( void );

extern bool FLASH_WriteBuffer( uint32_t adress, const uint8_t *buffer, size_t lenght );
extern bool FLASH_ReadBuffer( uint32_t adress, uint8_t *buffer, size_t lenght );

extern bool FLASH_EraseBlock_32K( uint32_t block );
extern bool FLASH_EraseBlock_64K( uint32_t block );
extern bool FLASH_EraseBlock_4K( uint32_t block );
uint8_t FLASH_ReadByte( uint32_t adress );
bool FLASH_WriteByte( uint32_t adress, uint8_t wbyte );

bool FLASH_GoToDeepSleep();
void FLASH_ResumeFromDeepSleep();
uint8_t FLASH_ReadStatus_1();
uint8_t FLASH_ReadStatus_2();
void FLASH_WE_Volatile_Status();
void FLASH_WriteStatus(uint8_t Reg1, uint8_t Reg2);

uint16_t FLASH_CRC_calc(uint32_t start, uint32_t end);

#endif
