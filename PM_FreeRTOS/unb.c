#include "unb.h"
#include "semtech.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_chip.h"
#include "em_usart.h"
#include "em_cmu.h"
#include "bsp.h"
#include "bsp_trace.h"

RADIO_settings_t UNB_settings =
{
	0x3BEEF,
	2,
	60,
	PWR_MODE_ALWAYS_MAX,
	868800000-25000,
	868800000+25000,
	100
};

unsigned long FreqLow=868800000-25000;
unsigned long FreqHigh=868800000+25000;
volatile int Channels=100;

extern void delay(uint32_t msec);

unsigned char UNBsendInLORA(uint8_t *message, uint8_t length,long freq)
{
	//SPI config
	SPI_setup();
	delay(50);
	ConfigGPIOforSemtech();
	ResetRF();
	//RF reset
	delay(50);
	//RF switch power on
	RFswitchPowerON();
	delay(50);
	SendInLORA((float)freq + XTAL_FIX,message,length);
	delay(100);
	RFswitchPowerOFF();
	return 0;
}


int setFreq=0;
//send

unsigned char UNBsend(uint8_t *message, uint8_t length,  unsigned long serial,uint8_t iter,unsigned char confirmationNeeded,uint8_t protocol)
{
	//SPI config
	SPI_setup();
	delay(50);
	ConfigGPIOforSemtech();
	//RF reset
	ResetRF();
	delay(50);
	//RF switch power on
	RFswitchPowerON();
	delay(50);

	for(int i=0;i<3;i++)
	{
		uint32_t freqReg;
		//setFreq += serial%(Channels-1);
		setFreq += 31;
		if(setFreq > (Channels-2)){
			setFreq = setFreq%(Channels-1);
		}
		//freqReg = freq915[setFreq];
		freqReg = FreqLow + XTAL_FIX + ((FreqHigh - FreqLow)/Channels)*(setFreq+1);

		SendWithFX(serial,iter,(float)freqReg,message,length,confirmationNeeded,protocol);
		delay(10);
	}

	RFswitchPowerOFF();
	return 0;
}

int UNBreceive(unsigned char *message,unsigned char length,unsigned char* RSSI,unsigned char* curRSSI,unsigned char *SNR,long freq, uint32_t address)
{
	int res=0;
	//SPI config
	SPI_setup();
	delay(10);
	ConfigGPIOforSemtech();
	//RF switch power on
	RFswitchPowerON();
	delay(10);
	//RF reset
	ResetRF();
	delay(10);

	//receive data to packet[]
	res= ReceiveInLORA((float)freq + XTAL_FIX,message,RSSI,curRSSI,SNR,length, address);

	RFswitchPowerOFF();
	return res;
}

int UNB_FrequencyConf(unsigned long freqLow, unsigned long freqHigh, int channels)
{
	FreqLow=freqLow;
	FreqHigh=freqHigh;
	Channels=channels;

	return 1;
}

extern uint8_t RSSI,curRSSI,SNR;

uint8_t UNBsend2(uint8_t *packet, uint8_t length, uint8_t* packetRec, unsigned long serial, uint8_t protocol)
{
	static uint8_t iter=0;
	uint8_t val;
	long freq = 869800000;//TODO make freq dependent on serial

	if(protocol == PROTOCOL_B)
	{
		UNBsend(packet,length,serial,iter,0,0);//protocol B
		return 0;
	}
	else if (protocol == PROTOCOL_E)
	{
		for(int j=0;j<3;j++)
		{
			UNBsend(packet,length,serial,iter,1,1);//protocol E

			delay(4000);

			#define BASE_FREQUENCY 868700000
			#define BANDWIDTH 31250
			freq = BASE_FREQUENCY + (BANDWIDTH / 2) + ((serial % 16)*BANDWIDTH);

			//freq = 870000000;

			val = UNBreceive(packetRec,250,&RSSI,&curRSSI,&SNR,freq, UNB_settings.ID);//TODO make serial filtering inside

			if(val != 0)
			{
				iter++;
				return val;
			}
		}
		return 0;
	}
	return 0;
}

//------------------------S.Om. refactoring
uint8_t UNB_Init (RADIO_settings_t *init)
{
	if(!init)
	{
		init = &UNB_settings;	//UNB_Init(0) to set defaults
	}
	else
	{
		UNB_settings = *init;	//save settings
	}
	SPI_setup();//set USART0 to SPI mode for SX1276

	UNB_FrequencyConf(init->unb_freq_start, init->unb_freq_end, init->unb_number_of_channels);

	GPIO_PinModeSet(gpioPortF,2, gpioModePushPull, 1);//SX1276 POWER EN
	delay(10);
	GPIO_PinModeSet(gpioPortB,14, gpioModePushPull, 0);//SX1276 RESET
	delay(10);
	GPIO_PinModeSet(gpioPortB,14, gpioModeInput,0);//SX1276 RESET

	return 1;
}

extern uint8_t packetRec[];



uint8_t UNB_SendPacket (uint8_t protocol, uint8_t *tx_packet, uint8_t tx_length)
{
	static uint32_t iter = 0;
	long freq = 869800000;//TODO make freq dependent on serial

	if(protocol == PROTOCOL_B)
	{
		UNBsend(tx_packet, tx_length, UNB_settings.ID, (uint8_t)(iter & 0xFF),0,0);//protocol B
		return 0;
	}
	else if (protocol == PROTOCOL_E)
	{
		for(int j=0;j<3;j++)
		{
			UNBsend(tx_packet, tx_length, UNB_settings.ID , (uint8_t)(iter & 0xFF),1,1);//protocol E

			//delay(4000);

			#define BASE_FREQUENCY 868700000
			#define BANDWIDTH 31250
			freq = BASE_FREQUENCY + (BANDWIDTH / 2) + ((UNB_settings.ID % 16)*BANDWIDTH);

			freq = 869000000;
			uint8_t len = UNBreceive(packetRec,250,&RSSI,&curRSSI,&SNR,freq, UNB_settings.ID);//TODO make serial filtering inside

			if(len != 0)
			{
				iter++;
				return len;
			}
		}
		return 0;
	}
	return 0;
}