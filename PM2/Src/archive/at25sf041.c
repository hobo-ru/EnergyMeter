#include "at25sf041.h"

#include "main.h"

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_wdog.h"

extern volatile bool DTI_Enable;



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
bool FLASH_IsWriteEnable( void )     {    return ( FLASH_ReadStatus_1() & FLASH_WEL_STATUS_MASK );    }

bool last_state = false;



void FLASH_Init_SPI(void)
{
  CMU_ClockEnable( cmuClock_GPIO, true );
  gpio_INI_FLASH_POW;
  gpio_INI_FLASH_CS;
  gpio_INI_FLASH_SCL;
  gpio_INI_FLASH_SI;
  gpio_INI_FLASH_SO;
}

void FLASH_Deinit_SPI(void)
{
  gpio_OFF_FLASH_SCL;
  gpio_OFF_FLASH_SI;
  gpio_OFF_FLASH_SO;
  gpio_OFF_FLASH_CS;
  gpio_OFF_FLASH_POW;
}

void FLASH_StartCmdAddr(uint8_t cmd, uint32_t adress)
{
    gpio_SET_FLASH_CS;
    gpio_RES_FLASH_CS;
    FLASH_ByteOut( cmd );
    FLASH_ByteOut( (adress>>16) & 0xFF );
    FLASH_ByteOut( (adress>>8) & 0xFF );
    FLASH_ByteOut( adress & 0xFF );
}

bool FLASH_EraseBlock( uint8_t cmd, uint32_t block )
{
    if (!FLASH_IsReady()) return false;
    if (!FLASH_WriteEnable()) return false;
    DTI_Enable = 0;
    FLASH_StartCmdAddr(cmd, block);
    gpio_SET_FLASH_CS;
    DTI_Enable = 1;
    return FLASH_GetReady();
}

bool FLASH_EraseBlock_64K( uint32_t block )
{
  FLASH_EraseBlock( FLASH_BLOCK_64K_ERASE, block );
  return FLASH_GetReady();
}

bool FLASH_EraseBlock_4K( uint32_t block )
{
  FLASH_Init_SPI();
  return FLASH_EraseBlock( FLASH_BLOCK_4K_ERASE, block );
}

/*bool FLASH_EraseBlock_32K( uint32_t block )
{
  FLASH_EraseBlock( FLASH_BLOCK_32K_ERASE, block );
  return FLASH_GetReady();
}*/


bool FLASH_EraseChip(void)
{
    if (!FLASH_IsReady()) return false;
    if (!FLASH_WriteEnable()) return false;
    DTI_Enable = 0;
    gpio_SET_FLASH_CS;
    gpio_RES_FLASH_CS;
    FLASH_ByteOut( FLASH_CHIP_ERASE );
    gpio_SET_FLASH_CS;
    DTI_Enable = 1;
    return FLASH_GetReady();
}

void FLASH_CreateNew(void)
{
  uint8_t status_1, status_2;
  status_1 = FLASH_ReadStatus_1();
  status_2 = FLASH_ReadStatus_2();
  
  if( status_1 || status_2) {
    //TODO анализ статуса
    
    
  }
  FLASH_EraseChip();
}

bool FLASH_GoToDeepSleep()
{
//    return true;
    if (!FLASH_IsReady()) return false;
    if (!FLASH_WriteEnable()) return false;
    DTI_Enable = 0;
    gpio_SET_FLASH_CS;
    gpio_RES_FLASH_CS;
    FLASH_ByteOut( FLASH_DEEP_POWER_DOWN );
    gpio_SET_FLASH_CS;
    DTI_Enable = 1;
    return true;
}

void FLASH_ResumeFromDeepSleep()
{
//    return;
  DTI_Enable = 0;
  gpio_SET_FLASH_CS;
  gpio_RES_FLASH_CS;
  FLASH_ByteOut( FLASH_RESUME_POWER_UP );
  gpio_SET_FLASH_CS;
  DTI_Enable = 1;
}

bool FLASH_WriteByte( uint32_t adress, uint8_t wbyte )
{ 
//    return false;
    if (!FLASH_IsReady()) return false;
    if (!FLASH_WriteEnable()) return false;
    DTI_Enable = 0;
    FLASH_StartCmdAddr(FLASH_WRITE_DATA, adress);
    FLASH_ByteOut( wbyte );
    gpio_SET_FLASH_CS;
    DTI_Enable = 1;
    return true;  
}

bool FLASH_WriteBuffer( uint32_t adress, const uint8_t *buffer, size_t lenght )
{
//    return false;
    if (adress > FLASH_TOTAL_SIZE_BYTE) return false;
    if (!FLASH_IsReady()) return false;
    if (!FLASH_WriteEnable()) return false;
    DTI_Enable = 0;

    FLASH_StartCmdAddr(FLASH_WRITE_DATA, adress);
    for(size_t cnt = 0; cnt < lenght; cnt++) {
        FLASH_ByteOut( *buffer++ );
        if (((uint8_t)((adress+cnt)&FLASH_PAGE_SIZE_BYTE)) == FLASH_PAGE_SIZE_BYTE) {
            gpio_SET_FLASH_CS;
            if (FLASH_GetReady() && FLASH_WriteEnable()) {
                FLASH_StartCmdAddr(FLASH_WRITE_DATA, adress+cnt+1);
            }
        }
    }
    gpio_SET_FLASH_CS;
    DTI_Enable = 1;
    return true;  
}

uint8_t FLASH_ReadByte( uint32_t adress )
{
    uint8_t rbyte = 0;
    if (!FLASH_IsReady()) return rbyte;
    DTI_Enable = 0;
    FLASH_StartCmdAddr(FLASH_READ_DATA, adress);
    rbyte = FLASH_ByteIn();
    DTI_Enable = 1;
    gpio_SET_FLASH_CS;
    return rbyte;
}

bool FLASH_ReadBuffer( uint32_t adress, uint8_t *buffer, size_t lenght )
{
    if (adress > FLASH_TOTAL_SIZE_BYTE || !buffer) return false;
    if (!FLASH_IsReady() ) return false;
    DTI_Enable = 0;
//    FLASH_StartCmdAddr(FLASH_READ_DATA, adress);

//    while( lenght ) {
//        *buffer = FLASH_ByteIn();
//        buffer++;
//        lenght--;
//    }
    FLASH_StartCmdAddr(FLASH_READ_DATA, adress);
    for(size_t cnt = 0; cnt < lenght; cnt++) {
        buffer[cnt] = FLASH_ByteIn();
//        if (((uint8_t)((adress+cnt)&FLASH_PAGE_SIZE_BYTE)) == FLASH_PAGE_SIZE_BYTE) {
//            gpio_SET_FLASH_CS;
//            if (FLASH_GetReady()) {
//                FLASH_StartCmdAddr(FLASH_READ_DATA, adress+cnt+1);
//            }
//        }
    }

    gpio_SET_FLASH_CS;
    DTI_Enable = 1;
    return true;  
}

bool FLASH_GetReady( void )
{
    DTI_Enable = 0;
    gpio_SET_FLASH_CS;
    gpio_RES_FLASH_CS;
    FLASH_ByteOut( FLASH_READ_STATUS_1 );
    while( FLASH_ByteIn() & FLASH_READY_STATUS_MASK );
    gpio_SET_FLASH_CS;
    DTI_Enable = 1;
    return true;
}

uint8_t FLASH_ReadStatus_1()
{
    uint8_t rbyte;
    DTI_Enable = 0;
    gpio_SET_FLASH_CS;
    gpio_RES_FLASH_CS; // Chip select
    FLASH_ByteOut(FLASH_READ_STATUS_1);
    rbyte = FLASH_ByteIn();
    gpio_SET_FLASH_CS;  
    DTI_Enable = 1;
    return rbyte;
}

uint8_t FLASH_ReadStatus_2()
{
    uint8_t rbyte;
    DTI_Enable = 0;
    gpio_SET_FLASH_CS;
    gpio_RES_FLASH_CS; // Chip select
    FLASH_ByteOut(FLASH_READ_STATUS_2);
    rbyte = FLASH_ByteIn();
    gpio_SET_FLASH_CS;  
    DTI_Enable = 1;
    return rbyte;
}

void FLASH_WE_Volatile_Status()
{
    DTI_Enable = 0;
    gpio_SET_FLASH_CS;
    gpio_RES_FLASH_CS; // Chip select
    FLASH_ByteOut(FLASH_WRITE_VOLATILE_STATUS);
    gpio_SET_FLASH_CS;  
    DTI_Enable = 1;
}

void FLASH_WriteStatus(uint8_t Reg1, uint8_t Reg2)
{
    FLASH_WriteEnable();
    DTI_Enable = 0;
    gpio_SET_FLASH_CS;
    gpio_RES_FLASH_CS; // Chip select
    FLASH_ByteOut(FLASH_WRITE_STATUS_REG);
    FLASH_ByteOut(Reg1);
    if(Reg2 != 0xff) FLASH_ByteOut(Reg2); 
    gpio_SET_FLASH_CS;  
    DTI_Enable = 1;
}

bool FLASH_WriteEnable( void ) 
{
    DTI_Enable = 0;
    gpio_SET_FLASH_CS;
    gpio_RES_FLASH_CS;
    FLASH_ByteOut( FLASH_WRITE_ENABLE );
    gpio_SET_FLASH_CS;
    DTI_Enable = 1;
    return FLASH_IsWriteEnable();
}

bool FLASH_WriteDisable( void ) 
{
    DTI_Enable = 0;
    gpio_SET_FLASH_CS;
    gpio_RES_FLASH_CS;
    FLASH_ByteOut( FLASH_WRITE_DISABLE );
    gpio_SET_FLASH_CS;
    DTI_Enable = 0;
    return !FLASH_IsWriteEnable();
}

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
    gpio_RES_FLASH_SCL;
    for ( i = 0; i < 8; i++ )    {
        retval = retval << 1;
        gpio_SET_FLASH_SCL;
        if (gpio_GET_FLASH_SO)  {
            retval |= 0x01;         // If a 1, set next bit to 1
        } else  {
            retval &= 0xFE;         // If a 0, set next bit to 0
        }
        gpio_RES_FLASH_SCL;
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
    for ( i = 0; i < 8; i++ ) {
        gpio_RES_FLASH_SCL;
        if ( Byte & 0x80 ) {
            gpio_SET_FLASH_SI;
        } else {
            gpio_RES_FLASH_SI;
        }
        gpio_SET_FLASH_SCL;
        Byte = Byte << 1;
    }
    gpio_RES_FLASH_SCL;
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
      WDOG_Feed();
    }
    return crc;
  }