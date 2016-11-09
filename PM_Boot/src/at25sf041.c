#include "at25sf041.h"



#define FLASH_READ_STATUS_1             0x5          //Read Status Register
#define FLASH_READ_STATUS_2             0x35          //Read Status Register

#define FLASH_READ_DATA                 0x3          //Read Data from Memory Array


#define FLASH_CHIP_ERASE                0xC7
#define FLASH_BLOCK_4K_ERASE            0x20
#define FLASH_BLOCK_32K_ERASE           0x52
#define FLASH_BLOCK_64K_ERASE           0xD8

#define FLASH_WRITE_DATA                0x2          //Write Data to Memory Array
#define FLASH_WRITE_ENABLE              0x6          //Set Write Enable Latch
#define FLASH_WRITE_DISABLE             0x4          //Set Write Disable Latch
#define FLASH_WRITE_STATUS_REG          0x1
#define FLASH_WRITE_VOLATILE_STATUS     0x50

#define FLASH_READ_SECUR_PAGE           0x48
#define FLASH_ERASE_SECUR_PAGE          0x44
#define FLASH_WRITE_SECUR_PAGE          0x42

#define FLASH_READ_ID                   0x90
#define FLASH_DEEP_POWER_DOWN           0xB9
#define FLASH_RESUME_POWER_UP           0xAB


#define FLASH_READY_STATUS_BIT          0
#define FLASH_READY_STATUS_MASK         (1<<FLASH_READY_STATUS_BIT)
#define FLASH_WEL_STATUS_BIT            1
#define FLASH_WEL_STATUS_MASK           (1<<FLASH_WEL_STATUS_BIT)
#define FLASH_BP0_STATUS_BIT            2
#define FLASH_BP0_STATUS_MASK           (1<<FLASH_BP0_STATUS_BIT)
#define FLASH_BP1_STATUS_BIT            3
#define FLASH_BP1_STATUS_MASK           (1<<FLASH_BP1_STATUS_BIT)
#define FLASH_BP2_STATUS_BIT            4
#define FLASH_BP2_STATUS_MASK           (1<<FLASH_BP2_STATUS_BIT)
#define FLASH_TB_STATUS_BIT             5
#define FLASH_TB_STATUS_MASK            (1<<FLASH_TB_STATUS_BIT)
#define FLASH_SEC_STATUS_BIT            6
#define FLASH_SEC_STATUS_MASK           (1<<FLASH_SEC_STATUS_BIT)
#define FLASH_SRP0_STATUS_BIT           7
#define FLASH_SRP0_STATUS_MASK          (1<<FLASH_SRP0_STATUS_BIT)


#define FLASH_RES7_STATUS_BIT          7
#define FLASH_RES7_STATUS_MASK         (1<<FLASH_RES7_STATUS_BIT)
#define FLASH_CMP_STATUS_BIT           6
#define FLASH_CMP_STATUS_MASK          (1<<FLASH_CMP_STATUS_BIT)
#define FLASH_LB3_STATUS_BIT           5
#define FLASH_LB3_STATUS_MASK          (1<<FLASH_LB3_STATUS_BIT)
#define FLASH_LB2_STATUS_BIT           4
#define FLASH_LB2_STATUS_MASK          (1<<FLASH_LB2_STATUS_BIT)
#define FLASH_LB1_STATUS_BIT           3
#define FLASH_LB1_STATUS_MASK          (1<<FLASH_LB1_STATUS_BIT)
#define FLASH_RES2_STATUS_BIT          2
#define FLASH_RES2_STATUS_MASK         (1<<FLASH_RES2_STATUS_BIT)
#define FLASH_QE_STATUS_BIT            1
#define FLASH_QE_STATUS_MASK           (1<<FLASH_QE_STATUS_BIT)
#define FLASH_SRP1_STATUS_BIT          0
#define FLASH_SRP1_STATUS_MASK         (1<<FLASH_SRP1_STATUS_BIT)



bool FLASH_WriteEnable( void );
bool FLASH_WriteDisable( void );
uint8_t FLASH_ByteIn( void );
void FLASH_ByteOut( uint8_t Byte );
uint8_t FLASH_ReadStatus_1( void );
uint8_t FLASH_ReadStatus_2( void );
bool FLASH_EraseChip( void );
bool FLASH_GetReady( void );

bool FLASH_IsReady( void )           {    return !( FLASH_ReadStatus_1() & FLASH_READY_STATUS_MASK ); }
//bool FLASH_IsWriteEnable( void )     {    return ( FLASH_ReadStatus_1() & FLASH_WEL_STATUS_MASK );    }

bool last_state = false;



void FLASH_Init_SPI(void)
{
  //!!!! использует SPI совместно с LCD last_state = CMU_IsClockEnable(cmuClock_GPIO);
  /*if( !last_state )*/
  CMU_ClockEnable( cmuClock_GPIO, true );
  GPIO_PinModeSet( FLASH_CS.port,       FLASH_CS.pin,   /*gpioModeInputPull*/gpioModePushPull, 1 );
  GPIO_PinOutSet ( FLASH_CS.port,       FLASH_CS.pin);       // Bring CS high
  GPIO_PinModeSet( FLASH_SCL.port,      FLASH_SCL.pin, 	gpioModePushPull, 	1 );
  GPIO_PinModeSet( FLASH_SI.port,       FLASH_SI.pin, 	gpioModePushPull, 	1 );
  GPIO_PinModeSet( FLASH_SO.port,       FLASH_SO.pin,   gpioModeInput,          1 );
}

/*
void FLASH_Deinit_SPI(void)
{
  GPIO_PinModeSet( FLASH_CS.port,       FLASH_CS.pin,   gpioModeDisabled, 	0 );
  GPIO_PinModeSet( FLASH_SI.port,       FLASH_SI.pin, 	gpioModeDisabled, 	0 );
  GPIO_PinModeSet( FLASH_SCL.port,      FLASH_SCL.pin, 	gpioModeDisabled, 	0 );
  GPIO_PinModeSet( FLASH_SO.port,       FLASH_SO.pin, 	gpioModeDisabled, 	0 );
  //if(last_state) CMU_ClockEnable(cmuClock_GPIO, false);   
}
*/

bool FLASH_EraseBlock( uint8_t cmd, uint32_t block )
{
  if( FLASH_IsReady() ) {
    if( FLASH_WriteEnable() ) {
      GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
      GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select
      FLASH_ByteOut( cmd );
      FLASH_ByteOut( (block>>16) & 0xFF );
      FLASH_ByteOut( (block>>8) & 0xFF );
      FLASH_ByteOut( block & 0xFF );
      GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );      
      return true;//FLASH_GetReady();
    } 
  }
  return false;
}

bool FLASH_EraseBlock_64K( uint32_t block )
{
  FLASH_EraseBlock( FLASH_BLOCK_64K_ERASE, block );
  return FLASH_GetReady();
}



void FLASH_ResumeFromDeepSleep()
{
  GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
  GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select
  FLASH_ByteOut( FLASH_RESUME_POWER_UP );
  GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );  
}


/*
bool FLASH_WriteBuffer( uint32_t adress, const uint8_t *buffer, size_t lenght )
{
  if( adress > FLASH_TOTAL_SIZE_BYTE ) return false;

  if( FLASH_IsReady() ) {
    if( FLASH_WriteEnable() ) {
      GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
      GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select
      
      FLASH_ByteOut( FLASH_WRITE_DATA );
      FLASH_ByteOut( (adress>>16) & 0xFF );
      FLASH_ByteOut( (adress>>8) & 0xFF );
      FLASH_ByteOut( adress & 0xFF );

      for(size_t cnt = 0; cnt < lenght; cnt++) {
        if( ((uint8_t)((adress+cnt)&FLASH_PAGE_SIZE_BYTE)) == FLASH_PAGE_SIZE_BYTE ) {
          FLASH_ByteOut( *buffer++ );
          GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
          if( FLASH_GetReady() ) {
              if( FLASH_WriteEnable() ) {
                GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
                GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select
                FLASH_ByteOut( FLASH_WRITE_DATA );
                FLASH_ByteOut( ((adress+cnt+1)>>16) & 0xFF );
                FLASH_ByteOut( ((adress+cnt+1)>>8 ) & 0xFF );
                FLASH_ByteOut( (adress+cnt+1) & 0xFF );
              }
          }
        } else FLASH_ByteOut( *buffer++ );
      }
      GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
      return true;  
    }
  }
  return false;
}
*/

bool FLASH_ReadBuffer( uint32_t adress, uint8_t *buffer, size_t lenght )
{
  if( adress > FLASH_TOTAL_SIZE_BYTE || !buffer ) return false;

  if( FLASH_IsReady() ) {
    GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
    GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select    
    FLASH_ByteOut( FLASH_READ_DATA );
    FLASH_ByteOut( (adress>>16) & 0xFF );
    FLASH_ByteOut( (adress>>8) & 0xFF );
    FLASH_ByteOut( adress & 0xFF );

    while( lenght ) {
      *buffer = FLASH_ByteIn();
      buffer++;
      lenght--;
    }
    GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
    return true;  
  }
  return false;
}


uint8_t FLASH_ReadByte( uint32_t adress )
{
  uint8_t rbyte;

  if( FLASH_IsReady() ) {
    GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
    GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select   
    FLASH_ByteOut( FLASH_READ_DATA );
    FLASH_ByteOut( (adress>>16) & 0xFF );
    FLASH_ByteOut( (adress>>8) & 0xFF );
    FLASH_ByteOut( adress & 0xFF );
    rbyte = FLASH_ByteIn();
  }
  GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
  return rbyte;
}
/*
bool FLASH_WriteByte( uint32_t adress, uint8_t wbyte )
{
  if( FLASH_IsReady() ) {
    if( FLASH_WriteEnable() ) {
      GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
      GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select
      
      FLASH_ByteOut( FLASH_WRITE_DATA );
      FLASH_ByteOut( (adress>>16) & 0xFF );
      FLASH_ByteOut( (adress>>8) & 0xFF );
      FLASH_ByteOut( adress & 0xFF );
 
      FLASH_ByteOut( wbyte );
      GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
      return true;  
    }
  }
  return false;
}
*/


bool FLASH_GetReady( void )
{
  GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
  GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select  
  FLASH_ByteOut( FLASH_READ_STATUS_1 );
  while( FLASH_ByteIn() & FLASH_READY_STATUS_MASK );
  GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
  return true;
}


uint8_t FLASH_ReadStatus_1( void ) 
{
  uint8_t status;
  GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
  GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select  
  FLASH_ByteOut( FLASH_READ_STATUS_1 );
  status = FLASH_ByteIn();
  GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
  return status;
}
/*
uint8_t FLASH_ReadStatus_2( void ) 
{
  uint8_t status;
  GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
  GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select  
  FLASH_ByteOut( FLASH_READ_STATUS_2 );
  status = FLASH_ByteIn();
  GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
  return status;
}
*/

bool FLASH_WriteEnable( void ) 
{
  GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
  GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select  
  FLASH_ByteOut( FLASH_WRITE_ENABLE );
  return true;//FLASH_IsWriteEnable();
}

/*
bool FLASH_WriteDisable( void ) 
{
  GPIO_PinOutSet( FLASH_CS.port, FLASH_CS.pin );
  GPIO_PinOutClear( FLASH_CS.port, FLASH_CS.pin ); // Chip select  
  FLASH_ByteOut( FLASH_WRITE_DISABLE );
  return !FLASH_IsWriteEnable();
}
*/
//volatile uint32_t edelay;
/********************************************************************
 * Function:        unsigned char bytein(void)
 *
 * Description:     This function inputs a single byte from the
 *                  SPI bus, MSb first.
 *******************************************************************/
uint8_t FLASH_ByteIn( void )
{
    int i;
    uint8_t retval = 0;    // Return value
    GPIO_PinOutClear( FLASH_SCL.port, FLASH_SCL.pin );        // Ensure SCK is low          
    for ( i = 0; i < 8; i++ )    {
        retval = retval << 1;
        GPIO_PinOutSet( FLASH_SCL.port, FLASH_SCL.pin );
        if (GPIO_PinInGet( FLASH_SO.port, FLASH_SO.pin ) == 1)  {
            retval |= 0x01;         // If a 1, set next bit to 1
        } else  {
            retval &= 0xFE;         // If a 0, set next bit to 0
        }
        GPIO_PinOutClear( FLASH_SCL.port, FLASH_SCL.pin );    // Bring SCK low for next bit        
    }
    return retval;
}

/********************************************************************
 * Function:        void byteout(unsigned char byte)
 *
 * Description:     This function outputs a single byte onto the
 *                  SPI bus, MSb first.
 *******************************************************************/

void FLASH_ByteOut( uint8_t Byte )
{
    int i;
    for ( i = 0; i < 8; i++ )     {
        GPIO_PinOutClear( FLASH_SCL.port, FLASH_SCL.pin );
        if ( Byte & 0x80 ) {
           GPIO_PinOutSet( FLASH_SI.port, FLASH_SI.pin );
        } else {
            GPIO_PinOutClear( FLASH_SI.port, FLASH_SI.pin );
        }
        GPIO_PinOutSet( FLASH_SCL.port, FLASH_SCL.pin );
        Byte = Byte << 1;
    }
    GPIO_PinOutClear( FLASH_SCL.port, FLASH_SCL.pin );
}


uint16_t FLASH_CRC_calc(uint32_t start, uint32_t end)
  {
    uint16_t crc = 0x0;
    uint32_t  data;

    for (data = start; data < end; data++)
    {
      crc  = (crc >> 8) | (crc << 8);
      crc ^= FLASH_ReadByte(data);
      crc ^= (crc & 0xff) >> 4;
      crc ^= crc << 12;
      crc ^= (crc & 0xff) << 5;
    }
    return crc;
  }