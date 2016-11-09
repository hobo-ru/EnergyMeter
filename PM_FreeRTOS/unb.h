/*
* UNB.h
*
*  Created on: 06.11.2013
*      Author: офис
*/

#ifndef UNB_H_
#define UNB_H_

#include "stdint.h"

#define LORA_FREQUENCY 869200000.0//870000000.0
#define LORA_ADDRESS_OFFSET (0)
#define LORA_ADDRESS_LENGTH (3)

unsigned char UNBsend(unsigned char *message, unsigned char length, unsigned long serial,unsigned char iter, unsigned char confirm,unsigned char protocol);
unsigned char UNBsendInLORA(unsigned char *message, unsigned char length,long freq);

unsigned char UNBsend2(unsigned char *packet, unsigned char length, unsigned char* packetRec, unsigned long serial, unsigned char protocol);

int UNBreceive(unsigned char *message,unsigned char length,unsigned char* RSSI,unsigned char* curRSSI,unsigned char *SNR,long freq, uint32_t address);

enum 
{
	PROTOCOL_B = 0,
	PROTOCOL_E = 1
};

typedef enum {
	NormalPower = 0,
	HighPower = 1,
	LowPower = 2
} UNB_power_typedef;

int UNB_FrequencyConf(unsigned long freqLow, unsigned long freqHigh, int channels);
void UNB_setPower(UNB_power_typedef power);


//--------------------------------------------------------------------------------------


typedef enum
{
	PWR_MODE_ALWAYS_MAX = 0,
	PWR_MODE_LOW_IF_GOOD_LINK = 1
}pwr_mode_t;//TODO implement

typedef struct
{
	uint32_t 		ID;							// device ID
	uint8_t 		protocol_e_retries;			// number of retries in case of no ack
	uint32_t 		protocol_e_retry_period;	// period between retries in seconds (for protocol e)
	pwr_mode_t		power_mode;					// adaptive power mode
	unsigned long 	unb_freq_start;				// start of freqency band
	unsigned long 	unb_freq_end;				// end of frequency band
	int 			unb_number_of_channels;		// number of channels the band is divided in
}RADIO_settings_t;

typedef struct
{
	uint32_t address:24;
	uint32_t type:8;
	uint8_t payload[20];
}RADIO_LoRa_packet_struct;

typedef union
{
	uint8_t array[24];
	RADIO_LoRa_packet_struct packet;
}RADIO_LoRa_packet_t;

uint8_t UNB_Init (RADIO_settings_t* init);
uint8_t UNB_SendPacket (uint8_t protocol, uint8_t *tx_packet, uint8_t tx_length);
void UNB_AckCallback(uint8_t status);
uint8_t UNB_RetreiveRxPackets(uint8_t *rx_packet, uint8_t rx_length);

#endif /* UNB_H_ */