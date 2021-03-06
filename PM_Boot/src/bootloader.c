/**************************************************************************//**
 * @file bootloader.c
 * @brief EFM32 Bootloader. Preinstalled on all new EFM32 devices
 * @version x.xx
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdbool.h>
#include "em_device.h"
#include "usart.h"
#include "xmodem.h"
#include "boot.h"
#include "debuglock.h"
#include "autobaud.h"
#include "crc.h"
#include "config.h"
#include "flash.h"
#include "at25sf041.h"

#ifndef NDEBUG
#include "debug.h"
#include <stdio.h>
#endif


#define BUTTON1 		((gpio_t){gpioPortD, 4	})
#define BUTTON2 		((gpio_t){gpioPortD, 5	})


/*
#define BOOTLOADER_VERSION_STRING "1.00 "

// Version string, used when the user connects
#if !defined( BOOTLOADER_VERSION_STRING )
#error "No bootloader version string defined !"
#endif
*/
// Vector table in RAM. We construct a new vector table to conserve space in
// flash as it is sparsly populated.
/*
#pragma location=0x20000000
__no_init uint32_t vectorTable[47];
*/
// This variable holds the computed CRC-16 of the bootloader and is used during
// production testing to ensure the correct programming of the bootloader.
// This can safely be omitted if you are rolling your own bootloader.
#pragma location=0x200000bc
__no_init uint16_t bootloaderCRC;

// If this flag is set the bootloader will be reset when the RTC expires.
// This is used when autobaud is started. If there has been no synchronization
// until the RTC expires the entire bootloader is reset.
// Essentially, this makes the RTC work as a watchdog timer.
bool resetEFM32onRTCTimeout = false;

//__ramfunc __noreturn void commandlineLoop(void);
//__ramfunc void verify(uint32_t start, uint32_t end);

/*
#if defined(_SILICON_LABS_32B_PLATFORM_2)
#define RTC_IRQ           RTCC_IRQn
#define RTC_INT_HANDLER   RTCC_IRQHandler
#define RTC_INT_CLEAR()   RTCC->IFC = _RTCC_IFC_MASK
#define RTC_INT_ENABLE()  RTCC->IEN = RTCC_IEN_CC1
#define RTC_COMPSET(x)    RTCC->CC[1].CCV = (x)
#define RTC_START()       RTCC->CTRL = RTCC_CTRL_DEBUGRUN | RTCC_CTRL_ENABLE \
                                       | RTCC_CTRL_CCV1TOP;                  \
                          RTCC->CC[1].CTRL = RTCC_CC_CTRL_MODE_OUTPUTCOMPARE \
                                             | RTCC_CC_CTRL_ICEDGE_NONE

#else
#define RTC_IRQ           RTC_IRQn
#define RTC_INT_HANDLER   RTC_IRQHandler
#define RTC_INT_CLEAR()   RTC->IFC = RTC_IFC_COMP1 | RTC_IFC_COMP0 | RTC_IFC_OF
#define RTC_INT_ENABLE()  RTC->IEN = RTC_IEN_COMP0
#define RTC_COMPSET(x)    RTC->COMP0 = (x)
#define RTC_START()       RTC->CTRL = RTC_CTRL_COMP0TOP | RTC_CTRL_DEBUGRUN | RTC_CTRL_EN
#endif
*/
#define BASE_FIRM_START_ADR             ( 0x60000 )                     //����� "������" ��������
#define UPGRADE_START_ADR               ( 0x70000 )                     //����� ��� �������� ����������

typedef struct {
  uint16_t      ver;
  uint16_t      start_add;
  uint16_t      end_add;
  uint16_t      crc;
  uint16_t      reserved[2];
  uint16_t      crc_of_this_struct;
}soft_update;

                                               
/**************************************************************************//**
 * Strings.
 *****************************************************************************/
/*uint8_t crcString[]     = "\r\nCRC: ";
uint8_t newLineString[] = "\r\n";
uint8_t readyString[]   = "\r\nReady\r\n";
uint8_t okString[]      = "\r\nOK\r\n";
uint8_t failString[]    = "\r\nFail\r\n";
uint8_t unknownString[] = "\r\n?\r\n";
*/


/**************************************************************************//**
 * @brief RTC IRQ Handler
 *   The RTC is used to keep the power consumption of the bootloader down while
 *   waiting for the pins to settle, or work as a watchdog in the autobaud
 *   sequence.
 *****************************************************************************/
/*void RTC_INT_HANDLER(void)
{
  RTC_INT_CLEAR();                  // Clear interrupt flags.
  if (resetEFM32onRTCTimeout)       // Check if EFM should be reset on timeout.
  {
#ifndef NDEBUG
    printf("Autobaud Timeout. Resetting EFM32.\r\n");
#endif
    // Write to the Application Interrupt/Reset Command Register to reset
    // the EFM32. See section 9.3.7 in the reference manual.
    SCB->AIRCR = 0x05FA0004;
  }
}
*/
/**************************************************************************//**
 * @brief
 *   This function is an infinite loop. It actively waits for one of the
 *   following conditions to be true:
 *   1) The SWCLK Debug pins is not asserted and a valid application is
 *      loaded into flash.
 *      In this case the application is booted.
 *   OR:
 *   2) The SWCLK pin is asserted and there is an incoming packet
 *      on the USART RX line
 *      In this case we start sensing to measure the baudrate of incoming packets.
 *
 *   If none of these conditions are met, the EFM32G is put to EM2 sleep for
 *   250 ms.
 *****************************************************************************/
/*
static void waitForBootOrUSART(void)
{
  uint32_t SWDpins;
#ifndef NDEBUG
  uint32_t oldPins = 0xf;
#endif

  // Initialize RTC/RTCC.
  RTC_INT_CLEAR();                    // Clear interrupt flags.
  RTC_COMPSET((PIN_LOOP_INTERVAL * LFRCO_FREQ) / 1000); // 250 ms wakeup time.
  RTC_INT_ENABLE();                   // Enable interrupt on compare channel.
  NVIC_EnableIRQ(RTC_IRQ);            // Enable RTC interrupt.
  RTC_START();                        // Start RTC.

  while (1)
  {
    // The SWCLK signal is used to determine if the application
    // Should be booted or if the bootloader should be started
    // SWCLK (F0) has an internal pull-down and should be pulled high
    // to enter bootloader mode.

    // Check input pins.
    SWDpins = 1;//GPIO->P[5].DIN & 0x1;

#ifndef NDEBUG
    if (oldPins != SWDpins)
    {
      oldPins = SWDpins;
      printf("New pin: %x \r\n", SWDpins);
    }
#endif

    // Check if pins are not asserted AND firmware is valid.
    if ((SWDpins != 0x1) && (BOOT_checkFirmwareIsValid()))
    {
      // Boot application.
#ifndef NDEBUG
      printf("Booting application \r\n");
#endif
      BOOT_boot();
    }

    // SWCLK (F0) is pulled high and SWDIO (F1) is pulled low.
    // Enter bootloader mode.
    if (SWDpins == 0x1)
    {
      // Increase timeout to 30 seconds.
      RTC_COMPSET(AUTOBAUD_TIMEOUT * LFRCO_FREQ);
      // If this timeout occurs the EFM32 is rebooted. This is
      // done so that the bootloader cannot get stuck in autobaud sequence.
      resetEFM32onRTCTimeout = true;
#ifndef NDEBUG
      printf("Starting autobaud sequence\r\n");
#endif
      return;
    }
    // Go to EM2 and wait for RTC wakeup.
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
  }
}
*/

/**************************************************************************//**
 * @brief
 *   Helper function to print flash write verification using CRC
 * @param start
 *   The start of the block to calculate CRC of.
 * @param end
 *   The end of the block. This byte is not included in the checksum.
 *****************************************************************************/
/*__ramfunc void verify(uint32_t start, uint32_t end)
{
  USART_printString(crcString);
  USART_printHex(CRC_calc((void *) start, (void *) end));
  USART_printString(newLineString);
}
*/
/**************************************************************************//**
 * @brief
 *   The main command line loop. Placed in Ram so that it can still run after
 *   a destructive write operation.
 *   NOTE: __ramfunc is a IAR specific instruction to put code into RAM.
 *   This allows the bootloader to survive a destructive upload.
 *****************************************************************************/
/*__ramfunc __noreturn void commandlineLoop(void)
{
  uint32_t flashSize;
  uint8_t  c;
  uint8_t *returnString;

  // Find the size of the flash. DEVINFO->MSIZE is the size in KB,
  // so left shift by 10.
  flashSize = ((DEVINFO->MSIZE & _DEVINFO_MSIZE_FLASH_MASK)
               >> _DEVINFO_MSIZE_FLASH_SHIFT)
              << 10;

  while (1)                                     // The main command loop.
  {
    c = USART_rxByte();                         // Retrieve new character.
    if (c != 0)
    {
      USART_txByte(c);                          // Echo the char
    }
    switch (c)
    {
    case 'u':                                   // Upload.
      USART_printString(readyString);
      XMODEM_download(APPLICATION_START_ADDR, flashSize);
      break;

    case 'd':                                   // Destructive upload.
      USART_printString(readyString);
#if defined(_SILICON_LABS_32B_PLATFORM_2)
      // Treat destructive upload on platform 2 as bootloader overwrite.
      XMODEM_download(BOOTLOADER_START_ADDR,
                      BOOTLOADER_START_ADDR + BOOTLOADER_SIZE);
#else
      XMODEM_download(BOOTLOADER_START_ADDR, flashSize);
#endif
      break;

    case 't':                                   // Write to user page.
      USART_printString(readyString);
      XMODEM_download(USER_PAGE_START_ADDR, USER_PAGE_END_ADDR);
      break;

    case 'p':                                   // Write to lock bits.
#if defined( CoreDebug )        // In core_cmX.h.
      DEBUGLOCK_startDebugInterface();
#endif
      USART_printString(readyString);
#if defined(USART_OVERLAPS_WITH_BOOTLOADER) && defined( CoreDebug )
      // Since the UART overlaps, the bit-banging in
      // DEBUGLOCK_startDebugInterface() will generate some traffic. To avoid
      // interpreting this as UART communication, we need to flush the
      // UART data buffers.
      BOOTLOADER_USART->CMD = BOOTLOADER_USART_CLEARRX;
#endif
      XMODEM_download(LOCK_PAGE_START_ADDR, LOCK_PAGE_END_ADDR);
      break;

    case 'b':                                   // Boot into new program.
      BOOT_boot();
      break;

    case 'l':                                   // Debug lock.
#if !defined(NDEBUG) && defined( CoreDebug )
      // We check if there is a debug session active in DHCSR. If there is we
      // abort the locking. This is because we wish to make sure that the debug
      // lock functionality works without a debugger attatched.
      if ((CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) != 0x0)
      {
        printf("\r\n\r\n **** WARNING: DEBUG SESSION ACTIVE. NOT LOCKING!  **** \r\n\r\n");
        USART_printString("Debug active.\r\n");
      }
      else
      {
        printf("Starting debug lock sequence.\r\n");
#endif
        if (DEBUGLOCK_lock())
        {
          returnString = okString;
        }
        else
        {
          returnString = failString;
        }
        USART_printString(returnString);

#if !defined(NDEBUG) && defined( CoreDebug )
        printf("Debug lock word: 0x%x \r\n", *(uint32_t *)DEBUG_LOCK_WORD_ADDR);
      }
#endif
      break;

    case 'v':             // Verify content by calculating CRC of entire flash.
      verify(0, flashSize);
      break;

    case 'c':             // Verify content by calculating CRC of application area.
      verify(APPLICATION_START_ADDR, flashSize);
      break;

    case 'n':             // Verify content by calculating CRC of user page.
      verify(USER_PAGE_START_ADDR, USER_PAGE_END_ADDR);
      break;

    case 'm':             // Verify content by calculating CRC of lock page.
      verify(LOCK_PAGE_START_ADDR, LOCK_PAGE_END_ADDR);
      break;

    case 'r':             // Reset command.
      // Write to the Application Interrupt/Reset Command Register to reset
      // the EFM32. See section 9.3.7 in the reference manual.
      SCB->AIRCR = 0x05FA0004;
      break;

      break;

    default:
      USART_printString(unknownString);
    case 0:               // Unknown command.
      // Timeout waiting for RX - avoid printing the unknown string.
      break;
    }
  }
}
*/
/**************************************************************************//**
 * @brief  Create a new vector table in RAM.
 *         We generate it here to conserve space in flash.
 *****************************************************************************/
/*static void generateVectorTable(void)
{
#if defined(_SILICON_LABS_32B_PLATFORM_2)
  vectorTable[RTC_IRQ + 16]             = (uint32_t)RTC_INT_HANDLER;

#else
  vectorTable[AUTOBAUD_TIMER_IRQn + 16] = (uint32_t) TIMER_IRQHandler;
  vectorTable[RTC_IRQ + 16]             = (uint32_t)RTC_INT_HANDLER;
#ifdef USART_OVERLAPS_WITH_BOOTLOADER
  vectorTable[GPIO_EVEN_IRQn + 16]      = (uint32_t) GPIO_IRQHandler;
#endif
#endif
  SCB->VTOR                             = (uint32_t)vectorTable;
}
*/
/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
__noreturn void main(void)
{
#if !defined(_SILICON_LABS_32B_PLATFORM_2)
  uint32_t periodTime24_8;
  uint32_t tuning;
#endif
#if !defined(_SILICON_LABS_32B_PLATFORM_2) || !defined(NDEBUG)
  uint32_t clkdiv;
#endif

  // Generate a new vector table and place it in RAM.
  //generateVectorTable();

  // Calculate CRC16 for the bootloader itself and the Device Information page.
  // This is used for production testing and can safely be omitted in
  // your own code.
  bootloaderCRC  = CRC_calc((void *)BOOTLOADER_START_ADDR,
                            (void *)BOOTLOADER_END_ADDR);
  bootloaderCRC |= CRC_calc((void *)(DEVINFO_START_ADDR + 2),
                            // Skip first 2 bytes, they are DEVINFO crc.
                            (void *)DEVINFO_END_ADDR)
                   << 16;
  // End safe to omit.

 // Enable clocks for peripherals.
  CMU->HFPERCLKDIV = CMU_HFPERCLKDIV_HFPERCLKEN;
  CMU->HFPERCLKEN0 = CMU_HFPERCLKEN0_GPIO | BOOTLOADER_USART_CLOCKEN |
                     AUTOBAUD_TIMER_CLOCK ;

  // Enable LE and DMA interface.
  CMU->HFCORECLKEN0 = CMU_HFCORECLKEN0_LE | CMU_HFCORECLKEN0_DMA;

  // Enable LFRCO for RTC.
  CMU->OSCENCMD = CMU_OSCENCMD_LFRCOEN;
  // Setup LFA to use LFRCRO.
  CMU->LFCLKSEL = CMU_LFCLKSEL_LFA_LFRCO | CMU_LFCLKSEL_LFB_HFCORECLKLEDIV2;
  // Enable RTC.
  CMU->LFACLKEN0 = CMU_LFACLKEN0_RTC;


#if defined ( _DEVINFO_HFRCOCAL1_BAND28_MASK )
  // Change to 28MHz internal oscillator to increase speed of
  // bootloader.
  tuning = (DEVINFO->HFRCOCAL1 & _DEVINFO_HFRCOCAL1_BAND28_MASK)
           >> _DEVINFO_HFRCOCAL1_BAND28_SHIFT;

  CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_28MHZ | tuning;


#elif defined( _DEVINFO_HFRCOCAL1_BAND21_MASK )
  // Change to 21MHz internal oscillator to increase speed of
  // bootloader.
  tuning = ((DEVINFO->HFRCOCAL1 & _DEVINFO_HFRCOCAL1_BAND21_MASK)
           >> _DEVINFO_HFRCOCAL1_BAND21_SHIFT);

  CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_21MHZ | tuning;

#elif defined( _CMU_HFRCOCTRL_FREQRANGE_MASK )
  // Change to 38MHz HFRCO. Use calibration data from DI page.
  CMU->HFRCOCTRL = *(uint32_t *)(&DEVINFO->HFRCOCAL12);
  // Set WSHFLE to allow access to LE peripherals when running at frequencies
  // above 32 MHz.
  CMU->CTRL |= CMU_CTRL_WSHFLE;

#else
#error "Can not make correct clock selection."
#endif
  

  static soft_update s_up;
  uint32_t update_address = UPGRADE_START_ADR;
  FLASH_Init_SPI();
  FLASH_ResumeFromDeepSleep();
  GPIO_PinModeSet( BUTTON1.port, BUTTON1.pin, gpioModeInputPullFilter, 1 );
  GPIO_PinModeSet( BUTTON2.port, BUTTON2.pin, gpioModeInputPullFilter, 1 );

  if(GPIO_PinInGet( BUTTON1.port, BUTTON1.pin ) == 0 )      //��� ���� ������� ������� ��������� "������" ��������
  {
    if(GPIO_PinInGet( BUTTON2.port, BUTTON2.pin ) == 0)
    {
      FLASH_EraseBlock_64K(UPGRADE_START_ADR);
      update_address = BASE_FIRM_START_ADR;   
    }
  }
  
  FLASH_ReadBuffer(update_address  , (uint8_t*)&s_up, sizeof(s_up)); //������ ��������� ����������� ���������� 
  if(CRC_calc((void *)s_up.start_add,(void *)s_up.end_add) == s_up.crc) BOOT_boot();  //������ ����������, ���� ���������� ��������� � �����������
  
  if(FLASH_CRC_calc(update_address + s_up.start_add, update_address + s_up.end_add) != s_up.crc)  BOOT_boot();// � ���������� �� �������� CRC
  
  FLASH_init();
  
  uint8_t mas[FLASH_PAGE_SIZE];
  
  for(uint32_t i = s_up.start_add; i < s_up.end_add; i += FLASH_PAGE_SIZE)
  {
    FLASH_eraseOneBlock(i);
  
    FLASH_ReadBuffer(update_address + i  , mas, FLASH_PAGE_SIZE);
    
    FLASH_writeBlock((void *)s_up.start_add,
                     i - s_up.start_add,
                     FLASH_PAGE_SIZE,
                     (uint8_t const *)mas);
  }
  
  BOOT_boot();  
  XMODEM_download(0,0); 

}
