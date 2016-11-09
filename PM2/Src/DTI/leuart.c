#include "em_chip.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_leuart.h"
#include "em_gpio.h"
#include "leuart.h"
#include "main.h"



uint8_t RxBuf[RxBufLength];
uint8_t TxBuf[TxBufLength];
uint8_t RxPtr = 0;
uint8_t ReadPtr = 0;
uint8_t TxPtr = 0;
uint8_t WritePtr = 0;

volatile LeUART_CALLBACK LeUART_RxCallback = 0;

void LeUART_Init( void )
{
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LEUART0, true);    /* Enable LEUART1 clock */
  CMU_ClockEnable(cmuClock_GPIO, true);       /* Enable GPIO clock */
 
/* Defining the LEUART1 initialization data */
  LEUART_Init_TypeDef leuartInit =
  {
    .enable   = leuartEnable,     /* Activate data reception on LEUn_RX pin. */
    .refFreq  = 0,                  /* Inherit the clock frequenzy from the LEUART clock source */
    .baudrate = 9600,             /* Baudrate = 9600 bps */
    .databits = leuartDatabits8,    /* Each LEUART frame containes 8 databits */
    .parity   = leuartNoParity,     /* No parity bits in use */
    .stopbits = leuartStopbits1,    /* Setting the number of stop bits in a frame to 2 bitperiods */
  };

  /* Reseting and initializing LEUART0 */
  LEUART_Reset(LEUART0);
  LEUART_Init(LEUART0, &leuartInit);

  /* Route LEUART1 RX pin to DMA location 0 */
  LEUART0->ROUTE = LEUART_ROUTE_RXPEN | LEUART_ROUTE_TXPEN | LEUART_ROUTE_LOCATION_LOC1;

  /* Enable GPIO for LEUART1. RX */
  gpio_INI_RS485_RX;
//  GPIO_PinModeSet(RS485_RX.port,            /* Port */
//                  RS485_RX.pin,                    /* Port number */
//                  gpioModeInputPull,    /* Pin mode is set to input only, with pull direction given bellow */
//                  1);                   /* Pull direction is set to pull-up */

  /* Enable GPIO for LEUART1. TX */
  gpio_INI_RS485_TX;
//  GPIO_PinModeSet(RS485_TX.port,                /* GPIO port */
//                  RS485_TX.pin,                        /* GPIO port number */
//                  gpioModePushPull,         /* Pin mode is set to push pull */
//                  1);                       /* High idle state */
  
  /* Enable GPIO for TX_EN */
  gpio_INI_RS485_TX_EN;
//#ifndef VER_3_0
//  GPIO_PinModeSet(RX485_TX_EN.port ,                /* GPIO port */
//                  RX485_TX_EN.pin,             /* GPIO port number */
//                  gpioModePushPull,         /* Pin mode is set to push pull */
//                  0);                       /* High idle state */
//#endif
  
    /* Enable LEUART Signal Frame Interrupt */
  LEUART_IntClear(LEUART0, LEUART_IF_RXDATAV);
  LEUART_IntEnable(LEUART0, LEUART_IEN_RXDATAV);

  LEUART_IntEnable(LEUART0, LEUART_IEN_TXC);

  NVIC_ClearPendingIRQ(LEUART0_IRQn);
  /* Enable LEUART1 interrupt vector */
  NVIC_EnableIRQ(LEUART0_IRQn); 
}

void LeUART_DeInit( void )
{
  NVIC_DisableIRQ(LEUART0_IRQn);
  LEUART_Reset(LEUART0);
  gpio_OFF_RS485_RX;
//  GPIO_PinModeSet(RS485_RX.port, RS485_RX.pin, gpioModePushPull, 0);
  gpio_OFF_RS485_TX;
//  GPIO_PinModeSet(RS485_TX.port, RS485_TX.pin, gpioModePushPull, 0); 
  gpio_OFF_RS485_TX_EN;
//#ifndef VER_3_0
//  GPIO_PinModeSet(RX485_TX_EN.port, RX485_TX_EN.pin, gpioModePushPull, 0);
//#endif
}

static void LeUART_TxEnable( bool enable )
{
//#ifndef VER_3_0
  static bool enabled = 0;
  if(enable)
  {
    gpio_INI_RS485_TX_EN;
//    GPIO_PinModeSet(RX485_TX_EN.port, RX485_TX_EN.pin, gpioModePushPull, 1);  
    if(!enabled)   //do delay on front
    {
      uint32_t delay = 20000;
      while(delay--);
    }
  }
  else
  {
    gpio_OFF_RS485_TX_EN;
//    GPIO_PinModeSet(RX485_TX_EN.port, RX485_TX_EN.pin, gpioModePushPull, 0);  
  }    
  enabled = enable;   
//#endif
}

void LEUART0_IRQHandler(void)
{
  uint32_t leuartif;

  // Store and reset pending interupts 
  leuartif = LEUART_IntGet( LEUART0 );
  LEUART_IntClear( LEUART0, leuartif );

  if ( leuartif & LEUART_IF_RXDATAV ) // RX valid Interrupt Flag 
  {     
    uint8_t c = (RxBuf[RxPtr++%RxBufLength] = LEUART0->RXDATA);
   // LeUART_PutChar(c); //echo
    if(LeUART_RxCallback) LeUART_RxCallback(c);
  }

  if ( leuartif & LEUART_IF_TXBL )   // TX Buffer Level Interrupt Flag 
  {  
    if(WritePtr!=TxPtr)
    {
      LEUART0->TXDATA = TxBuf[TxPtr++%TxBufLength];
    } 
    else
    {
         LEUART_IntDisable(LEUART0, LEUART_IEN_TXBL);           // TX Buffer Level Interrupt disable         
    }
  }
  if ( leuartif & LEUART_IF_TXC  )   // TX Complete Interrupt Flag 
  {
    LeUART_TxEnable(false);
  }
}

void LeUART_PutChar(uint8_t c)
{
 LEUART_IntDisable(LEUART0, LEUART_IEN_TXBL);           // TX Buffer Level Interrupt disable
 
 TxBuf[WritePtr++%TxBufLength]=c;
 
 if(WritePtr == TxPtr) return;
 
 LeUART_TxEnable(true);
 LEUART_IntEnable(LEUART0, LEUART_IEN_TXBL);          // TX Buffer Level Interrupt enable
}

uint16_t LeUART_GetChar(void)
{
 if(RxPtr!=ReadPtr) 
 {
  return RxBuf[ReadPtr++%RxBufLength];
 }
 else return 0x8000;
}


void LeUART_SetRxCallback(LeUART_CALLBACK cb)
{
    LeUART_RxCallback = cb;
}

