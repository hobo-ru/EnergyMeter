#include "rs485.h"
#include "leuart.h"
#include "mqueue.h"

#include <string.h>


#define RS485_START  0xDD
#define RS485_END    0xDE
#define RS485_ESC    0xDF

typedef enum {
  RS485_START_STATE
  , RS485_RECEIVING_STATE
  , RS485_ESC_STATE
 
}RS485_PARSE_STATES_E;

typedef struct {
  RS485_CMD_E   cmd;
  RS485_EXECUTE callback;
}RS485_CALLBACK_S;



void RS485_StartNew( void );




#define RS485_PARAM_BUFFER_SIZE 32
__data uint8_t rs485_param[RS485_PARAM_BUFFER_SIZE];
volatile uint8_t rs485_param_head = 0, rs485_param_tail = 0;

RS485_PARSE_STATES_E           ParseStates = RS485_START_STATE;
__data RS485_CALLBACK_S  rs485_callback[RS485_CALLBACK_BUFFER_SIZE];



void RS485_StoreParam( uint8_t param )
{
  rs485_param[rs485_param_head++] = param;
  if( rs485_param_head >= RS485_PARAM_BUFFER_SIZE )     rs485_param_head = 0;
}




void GetDeviceID( void )
{
}
void SetDeviceID( void )
{
}
void GetCalibrateSettings( void )
{
  uint8_t buffer[256], ptr;
  
  ptr = 0;
  
  
  
  LeUART_SendCmd ( RS485_GET_CALIBRATE_SETTINGS, buffer, ptr );
    //LeUART_TxEnable();
  LeUART_StartTransmitt();
}
void StartCalibrate( void )
{
}
void GetTarifSettings( void )
{
}
void SetTarif( void )
{
}
void GetArchDailyCnt( void )
{
}
void GetArchLastDaily( void )
{
}
void GetArchLdProfileCnt( void )
{
}
void GetArchLastLdProfile( void )
{
}
void GetArchQualityCnt( void )
{
}
void GetArchLastQuality( void )
{
}
void GetArchEventCnt( void )
{
}
void GetArchLastEvent( void )
{
}
void GetLocalTime( void )
{
}
void SetLocalTime( void )
{
}
void GetAdders( void )
{
}
void WriteUpdate( void )
{
}
void CheckUpdate( void )
{
}
void ProceedUpdate( void )
{
}



void RS485_Init( void )
{
  LeUART_Init(); 
  RS485_StartNew();
  
  memset( &rs485_callback, 0, sizeof(rs485_callback) );
  
  RS485_SetExecute( RS485_GET_DEVICE_ID, &GetDeviceID );
  RS485_SetExecute( RS485_SET_DEVICE_ID, &SetDeviceID );

  RS485_SetExecute( RS485_GET_CLIBRATE_SETTINGS, &GetCalibrateSettings );
  RS485_SetExecute( RS485_START_CALIBRATE, &StartCalibrate );

  RS485_SetExecute( RS485_GET_TARIF_SETTINGS, &GetTarifSettings );
  RS485_SetExecute( RS485_SET_TARIF, &SetTarif );  

  RS485_SetExecute( RS485_GET_ARCH_DAILY_CNT, &GetArchDailyCnt );
  RS485_SetExecute( RS485_GET_ARCH_LAST_DAILY, &GetArchLastDaily );

  RS485_SetExecute( RS485_GET_ARCH_LD_PROFILE_CNT, &GetArchLdProfileCnt );
  RS485_SetExecute( RS485_GET_ARCH_LAST_LD_PROFILE, &GetArchLastLdProfile );

  RS485_SetExecute( RS485_GET_ARCH_QUALITY_CNT, &GetArchQualityCnt );
  RS485_SetExecute( RS485_GET_ARCH_LAST_QUALITY, &GetArchLastQuality );

  RS485_SetExecute( RS485_GET_ARCH_EVENT_CNT, &GetArchEventCnt );
  RS485_SetExecute( RS485_GET_ARCH_LAST_EVENT, &GetArchLastEvent );

  RS485_SetExecute( RS485_GET_LOCAL_TIME, &GetLocalTime );
  RS485_SetExecute( RS485_SET_LOCAL_TIME, &SetLocalTime );

  RS485_SetExecute( RS485_GET_ADDERS, &GetAdders );   

  RS485_SetExecute( RS485_WRITE_UPDATE, &WriteUpdate );
  RS485_SetExecute( RS485_CHECK_UPDATE, &CheckUpdate );
  RS485_SetExecute( RS485_PROCEED_UPDATE, &ProceedUpdate );
  
}

void RS485_SetExecute( RS485_CMD_E cmd, RS485_EXECUTE callback )
{
  size_t i;
  
	for( i=0; i < RS485_CALLBACK_BUFFER_SIZE; i++ ) {
		if( rs485_callback[i].cmd == cmd ) {
			rs485_callback[i].callback = callback;
			return;
		} else if( rs485_callback[i].cmd == RS485_NO_CMD ) {
                  break;
                }
	}
        if( i >= RS485_CALLBACK_BUFFER_SIZE ) return;
        
        rs485_callback[i].cmd = cmd;
        rs485_callback[i].callback = callback;
}

void RS485_StartNew( void )
{
  ParseStates = RS485_START_STATE;

}

void RS485_Parse( uint8_t data )
{
  	switch( ParseStates ) {
		case RS485_START_STATE:
			if(data == RS485_START)
				ParseStates = RS485_RECEIVING_STATE;
		break;

		case RS485_RECEIVING_STATE:
			if(data == RS485_ESC)
				ParseStates = RS485_ESC_STATE;
			else if(data == RS485_END) {
				ParseStates = RS485_START_STATE;
                                mQueue_PutMsg(RS_485_CMD_EVENT, 0);
				return;
			} else RS485_StoreParam( data );
		break;

		case RS485_ESC_STATE:
			RS485_StoreParam( data ^ 0xFF );
			ParseStates = RS485_RECEIVING_STATE;
		break;
	}

}


void  RS485_ExecuteCmd( void )
{

}

