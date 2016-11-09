#ifndef _MODEM_H_
#define _MODEM_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "electro_v5.h"
#include "modem_uart.h"

#define SLIP_DRX 0x0
#define SLIP_CRX 0x1
#define SLIP_FASTDL 0x2

typedef enum {
    AXEM_CMD_ECHO               = 0x00, // Эхо, модем отвечает копией переданного пакета
    AXEM_CMD_SEND_BY_B          = 0x01, // Отправить {PAYLOAD} по протоколу "B"
    AXEM_CMD_SEND_BY_C          = 0x02,	// Отправить {PAYLOAD} по протоколу "C"
    AXEM_CMD_SEND_BY_DL         = 0x03, // Отправить {PAYLOAD} по протоколу "DL" (обратная связь)
    AXEM_CMD_SET_MODE           = 0x04, // Режим приёма (\x01 - ВКЛ, \x00 - ВЫКЛ) *
    AXEM_CMD_SET_FREQUENCY      = 0x05, // Частота передачи по "DL", (uint32_t) Герц *
    AXEM_CMD_SET_ENERGY         = 0x06, // Мощность передачи, (int8_t) dBm *
    AXEM_CMD_SET_DESTINATION    = 0x07, // Адрес назначения для DL сообщения *
    AXEM_CMD_SEND_CORRECT       = 0x08, // Отправить сообщение для корректировки частоты (для БС)
    AXEM_CMD_SEND_GET_SERIAL    = 0x09, // Запросить ID - адрес
    AXEM_CMD_DL_PACKET          = 0x10, // Пакет, принятый по протоколу "DL"
    AXEM_CMD_BOOTLOAD           = 0x20, // Запустить бутлоудер
    AXEM_CMD_GET_PACKETS_TO_SEND= 0x21, // число пакетов в очереди
    AXEM_CMD_SEND_NBFi          = 0x32, // send NBFi
    AXEM_CMD_Config             = 0x40, // ?
    AXEM_CMD_SEND_FAST_DL       = 0x43, // send fast DL
} AXEM_CMD;

typedef enum {
	MODEM_READY,
    MODEM_SEND_CMD,
    MODEM_SEND_PAYLOAD,
    MODEM_WAIT_ACK,
} modem_states_e;

bool Modem_IsSending( void );
bool Modem_IsWaitingAck( void );

extern modem_states_e ModemState;
extern uint8_t DL_Mode;
extern uint8_t AXSEM_ACK_timeout;
extern uint8_t AXSEM_packets_to_send;

void AXSEM_Reset(void);
void AXSEM_Off(void);

//void Modem_SetDLPacket(uint8_t cmd, uint8_t *data, uint8_t len, uint8_t dti_interface);
void Modem_SetDLPacket(SLIP_BUF * slip);
//bool Event_Send( ArchiveEvent_e event, time_t time );

bool ModemUART_SLIP_Send(uint8_t cmd, uint8_t *payload, uint8_t len);

#endif
