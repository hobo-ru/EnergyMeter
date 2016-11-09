#ifndef FLASH_AT25SF041_H
#define FLASH_AT25SF041_H



#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include <stdint.h>
#include <stdbool.h>


#define FLASH_TOTAL_SIZE_BYTE          0x7FFFF  //524287
#define FLASH_PAGE_SIZE_BYTE           0xFF
#define FLASH_BLOCK_4K_SIZE_BYTE       0xFFF
#define FLASH_BLOCK_32K_SIZE_BYTE      0x7FFF
#define FLASH_BLOCK_64K_SIZE_BYTE      0xFFFF


typedef struct {
	GPIO_Port_TypeDef port;
	unsigned int pin;
}gpio_t;


#define FLASH_CS 		((gpio_t){gpioPortC, 15	})
#define FLASH_SI 		((gpio_t){gpioPortC, 1	})      // EEPROM INPUT
#define FLASH_SO 		((gpio_t){gpioPortC, 2	})      // EEEPROM OUTPUT
#define FLASH_SCL 		((gpio_t){gpioPortC, 3	})

extern void FLASH_Init_SPI( void );
extern void FLASH_Deinit_SPI( void );
extern void FLASH_CreateNew( void );

extern bool FLASH_WriteBuffer( uint32_t adress, const uint8_t *buffer, size_t lenght );
extern bool FLASH_ReadBuffer( uint32_t adress, uint8_t *buffer, size_t lenght );

extern bool FLASH_EraseBlock_64K( uint32_t block );
uint8_t FLASH_ReadByte( uint32_t adress );
bool FLASH_WriteByte( uint32_t adress, uint8_t wbyte );

void FLASH_ResumeFromDeepSleep();
uint16_t FLASH_CRC_calc(uint32_t start, uint32_t end);


#endif