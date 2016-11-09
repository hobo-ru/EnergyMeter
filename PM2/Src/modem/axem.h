#ifndef _AXEM_H_
#define _AXEM_H_

#include "modem_uart.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>



typedef void (*AXEM_CALLBACK)(const uint8_t *data, size_t lenght);
typedef void (*AXEM_EXECUTE)(void);
typedef uint8_t (*AXEM_GET_DATA)(size_t ptr);


typedef enum {
	AXEM_SEND_ECHO_CMD		//0 - Эхо, модем отвечает копией переданного пакета
	, AXEM_SEND_BY_B_CMD		//1 - Отправить {PAYLOAD} по протоколу "B"
	, AXEM_SEND_BY_C_CMD		//2 - Отправить {PAYLOAD} по протоколу "C"
	, AXEM_SEND_BY_DL_CMD		//3 - Отправить {PAYLOAD} по протоколу "DL" (обратная связь)
	, AXEM_SET_MODE_CMD		//4 - Режим приёма (\x01 - ВКЛ, \x00 - ВЫКЛ) *
	, AXEM_SET_FREQUENCY_CMD	//5 - Частота передачи по "DL", (uint32_t) Герц *
	, AXEM_SET_ENERGY_CMD		//6 - Мощность передачи, (int8_t) dBm *
	, AXEM_SET_DESTINATION_CMD	//7 - Адрес назначения для DL сообщения *
	, AXEM_SEND_CORRECT_CMD		//8 - Отправить сообщение для корректировки частоты (для БС)
        , AXEM_SEND_GET_SERIAL_CMD      //9 - Запросить ID - адрес
	, AXEM_BOOTLOAD_CMD = 0x20      //0x20 - Запустить бутлоудер
        , AXEM_SEND_FAST_DL_CMD = 0x43  // send fast DL

}AXEM_TRANSMITT_CMD_E;

typedef enum {
	AXEM_RECEIVE_ECHO_CMD			//0 - Ответ на эхо - запрос
	, AXEM_RECEIVE_B_CMD			//1 - Статус, присылается после окончания радиопередачи
	, AXEM_RECEIVE_C_CMD			//2 - Статус, присылается после окончания радиопередачи
	, AXEM_RECEIVE_DL_CMD			//3 - Статус, присылается после окончания радиопередачи
	, AXEM_RECEIVE_MODE_CMD			//4 - Режим приёма (\x01 - ВКЛ, \x00 - ВЫКЛ) *
	, AXEM_RECEIVE_FREQUENCY_CMD		//5 - Частота передачи по "DL", (uint32_t) Герц *
	, AXEM_RECEIVE_ENERGY_CMD		//6 - Мощность передачи, (int8_t) dBm *
        , AXEM_RECEIVE_SERIAL_CMD  = 9              //9 -  ID - адрес
	, AXEM_RECEIVE_DL_PACKET_CMD = 0x10	//0x10 - Пакет, принятый по протоколу "DL"
        , AXEM_RECEIVE_RSSI_CMD = 0x55	        //0x10 - Пакет, принятый по протоколу "DL"

}AXEM_RECEIVE_CMD_E;



#define SLIP_START  0xDD
#define SLIP_END    0xDE
#define SLIP_ESC    0xDF

#define SLIP_DRX 0x0
#define SLIP_CRX 0x1
#define SLIP_FASTDL 0x2

extern uint8_t DL_Mode;


extern void AxemInit( void );
extern void AxemDeinit( void );

extern void Axem_SetReceiveCallback( UART_RECEIVE_CALLBACK Receive );
extern void Axem_SetExecuteCallback( AXEM_EXECUTE Execute );

extern bool Axem_SetReplayCallback( AXEM_RECEIVE_CMD_E code, AXEM_CALLBACK Callback );
extern AXEM_CALLBACK Axem_GetReplayCallback( AXEM_RECEIVE_CMD_E cmd );
extern void Axem_SetEnergyMode( uint8_t energy );

extern bool Axem_Send( AXEM_GET_DATA GetData, size_t lenght );
//extern bool SLIP_Send(uint8_t cmd, uint8_t *payload, uint8_t len);
extern bool Axem_ParseSlip( uint8_t slip );
extern bool Axem_ExecuteCallback( void );
extern void Axem_ThrowStatus( void );
void AxemReset();
void AxemEverySecond();

#endif