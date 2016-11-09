#include "leuart.h"

#include "em_chip.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_leuart.h"
#include "em_gpio.h"

#include "main.h"
#include "mqueue.h"


#define LEUART_TX_BUFFER_SIZE 256
#define LEUART_RX_BUFFER_SIZE 32

__data uint8_t leuart_tx[LEUART_TX_BUFFER_SIZE];
uint8_t volatile leUART_TX_head = 0, leUART_TX_tail = 0;

bool LeUART_IsEnable = false;



void LeUART_Init( void )
{
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_LEUART0, true);    /* Enable LEUART1 clock */
  CMU_ClockEnable(cmuClock_GPIO, true);       /* Enable GPIO clock */
  
/* Defining the LEUART1 initialization data */
  LEUART_Init_TypeDef leuartInit =
  {
    .enable   = leuartEnableTx,     /* Activate data reception on LEUn_RX pin. */
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
  LEUART0->ROUTE = /*LEUART_ROUTE_RXPEN |*/ LEUART_ROUTE_TXPEN
                   | LEUART_ROUTE_LOCATION_LOC1;

  /* Enable GPIO for LEUART1. RX */
  //GPIO_PinModeSet(RS485_RX.port,            /* Port */
  //                RS485_RX.pin,                    /* Port number */
  //                gpioModeInputPull,    /* Pin mode is set to input only, with pull direction given bellow */
  //                1);                   /* Pull direction is set to pull-up */

  /* Enable GPIO for LEUART1. TX */
  GPIO_PinModeSet(RS485_TX.port,                /* GPIO port */
                  RS485_TX.pin,                        /* GPIO port number */
                  gpioModePushPull,         /* Pin mode is set to push pull */
                  1);                       /* High idle state */

    /* Enable LEUART Signal Frame Interrupt */
  LEUART_IntClear(LEUART0, LEUART_IF_RXDATAV);
  LEUART_IntEnable(LEUART0, LEUART_IEN_SIGF);

  NVIC_ClearPendingIRQ(LEUART0_IRQn);
  /* Enable LEUART1 interrupt vector */
  NVIC_EnableIRQ(LEUART0_IRQn);
  
  LeUART_IsEnable = true;
}

void LeUART_TxEnable( void )
{
  GPIO_PinModeSet(RX485_TX_EN.port,         /* GPIO port */
                  RX485_TX_EN.pin,          /* GPIO port number */
                  gpioModePushPull,         /* Pin mode is set to push pull */
                  1);  

}
void LeUART_TxDisable( void )
{
  GPIO_PinModeSet(RX485_TX_EN.port,         /* GPIO port */
                  RX485_TX_EN.pin,          /* GPIO port number */
                  gpioModeDisabled,         /* Pin mode is set to push pull */
                  0);  

}
void LeUART_Deinit( void )
{
    /* RX Data Valid Interrupt Enable */
  LEUART_IntDisable(LEUART0, LEUART_IEN_RXDATAV | LEUART_IEN_TXBL);
  
  /* Enable LEUART1 interrupt vector */
  NVIC_DisableIRQ(LEUART0_IRQn);  
  
  /* Enable GPIO for LEUART1. RX */
  GPIO_PinModeSet(RS485_RX.port,            /* Port */
                  RS485_RX.pin,                    /* Port number */
                  gpioModeDisabled,    /* Pin mode is set to input only, with pull direction given bellow */
                  0);                   /* Pull direction is set to pull-up */

  /* Enable GPIO for LEUART1. TX */
  GPIO_PinModeSet(RS485_TX.port,                /* GPIO port */
                  RS485_TX.pin,                        /* GPIO port number */
                  gpioModeDisabled,         /* Pin mode is set to push pull */
                  0);                       /* High idle state */
  LeUART_IsEnable = false;
  //LeUART_TxDisable();
}

void LeUART_StartTransmitt( void )
{
	LEUART_IntEnable(LEUART0, LEUART_IEN_TXBL);           /* TX Buffer Level Interrupt Enable */
}
void LeUART_StopTransmitt( void )
{
	LEUART_IntDisable(LEUART0, LEUART_IEN_TXBL);           /* TX Buffer Level Interrupt Enable */
}

bool LeUART_TxIsEmpty( void )
{
	return (leUART_TX_tail == leUART_TX_head);
}
size_t LeUART_GetTxEmptyBufferSize( void )
{
	if( leUART_TX_tail <= leUART_TX_head )    return  leUART_TX_head - leUART_TX_tail;
	else return ( LEUART_TX_BUFFER_SIZE - leUART_TX_tail ) + leUART_TX_head;
}
bool LeUART_PutTx( uint8_t data )
{
	leuart_tx[leUART_TX_head++] = data;
	if( leUART_TX_head >= LEUART_TX_BUFFER_SIZE )		leUART_TX_head = 0;
        return true;
}
bool LeUART_PutSlipTx( uint8_t data )
{
      	if( (data == SLIP_START) || (data == SLIP_END) || (data == SLIP_ESC) ) {
		LeUART_PutTx( SLIP_ESC );
		LeUART_PutTx( data ^ 0xFF );
    	}
        return true;
}
bool LeUART_GetTx( uint8_t *data )
{
	if( LeUART_TxIsEmpty() ) return false;
	*data = leuart_tx[leUART_TX_tail++];
	if ( leUART_TX_tail >= LEUART_TX_BUFFER_SIZE ) leUART_TX_tail = 0;
        return true;
}
size_t LeUART_WriteTxBuffer( const uint8_t *data, size_t lenght )
{
	size_t Cnt = 0;
	while( lenght ) {
		leuart_tx[leUART_TX_head++] = *data;
		if( leUART_TX_head >= LEUART_TX_BUFFER_SIZE ) leUART_TX_head = 0;
		data++;
		lenght--;
                Cnt++;
	}
	return Cnt;
}
size_t LeUART_SendCmd( uint8_t cmd, const uint8_t *data, size_t lenght )
{
	size_t Cnt = 0;
        
        LeUART_PutTx( RS485_START );
        LeUART_PutSlipTx( cmd );             

	while( lenght ) {
                LeUART_PutSlipTx( *data );
		data++;
		lenght--;
                Cnt++;
	}          
        LeUART_PutTx( RS485_END );
	return Cnt;
}


void LeUART_SendManually( void )
{
	while ( leUART_TX_tail != leUART_TX_head ) {
		LEUART_Tx( LEUART0, leuart_tx[leUART_TX_tail++] );
		if( leUART_TX_tail >= LEUART_TX_BUFFER_SIZE ) leUART_TX_tail = 0;
	}
        //LeUART_TxDisable();
}

void LeUART_SendCalibr(uint32_t ade_watt, uint32_t ade_amp, uint32_t ade_volt, uint32_t merc_imp)
{
  uint8_t str[64];
  if( !LeUART_IsEnable ) LeUART_Init();
  
  snprintf(str, sizeof(str), "W%09dA%09dV%09dM%09d", ade_watt, ade_amp, ade_volt, merc_imp);
  LeUART_WriteTxBuffer(str, strlen(str));
  //LeUART_TxEnable();
  LeUART_StartTransmitt();
}



/**************************************************************************//**
 * @brief LEUART IRQ handler
 *
 * When the signal frame is detected by the LEUART, this interrupt routine will
 * zero-terminate the char array, write the received string the to the LCD, and
 * reset the DMA for new data.
 *
 *****************************************************************************/
void LEUART0_IRQHandler(void)
{
  uint32_t leuartif;
  uint8_t data;

  /* Store and reset pending interupts */
  leuartif = LEUART_IntGet( LEUART0 );
  LEUART_IntClear( LEUART0, leuartif );

  if ( leuartif & LEUART_IF_RXDATAV ) {  /**< TX Complete Interrupt Flag */
    mQueue_PutMsg( LEUART_RECV_EVENT, (uint8_t)LEUART0->RXDATA );
  }

  if ( leuartif & LEUART_IF_TXBL ) {  /**< TX Buffer Level Interrupt Flag */
    if( LeUART_GetTx( &data ) ) {
      LEUART0->TXDATA = data;
      //LEUART_Tx( LEUART0, data );
    } else {
      LEUART_IntDisable( LEUART0, LEUART_IEN_TXBL );
      //LeUART_TxDisable();
    }
  }
}



