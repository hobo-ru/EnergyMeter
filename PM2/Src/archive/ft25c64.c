#include "ft25c64.h"

#include "main.h"

#include "em_cmu.h"
#include "em_gpio.h"


#define EEPROM_WRITE_ENABLE      0x6          //Set Write Enable Latch
#define EEPROM_WRITE_DISABLE     0x4          //Reset Write Enable Latch

#define EEPROM_STATUS_REG        0x5          //Read Status Register

#define EEPROM_WRITE_DATA        0x2          //Write Data to Memory Array
#define EEPROM_READ_DATA         0x3          //Read Data from Memory Array

#define EEPROM_READY_STATUS_BIT         0
#define EEPROM_READY_STATUS_MASK        (1<<EEPROM_READY_STATUS_BIT)
#define EEPROM_WEN_STATUS_BIT           1
#define EEPROM_WEN_STATUS_MASK          (1<<EEPROM_WEN_STATUS_BIT)
#define EEPROM_BP0_STATUS_BIT           2
#define EEPROM_BP0_STATUS_MASK          (1<<EEPROM_BP0_STATUS_BIT)
#define EEPROM_BP1_STATUS_BIT           3
#define EEPROM_BP1_STATUS_MASK          (1<<EEPROM_BP1_STATUS_BIT)
#define EEPROM_RES0_STATUS_BIT          4
#define EEPROM_RES0_STATUS_MASK         (1<<EEPROM_RES0_STATUS_BIT)
#define EEPROM_RES1_STATUS_BIT          5
#define EEPROM_RES1_STATUS_MASK         (1<<EEPROM_RES1_STATUS_BIT)
#define EEPROM_RES2_STATUS_BIT          6
#define EEPROM_RES2_STATUS_MASK         (1<<EEPROM_SEC_STATUS_BIT)
#define EEPROM_WPEN_STATUS_BIT          7
#define EEPROM_WPEN_STATUS_MASK         (1<<EEPROM_WPEN_STATUS_BIT)


bool EEPROM_WriteEnable(void) ;
uint8_t EEPROM_ByteIn(void);
void EEPROM_ByteOut(uint8_t Byte);

uint8_t EEPROM_ReadStatus(void) ;
#define EEPROM_IsReady !(EEPROM_ReadStatus() & 0x1)



void EEPROM_Init(void)
{
  //!!!! использует SPI совместно с LCD
  CMU_ClockEnable(cmuClock_GPIO, true);  
  GPIO_PinModeSet (FLASH_CS.port,       FLASH_CS.pin,   /*gpioModeInputPull*/gpioModePushPull,       1);
  GPIO_PinOutSet  (FLASH_CS.port,       FLASH_CS.pin);       // Bring CS high
  GPIO_PinModeSet (FLASH_SCL.port,      FLASH_SCL.pin, 	gpioModePushPull, 	1);  
  GPIO_PinModeSet (FLASH_SI.port,       FLASH_SI.pin, 	gpioModePushPull, 	1);
  GPIO_PinModeSet (FLASH_SO.port,       FLASH_SO.pin,   gpioModeInput,          1);
}

void EEPROM_Deinit(void)
{
  GPIO_PinModeSet (FLASH_CS.port,       FLASH_CS.pin, gpioModeDisabled, 	0);
  GPIO_PinModeSet (FLASH_SI.port,       FLASH_SI.pin, 	gpioModeDisabled, 	0);
  GPIO_PinModeSet (FLASH_SCL.port,      FLASH_SCL.pin, 	gpioModeDisabled, 	0);  
  GPIO_PinModeSet (FLASH_SO.port,       FLASH_SO.pin, 	gpioModeDisabled, 	0);  
  //CMU_ClockEnable(cmuClock_GPIO, false);   
}

bool EEPROM_WriteBuffer(uint16_t adress, const uint8_t *buffer, size_t lenght)
{
  if( adress > EEPROM_TOTAL_SIZE_BYTE) return false;

  if( EEPROM_IsReady ) {
    if( EEPROM_WriteEnable() ) {
      GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
      GPIO_PinOutClear(FLASH_CS.port, FLASH_CS.pin); // Chip select
      
      EEPROM_ByteOut( EEPROM_WRITE_DATA );
      EEPROM_ByteOut((adress>>8) & 0xFF);
      EEPROM_ByteOut(adress & 0xFF);

      for(size_t cnt = 0; cnt < lenght; cnt++) {
        if( ((uint8_t)((adress+cnt)&EEPROM_PAGE_SIZE_BYTE)) == EEPROM_PAGE_SIZE_BYTE ) {
          EEPROM_ByteOut(*buffer++);
          GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
          if( EEPROM_IsReady ) {
              if( EEPROM_WriteEnable() ) {
                GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
                GPIO_PinOutClear(FLASH_CS.port, FLASH_CS.pin); // Chip select
                EEPROM_ByteOut( EEPROM_WRITE_DATA );
                EEPROM_ByteOut(((adress+cnt+1)>>8) & 0xFF);
                EEPROM_ByteOut((adress+cnt+1) & 0xFF);   
              }
          }
        } else EEPROM_ByteOut(*buffer++);
      }
      GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
      return true;  
    }
  }
  return false;
}

bool EEPROM_ReadBuffer(uint16_t adress, uint8_t *buffer, size_t lenght)
{
  if( adress > EEPROM_TOTAL_SIZE_BYTE || !buffer ) return false;

  if( EEPROM_IsReady ) {
    GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
    GPIO_PinOutClear(FLASH_CS.port, FLASH_CS.pin); // Chip select    
    EEPROM_ByteOut( EEPROM_READ_DATA );
    EEPROM_ByteOut((adress>>8) & 0xFF);
    EEPROM_ByteOut(adress & 0xFF);

    while(lenght) {
      *buffer = EEPROM_ByteIn();
      buffer++;
      lenght--;
    }
    GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
    return true;  
  }
  return false;
}

uint8_t EEPROM_ReadByte(uint16_t adress)
{
  uint8_t rbyte;

  if( EEPROM_IsReady ) {
    GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
    GPIO_PinOutClear(FLASH_CS.port, FLASH_CS.pin); // Chip select   
    EEPROM_ByteOut( EEPROM_READ_DATA );
    EEPROM_ByteOut((adress>>8) & 0xFF);
    EEPROM_ByteOut(adress & 0xFF);
    rbyte = EEPROM_ByteIn();
  }
  GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
  return rbyte;
}
bool EEPROM_WriteByte(uint16_t adress, uint8_t wbyte)
{
  if( EEPROM_IsReady ) {
    if( EEPROM_WriteEnable() ) {
      GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
      GPIO_PinOutClear(FLASH_CS.port, FLASH_CS.pin); // Chip select
      
      EEPROM_ByteOut( EEPROM_WRITE_DATA );
      EEPROM_ByteOut((adress>>8) & 0xFF);
      EEPROM_ByteOut(adress & 0xFF);
 
      EEPROM_ByteOut(wbyte);
      GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
      return true;  
    }
  }
  return false;
}


uint8_t EEPROM_ReadStatus(void) 
{
  uint8_t status;
  GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
  GPIO_PinOutClear(FLASH_CS.port, FLASH_CS.pin); // Chip select  
  EEPROM_ByteOut( EEPROM_STATUS_REG );
  status = EEPROM_ByteIn();
  GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
  return status;
}

bool EEPROM_WriteEnable(void) 
{
  GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin);
  GPIO_PinOutClear(FLASH_CS.port, FLASH_CS.pin); // Chip select  

  EEPROM_ByteOut( EEPROM_WRITE_ENABLE );

  if( EEPROM_ReadStatus() & EEPROM_WEN_STATUS_MASK )
    return true;
  return false;

}

//volatile uint32_t edelay;
/********************************************************************
 * Function:        unsigned char bytein(void)
 *
 * Description:     This function inputs a single byte from the
 *                  SPI bus, MSb first.
 *******************************************************************/
uint8_t EEPROM_ByteIn(void)
{
    uint8_t retval = 0;    // Return value
    GPIO_PinOutClear(FLASH_SCL.port, FLASH_SCL.pin);        // Ensure SCK is low          
    for (uint8_t i = 0; i < 8; i++)         // Loop through each bit
    {
        retval = retval << 1;       // Shift byte left for next bit
        GPIO_PinOutSet(FLASH_SCL.port, FLASH_SCL.pin);       // Bring SCK high

        if (GPIO_PinInGet(FLASH_SO.port, FLASH_SO.pin) == 1)                // Check if next bit is a 1
        {
            retval |= 0x01;         // If a 1, set next bit to 1
        }
        else
        {
            retval &= 0xFE;         // If a 0, set next bit to 0
        }
        //edelay = 0xfff;
        //while(edelay) edelay--;
        GPIO_PinOutClear(FLASH_SCL.port, FLASH_SCL.pin);    // Bring SCK low for next bit        
    }

    return retval;
} // end of bytein(void)

/********************************************************************
 * Function:        void byteout(unsigned char byte)
 *
 * Description:     This function outputs a single byte onto the
 *                  SPI bus, MSb first.
 *******************************************************************/
void EEPROM_ByteOut(uint8_t Byte)
{
    for (uint8_t i = 0; i < 8; i++)         // Loop through each bit
    {
        GPIO_PinOutClear(FLASH_SCL.port, FLASH_SCL.pin);                        // Ensure SCK is low      
        if (Byte & 0x80)            // Check if next bit is a 1
        {
           GPIO_PinOutSet(FLASH_SI.port, FLASH_SI.pin);                 // If a 1, pull SO high
        }
        else
        {
            GPIO_PinOutClear(FLASH_SI.port, FLASH_SI.pin);                 // If a 0, pull SO low
        }
       // edelay = 0xfff;
        //while(edelay) edelay--;
        GPIO_PinOutSet(FLASH_SCL.port, FLASH_SCL.pin);                    // Bring SCK high to latch bit
        Byte = Byte << 1;       // Shift byte left for next bit

    }
    GPIO_PinOutClear(FLASH_SCL.port, FLASH_SCL.pin);        // Bring SCK low for next bit    
}

