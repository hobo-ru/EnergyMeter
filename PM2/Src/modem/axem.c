#include "axem.h"

#include <time.h>
#include <string.h>

#include <systimer.h>

#include "main.h"
#include "meter.h"


#define AXEM_CALLBACK_BUFFER_SIZE	8
#define AXEM_RECOGNIZE_BUFFER_SIZE	128



uint8_t DL_Mode = SLIP_CRX;

int32_t timer_LCD_BACKLIGHT_off = 0;

void send_BACKLIGHT_off()
{
    int i;
    uint8_t u = 0;
    for (i = 1; i < PHASE_MAX; i++) {
        if (meter->data->curr.phase[i].U > 160) u++;
        if (meter->data->curr.phase[i].U > 200) u+=10;
    }
    if (u < 2) {
        gpio_OFF_LCD_BACKLIGHT;
        timer_LCD_BACKLIGHT_off = 12;
    }
}

void send_BACKLIGHT_on()
{
    if (!CheckPowerLine()) return;
    gpio_INI_LCD_BACKLIGHT;
}

typedef struct {
	uint8_t cmd;
	AXEM_CALLBACK Fn;
}axem_callback_s;

typedef enum {
	SLIP_START_STATE
	, SLIP_RECEIVING_STATE
	, SLIP_ESC_STATE
}recv_states_e;



__data uint8_t recognize_buf[AXEM_RECOGNIZE_BUFFER_SIZE];
uint8_t RecognizeHead = 0, RecognizeTail = 0;

static recv_states_e recv_state = SLIP_START_STATE;
static axem_callback_s AxemCallback[AXEM_CALLBACK_BUFFER_SIZE];
uint8_t CallbackHead = 0;
static AXEM_EXECUTE AxemExecute = NULL;


void AxemReset()
{
//#ifdef VER_2_3 
//  GPIO_PinModeSet( IMPLED.port, IMPLED.pin, gpioModePushPull, 0 );
//  uint32_t delay = 20000;
//  while(delay--);
//  GPIO_PinModeSet( IMPLED.port, IMPLED.pin, gpioModePushPull, 1 );
//#endif      
//#ifdef VER_3_0  
  gpio_INI_AXSEM_RESET;
//  GPIO_PinModeSet( AXSEM_RESET.port, AXSEM_RESET.pin, gpioModePushPull, 0 );
  uint32_t delay = 20000;
  while (delay--);
  gpio_OFF_AXSEM_RESET;
//  GPIO_PinModeSet( AXSEM_RESET.port, AXSEM_RESET.pin, gpioModeInputPull, 1 );
//#endif      
}


void AxemInit( void )
{
        AxemReset();
	ModemUART_Init();
	memset( &AxemCallback, 0, sizeof(AxemCallback) );
	CallbackHead = 0;
}

void AxemDeinit( void )
{
	ModemUART_Deinit();

	memset( &AxemCallback, 0, sizeof(AxemCallback) );
	CallbackHead = 0;
        AxemExecute = NULL;
}

void Axem_SetReceiveCallback( UART_RECEIVE_CALLBACK Receive )
{
	ModemUART_SetReceiveCallback( Receive );
}
void Axem_SetExecuteCallback( AXEM_EXECUTE Execute )
{
	AxemExecute = Execute;
}

AXEM_CALLBACK Axem_GetReplayCallback( AXEM_RECEIVE_CMD_E cmd )
{
	int i;
	if( CallbackHead ) {
		for(i=0; i < CallbackHead; i++) {
			if( AxemCallback[i].cmd == cmd ) {
				return AxemCallback[i].Fn;
			}
		}
	}
	return NULL;
}

bool Axem_SetReplayCallback( AXEM_RECEIVE_CMD_E code, AXEM_CALLBACK Callback )
{
	int i;
	if( CallbackHead >= AXEM_CALLBACK_BUFFER_SIZE ) return false;

	if( CallbackHead ) {
		for( i=0; i < CallbackHead; i++ ) {
			if( AxemCallback[i].cmd == code ) {
				AxemCallback[i].Fn = Callback;
				return true;
			}
		}
	}
        AxemCallback[CallbackHead].cmd = code;
        AxemCallback[CallbackHead].Fn = Callback;
        CallbackHead++;
	return true;
}


bool Axem_Send( AXEM_GET_DATA GetData, size_t lenght )
{
	int i;
        uint8_t data;
        
    send_BACKLIGHT_off();
	// Encode to SLIP
	ModemUART_PutTx( SLIP_START );
        ModemUART_StartSend();
        DelayMs(10);
        
        ModemUART_PutTx( SLIP_START );
	for(i=0; i<lenght; i++)  
        {
            data = GetData( i );
            if( (data == SLIP_START) || (data == SLIP_END) || (data == SLIP_ESC) ) 
            {
                ModemUART_PutTx( SLIP_ESC );
                ModemUART_PutTx( data ^ 0xFF );
            } 
            else ModemUART_PutTx( data );
	}
	ModemUART_PutTx( SLIP_END );
	ModemUART_StartSend();
        return true;
}

/*
bool SLIP_Send(uint8_t cmd, uint8_t *payload, uint8_t len)
{
	int i;
        //uint8_t data;
        
	// Encode to SLIP
	ModemUART_PutTx( SLIP_START );
        ModemUART_StartSend();
        DelayMs(10);
        
        ModemUART_PutTx( SLIP_START );
        ModemUART_PutTx( cmd );
	for(i=0; i<lenght; i++)  
        {
            //data = GetData( i );
            if( (data == SLIP_START) || (data == SLIP_END) || (data == SLIP_ESC) ) 
            {
                ModemUART_PutTx( SLIP_ESC );
                ModemUART_PutTx( payload[i] ^ 0xFF );
            } 
            else ModemUART_PutTx( payload[i] );
	}
	ModemUART_PutTx( SLIP_END );
	ModemUART_StartSend();
        return true;
}*/

void Axem_StoreRecognize( uint8_t payload )
{
  recognize_buf[RecognizeHead++] = payload;
  if( RecognizeHead >= AXEM_RECOGNIZE_BUFFER_SIZE )     RecognizeHead = 0;
}
void Axem_ThrowStatus( void )
{
  recv_state = SLIP_START_STATE;
  RecognizeTail = RecognizeHead;
}
bool Axem_ParseSlip( uint8_t slip )
{
	switch(recv_state) {
		case SLIP_START_STATE:
			if(slip == SLIP_START)
				recv_state = SLIP_RECEIVING_STATE;
		break;

		case SLIP_RECEIVING_STATE:
			if(slip == SLIP_ESC)
				recv_state = SLIP_ESC_STATE;
			else if(slip == SLIP_END) {
				recv_state = SLIP_START_STATE;
                                if( AxemExecute) AxemExecute();
				return true;
			} else Axem_StoreRecognize( slip );
		break;

		case SLIP_ESC_STATE:
			Axem_StoreRecognize( slip ^ 0xFF );
			recv_state = SLIP_RECEIVING_STATE;
		break;
	}
	return false;
}

uint8_t Axem_GetRecognize( void )
{
	uint8_t Data;
	Data = recognize_buf[RecognizeTail++];

	if( RecognizeTail >= AXEM_RECOGNIZE_BUFFER_SIZE )       RecognizeTail = 0;
	return Data;
}


bool Axem_ExecuteCallback( void )
{
	int i;
	size_t Lenght;
	AXEM_CALLBACK Callback;
	uint8_t buffer[140];

	if( RecognizeHead == RecognizeTail ) return false;

	if(RecognizeTail <= RecognizeHead )     Lenght = RecognizeHead - RecognizeTail;
	else Lenght = (AXEM_RECOGNIZE_BUFFER_SIZE - RecognizeTail) + RecognizeHead;

	if( Lenght ) {
		Callback = Axem_GetReplayCallback( (AXEM_RECEIVE_CMD_E)Axem_GetRecognize() );
                Lenght--;
		if( Callback ) {
			i = 0;
			while( i < Lenght && i<sizeof(buffer) ) {
				buffer[i++] = Axem_GetRecognize();
			}
			Callback( buffer, Lenght );
                        return true;
		}
	}
	return false;
}

void AxemEverySecond()
{
    if (DL_Mode == SLIP_FASTDL) {
        send_BACKLIGHT_off();
        timer_LCD_BACKLIGHT_off = 1;
        return;
    }
    if (timer_LCD_BACKLIGHT_off) {
        if (!--timer_LCD_BACKLIGHT_off) {
            send_BACKLIGHT_on();
        }
    }
}
