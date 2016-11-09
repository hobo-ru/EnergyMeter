#include "em_device.h"
#include "em_gpio.h"
#include "em_chip.h"
#include "em_usart.h"
#include "em_cmu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "semtech.h"
#include "unb.h"


////////////////////
// Init SPI
///////////////////
/* Defines */

#include "FreeRTOS.h"
#include "task.h"
extern uint32_t sTicks;

#define HFRCO_FREQUENCY         14000000
#define SPI_PERCLK_FREQUENCY    HFRCO_FREQUENCY
#define SPI_BAUDRATE            1000000

#define MAXLENGTH 100

unsigned char bits[3000];
unsigned char pre[3]={0xCC,0xDD,0xEE};
unsigned char message[MAXLENGTH];
unsigned char tempBuf[MAXLENGTH];
long ends[4]={384,384*2,384*3,1320};

extern void delay(uint32_t msec);
//extern uint32_t sTicks;

uint32_t digital_update_crc32( uint32_t crc, const uint8_t *data, uint8_t len )
{
	static const uint32_t table[256] = {
		0x00000000U,0x04C11DB7U,0x09823B6EU,0x0D4326D9U,
		0x130476DCU,0x17C56B6BU,0x1A864DB2U,0x1E475005U,
		0x2608EDB8U,0x22C9F00FU,0x2F8AD6D6U,0x2B4BCB61U,
		0x350C9B64U,0x31CD86D3U,0x3C8EA00AU,0x384FBDBDU,
		0x4C11DB70U,0x48D0C6C7U,0x4593E01EU,0x4152FDA9U,
		0x5F15ADACU,0x5BD4B01BU,0x569796C2U,0x52568B75U,
		0x6A1936C8U,0x6ED82B7FU,0x639B0DA6U,0x675A1011U,
		0x791D4014U,0x7DDC5DA3U,0x709F7B7AU,0x745E66CDU,
		0x9823B6E0U,0x9CE2AB57U,0x91A18D8EU,0x95609039U,
		0x8B27C03CU,0x8FE6DD8BU,0x82A5FB52U,0x8664E6E5U,
		0xBE2B5B58U,0xBAEA46EFU,0xB7A96036U,0xB3687D81U,
		0xAD2F2D84U,0xA9EE3033U,0xA4AD16EAU,0xA06C0B5DU,
		0xD4326D90U,0xD0F37027U,0xDDB056FEU,0xD9714B49U,
		0xC7361B4CU,0xC3F706FBU,0xCEB42022U,0xCA753D95U,
		0xF23A8028U,0xF6FB9D9FU,0xFBB8BB46U,0xFF79A6F1U,
		0xE13EF6F4U,0xE5FFEB43U,0xE8BCCD9AU,0xEC7DD02DU,
		0x34867077U,0x30476DC0U,0x3D044B19U,0x39C556AEU,
		0x278206ABU,0x23431B1CU,0x2E003DC5U,0x2AC12072U,
		0x128E9DCFU,0x164F8078U,0x1B0CA6A1U,0x1FCDBB16U,
		0x018AEB13U,0x054BF6A4U,0x0808D07DU,0x0CC9CDCAU,
		0x7897AB07U,0x7C56B6B0U,0x71159069U,0x75D48DDEU,
		0x6B93DDDBU,0x6F52C06CU,0x6211E6B5U,0x66D0FB02U,
		0x5E9F46BFU,0x5A5E5B08U,0x571D7DD1U,0x53DC6066U,
		0x4D9B3063U,0x495A2DD4U,0x44190B0DU,0x40D816BAU,
		0xACA5C697U,0xA864DB20U,0xA527FDF9U,0xA1E6E04EU,
		0xBFA1B04BU,0xBB60ADFCU,0xB6238B25U,0xB2E29692U,
		0x8AAD2B2FU,0x8E6C3698U,0x832F1041U,0x87EE0DF6U,
		0x99A95DF3U,0x9D684044U,0x902B669DU,0x94EA7B2AU,
		0xE0B41DE7U,0xE4750050U,0xE9362689U,0xEDF73B3EU,
		0xF3B06B3BU,0xF771768CU,0xFA325055U,0xFEF34DE2U,
		0xC6BCF05FU,0xC27DEDE8U,0xCF3ECB31U,0xCBFFD686U,
		0xD5B88683U,0xD1799B34U,0xDC3ABDEDU,0xD8FBA05AU,
		0x690CE0EEU,0x6DCDFD59U,0x608EDB80U,0x644FC637U,
		0x7A089632U,0x7EC98B85U,0x738AAD5CU,0x774BB0EBU,
		0x4F040D56U,0x4BC510E1U,0x46863638U,0x42472B8FU,
		0x5C007B8AU,0x58C1663DU,0x558240E4U,0x51435D53U,
		0x251D3B9EU,0x21DC2629U,0x2C9F00F0U,0x285E1D47U,
		0x36194D42U,0x32D850F5U,0x3F9B762CU,0x3B5A6B9BU,
		0x0315D626U,0x07D4CB91U,0x0A97ED48U,0x0E56F0FFU,
		0x1011A0FAU,0x14D0BD4DU,0x19939B94U,0x1D528623U,
		0xF12F560EU,0xF5EE4BB9U,0xF8AD6D60U,0xFC6C70D7U,
		0xE22B20D2U,0xE6EA3D65U,0xEBA91BBCU,0xEF68060BU,
		0xD727BBB6U,0xD3E6A601U,0xDEA580D8U,0xDA649D6FU,
		0xC423CD6AU,0xC0E2D0DDU,0xCDA1F604U,0xC960EBB3U,
		0xBD3E8D7EU,0xB9FF90C9U,0xB4BCB610U,0xB07DABA7U,
		0xAE3AFBA2U,0xAAFBE615U,0xA7B8C0CCU,0xA379DD7BU,
		0x9B3660C6U,0x9FF77D71U,0x92B45BA8U,0x9675461FU,
		0x8832161AU,0x8CF30BADU,0x81B02D74U,0x857130C3U,
		0x5D8A9099U,0x594B8D2EU,0x5408ABF7U,0x50C9B640U,
		0x4E8EE645U,0x4A4FFBF2U,0x470CDD2BU,0x43CDC09CU,
		0x7B827D21U,0x7F436096U,0x7200464FU,0x76C15BF8U,
		0x68860BFDU,0x6C47164AU,0x61043093U,0x65C52D24U,
		0x119B4BE9U,0x155A565EU,0x18197087U,0x1CD86D30U,
		0x029F3D35U,0x065E2082U,0x0B1D065BU,0x0FDC1BECU,
		0x3793A651U,0x3352BBE6U,0x3E119D3FU,0x3AD08088U,
		0x2497D08DU,0x2056CD3AU,0x2D15EBE3U,0x29D4F654U,
		0xC5A92679U,0xC1683BCEU,0xCC2B1D17U,0xC8EA00A0U,
		0xD6AD50A5U,0xD26C4D12U,0xDF2F6BCBU,0xDBEE767CU,
		0xE3A1CBC1U,0xE760D676U,0xEA23F0AFU,0xEEE2ED18U,
		0xF0A5BD1DU,0xF464A0AAU,0xF9278673U,0xFDE69BC4U,
		0x89B8FD09U,0x8D79E0BEU,0x803AC667U,0x84FBDBD0U,
		0x9ABC8BD5U,0x9E7D9662U,0x933EB0BBU,0x97FFAD0CU,
		0xAFB010B1U,0xAB710D06U,0xA6322BDFU,0xA2F33668U,
		0xBCB4666DU,0xB8757BDAU,0xB5365D03U,0xB1F740B4U,
	};

	while (len > 0)
	{
		crc = table[*data ^ ((crc >> 24) & 0xff)] ^ (crc << 8);
		data++;
		len--;
	}
	return crc;
}

uint32_t digital_crc32(const uint8_t *buf, uint8_t len)
{
	return digital_update_crc32(0xffffffff, buf, len) ^ 0xffffffff;
}

int FormMessage(unsigned long ID,unsigned char iter,unsigned char* buf,int length,unsigned char confirmationNeeded,unsigned char protocol)
{
	int L=0;
	unsigned char i=0;
	unsigned char ii=0;
	long j=0;
	unsigned char temp=0;

	uint32_t crc=0;
	unsigned char len=0;//compressed len bits
	//        wanna send    0 1 2 3 4 5 6 7 8 9 10 11  12   //13  14  15  16  17  18  19  20
//	unsigned char lens[21]={0,0,1,2,3,4,5,6,7,8,9, 10, 11 };//11, 12, 12, 13, 13, 14, 14, 15};

	unsigned char lens[21]={0,0,0,1,2,3,4,5,6,7,8,9,10,11,11,12,12,13,13,14,14};

	if(length == 0)//EA's idea to add service type with len=0xF, actual length of 10 bits
	{
		len = 0xF;
		length = 10;//TODO define service msg length
	}
	else
	{
		len=lens[length];
	}

	if(protocol==0)
	{
		pre[0]=0xAA;
		pre[1]=0xF0;
		pre[2]=0x99;
	}
	if(protocol==1)
	{
		pre[0]=0xCC;
		pre[1]=0xDD;
		pre[2]=0xEE;
	}

	if(protocol==0)
	{
		message[3]=(ID&0xFF);ID=(ID>>8);//EA's idea to shorten ID
	}
	message[2]=(ID&0xFF);ID=(ID>>8);
	message[1]=(ID&0xFF);ID=(ID>>8);
	message[0]=(ID&0xFF);ID=(ID>>8);

	for(j=0;j<length;j++)
	{
		if(protocol==0) 	message[j+4]=buf[j];
		else 				message[j+3]=buf[j];//EA's idea to shorten ID
	}

	if(protocol==1)
	{
		tempBuf[0]=len+((iter&0x0F)<<4);
	}
	else
	{
		tempBuf[0]=len;
	}
	tempBuf[1]=message[0];
	tempBuf[2]=message[1];
	tempBuf[3]=message[2];
	if(protocol==0) tempBuf[4]=message[3];//EA's idea to shorten ID

	for(j=0;j<length;j++)
	{
		if(protocol==0)		tempBuf[j+5]=buf[j];
		else 				tempBuf[j+4]=buf[j];//EA's idea to shorten ID
	}

	if(protocol == 0)
	{
		crc=digital_crc32(tempBuf,5+length);
		message[7+length]=(crc&0xFF);crc=(crc>>8);
		message[6+length]=(crc&0xFF);crc=(crc>>8);
		message[5+length]=(crc&0xFF);crc=(crc>>8);
		message[4+length]=(crc&0xFF);crc=(crc>>8);
	}
	else if(protocol == 1)
	{
		crc=digital_crc32(tempBuf,4+length);//EA's idea to shorten ID
		message[6+length]=(crc&0xFF);crc=(crc>>8);
		message[5+length]=(crc&0xFF);crc=(crc>>8);
		message[4+length]=(crc&0xFF);crc=(crc>>8);
		message[3+length]=(crc&0xFF);crc=(crc>>8);
	}

	for(i=0;i<48;i++)
	{
		bits[L]=0;
		L++;
	}
	// 1 and 0
	for(i=0;i<12;i++)
	{
		bits[L]=0;
		L++;
	}
	for(i=0;i<12;i++)
	{
		if(i==0)bits[L]=1;
		else bits[L]=0;
		L++;
	}
	//preambule
	for(ii=0;ii<3;ii++)
	{
		temp=pre[ii];

		for(j=0;j<8;j++)
		{
			if((temp&0x80)==0)
			{
				for(i=0;i<12;i++)
				{
					if(i==0)bits[L]=1;
					else bits[L]=0;
					L++;
				}
			}else
			{
				for(i=0;i<12;i++)
				{
					bits[L]=0;
					L++;
				}
			}
			temp=(temp<<1);
		}
	}
	//4 or 8 bits 0

	if(protocol==1)
	{
		temp=len+((iter&0x0F)<<4);
		for(ii=0;ii<8;ii++)
		{
			if((temp&0x80)==0)
			{
				for(i=0;i<12;i++)
				{
					if(i==0)bits[L]=1;
					else bits[L]=0;
					L++;
				}
			}else
			{
				for(i=0;i<12;i++)
				{
					bits[L]=0;
					L++;
				}
			}
			temp=(temp<<1);
		}
	}
	else
	{
		temp=len;
		for(ii=0;ii<4;ii++)
		{
			if((temp&0x08)==0)
			{
				for(i=0;i<12;i++)
				{
					if(i==0)bits[L]=1;
					else bits[L]=0;
					L++;
				}
			}else
			{
				for(i=0;i<12;i++)
				{
					bits[L]=0;
					L++;
				}
			}
			temp=(temp<<1);
		}
	}
	//message

	//for(ii=0;ii<8+length;ii++)
	for(ii=0; ii < ((protocol)?7:8) + length; ii++)//EA's idea to shorten ID
	{
		temp=message[ii];

		for(j=0;j<8;j++)
		{
			if((temp&0x80)==0)
			{
				for(i=0;i<12;i++)
				{
					if(i==0)bits[L]=1;
					else bits[L]=0;
					L++;
				}
			}else
			{
				for(i=0;i<12;i++)
				{
					bits[L]=0;
					L++;
				}
			}
			temp=(temp<<1);
		}
		temp=temp;
	}

	return L;
}


void SendWithFX(unsigned long ID,unsigned char iter,float freq,unsigned char* packet,int length,unsigned char confirmationNeeded,unsigned char protocol)
{
	long packetLength=165;
	unsigned char val;
	unsigned char temp;
	int i=0;
	int ii=0;
	int jj=0;
	int j=0;
	int L=0;


	{
		ConfigSemtechFX();
		SetFreq(freq,0);
		delay(200);
		packetLength=FormMessage(ID,iter,packet,length,confirmationNeeded,protocol)/8+1;

		writeReg(0x32,packetLength & 0xFF);			//packetLength 7:0 bits
		writeReg(0x31,0x40 | (packetLength >> 8));	//packetLength 10:8 bits

		//GPIO_PinOutSet(gpioPortD,7);//led

		SetMode(FSTx_MODE);
		delay(10);

		L=0;
		for(i=0;i<50;i++)
		{
			//send
			temp=0;
			ii=7;
			for(j=0;j<8;j++)
			{
				temp+=(bits[L]<<ii);
				L++;
				ii--;
			}
			writeReg(0x00,temp);
		}

		SetMode(Tx_MODE);

		for(j=0;j<packetLength-50;j++)
		{
			val=readReg(0x3F);
			while((val&0x20)!=0)
			{
				delay(1);
				val=readReg(0x3F);
			}

			//send
			temp=0;
			ii=7;
			for(jj=0;jj<8;jj++)
			{
				temp+=(bits[L]<<ii);
				L++;
				ii--;
			}
			writeReg(0x00,temp);
		}

		delay(1);



		val=readReg(0x3F);;
		i=0;
		while(!(val&0x08))//(val&0x07)!=STANDBY_MODE)
		{
			//val=readReg(RegOpMode);
			val=readReg(0x3F);
			i++;
		}

		//writeReg(0x06,0xAA);
		//delay(1);
		val=readReg(0x06);
		SetMode(STANDBY_MODE);
		//GPIO_PinOutClear(gpioPortD,7);//led
	}
}

void SendWithFX_carrier(float freq, uint32_t ms)
{
	//unsigned char val;
	//int i=0;



	ConfigSemtechFX();
	SetFreq(freq,0);

	writeReg(0x25,0);
	writeReg(0x26,0);
	writeReg(0x30,0);
	writeReg(0x32,0);

	//GPIO_PinOutSet(gpioPortD,7);//led

	SetMode(FSTx_MODE);
	delay(10);
//	writeReg(0x00,0);
//	writeReg(0x00,0);
//	writeReg(0x00,0);
//	writeReg(0x00,0);
//	writeReg(0x00,0);
//	writeReg(0x00,0);

	SetMode(Tx_MODE);

//	while(1)
//	{
//		while((val&0x20)!=0)
//		{
//			delay(1);
//			val=readReg(0x3F);
//		}
//
//		writeReg(0x00,0);
//		writeReg(0x00,0);
//		writeReg(0x00,0);
//		writeReg(0x00,0);
//		writeReg(0x00,0);
//		writeReg(0x00,0);
//	}

	if(ms)
	{
		delay(ms);

		//		val=readReg(0x3F);;
		//		i=0;
		//		while(!(val&0x08))//(val&0x07)!=STANDBY_MODE)
		//		{
		//			//val=readReg(RegOpMode);
		//			val=readReg(0x3F);
		//			i++;
		//		}

		//writeReg(0x06,0xAA);
		//delay(1);

		//val=readReg(0x06);
		SetMode(STANDBY_MODE);
		//GPIO_PinOutClear(gpioPortD,7);//led
	}
}

void SendWithFX_carrier_stop()
{
	SetMode(STANDBY_MODE);
}

void SendInLORA(float freq,unsigned char *packet,int length)
{
	int i=0;
	unsigned char val=0;

	//LORA
	ConfigSemtech();

	SetMode(SLEEP_MODE);
	delay(10);
	writeReg(0x22,length);//packet length
	SetMode(STANDBY_MODE);

	SetFreq(freq,0);
	delay(100);

	SetMode(FSTx_MODE);
	delay(10);
	writeReg(0x0D,0x80);//set Set FifoAddrPtr to FifoTxBaseAddrs.

	//Write PayloadLength bytes to the FIFO (RegFifo)
	for(i=0;i<length;i++)
	{
		writeReg(0x00,packet[i]);
	}

	SetMode(Tx_MODE);

	delay(1);

	val=readReg(RegOpMode);

	while((val&0x07)!=STANDBY_MODE)
	{
		val=readReg(RegOpMode);
	}
	delay(20);
	SetMode(STANDBY_MODE);

}
int wrongCRCcount=0;
int timeoutCount=0;
int okCount=0;
#define LORA_RECEIVE_TIMEOUT	(60000)
unsigned char temp;
int ReceiveInLORA(float freq,unsigned char *packet,unsigned char* RSSI,unsigned char* curRSSI,unsigned char* SNR, int length, uint32_t address)
{
	int ok;
	int i=0;
	writeReg(0x22,length);//packet length

	//LORA
	ConfigSemtech();
	SetFreq(freq,0);
	delay(10);//??

	uint32_t timeout = sTicks;

//RX:	//RX
	ok=0;
	{
		writeReg(0x0D,0x00);//set Set FifoAddrPtr to FifoRxBaseAddr.

		SetMode(FSRx_MODE);
		delay(20);

		SetMode(RxCont_MODE);
		delay(50);
		*curRSSI=readReg(0x1B);

		temp=0;
		while(((sTicks - timeout) < 90000)&&((temp&0x10)!=0x10))// wait valid header
		{
			delay(50);
			temp=readReg(0x12);
		}

		if((temp&0x10)==0x10)//RX ongoing
		{
			//timeout = sTicks;//started receiving packet, reset timeout
			temp=0;
			while(((temp&0xC0)==0)&&((sTicks - timeout) < 90000))
			{
				temp=readReg(0x12);
				delay(100);
			}
		}
		else temp=0;

		if(((temp&0x40)!=0)/*&&(((temp&0x20)==0))*/)//if rx done and crc ok
		{
			*RSSI=readReg(0x1A);
			*SNR=readReg(0x19);
			length=readReg(0x13);
			for(i=0;i<length;i++)
			{
				packet[i]=readReg(0x00);
			}
			SetMode(SLEEP_MODE);
			delay(10);
			SetMode(STANDBY_MODE);

//			//address filter
//			if((address != 0) && (memcmp(packet + LORA_ADDRESS_OFFSET, &address, LORA_ADDRESS_LENGTH) != 0))
//			{
//				//address mismatch, get back into RX
//				goto RX;
//			}

			ok=length;
			okCount++;

			if((temp&0x20)!=0)
			{
				wrongCRCcount = 1;
			}
			else
			{
				wrongCRCcount = 0;
			}
		}
		else
		{
			if((temp&0x80)!=0)
			{
				timeoutCount++;
			}
			if((temp&0x20)!=0)
			{
				wrongCRCcount = 1;
			}
		}
	}

	writeReg(0x12, 0xFF);// clear radio interrupts
	temp=readReg(0x12);
	SetMode(STANDBY_MODE);

	return ok;
}

void SPI_setup(void)
{
	USART_TypeDef *spi=USART0;

	CMU_ClockEnable(cmuClock_USART0, true);

	/* Setting baudrate */
	spi->CLKDIV = 1024;

	/* Configure SPI */
	/* Using synchronous (SPI) mode*/
	spi->CTRL = USART_CTRL_SYNC| USART_CTRL_MSBF;
	/* Clearing old transfers/receptions, and disabling interrupts */
	spi->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
	spi->IEN = 0;
	/* Enabling pins and setting location */
	spi->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN ;

	/* Enabling Master, TX and RX */
	spi->CMD   = USART_CMD_MASTEREN | USART_CMD_TXEN | USART_CMD_RXEN;
	spi->CTRL |= USART_CTRL_AUTOCS;

	/* Set GPIO config to master */
	GPIO_Mode_TypeDef gpioModeMosi = gpioModePushPull;
	GPIO_Mode_TypeDef gpioModeMiso = gpioModeInput;
	GPIO_Mode_TypeDef gpioModeCs   = gpioModePushPull;
	GPIO_Mode_TypeDef gpioModeClk  = gpioModePushPull;

	/* Clear previous interrupts */
	spi->IFC = _USART_IFC_MASK;

	GPIO_PinModeSet(gpioPortE, 10, gpioModeMosi, 0); /* MOSI */
	GPIO_PinModeSet(gpioPortE, 11, gpioModeMiso, 0); /* MISO */
	GPIO_PinModeSet(gpioPortE, 12, gpioModeCs,   0); /* CS */
	GPIO_PinModeSet(gpioPortE, 13, gpioModeClk,  0); /* Clock */

	/* Enable interrupts */
	NVIC_ClearPendingIRQ(USART0_RX_IRQn);
	NVIC_ClearPendingIRQ(USART0_TX_IRQn);
	NVIC_DisableIRQ(USART0_RX_IRQn);
}

void ResetRF()
{
	GPIO_PinModeSet(gpioPortB,14, gpioModePushPull, 0);
	GPIO_PinOutClear(gpioPortB,14);//RF reset
	delay(1);
	GPIO_PinModeSet(gpioPortB,14, gpioModeInput,0);
}

void RFswitchPowerON()
{
	GPIO_PinModeSet(gpioPortF,2, gpioModePushPull, 0);
	GPIO_PinOutSet(gpioPortF,2);

}
void RFswitchPowerOFF()
{
	GPIO_PinModeSet(gpioPortF,2, gpioModePushPull, 0);
	GPIO_PinOutClear(gpioPortF,2);
}

void ConfigGPIOforSemtech()
{
	//GPIO_PinModeSet(gpioPortD,5, gpioModePushPull, 0);
	//GPIO_PinModeSet(gpioPortD,4, gpioModeInputPull, 0);//TODO what for? its uart
	//GPIO_PinOutClear(gpioPortD,4);

	GPIO_PinModeSet(gpioPortF,2, gpioModePushPull, 0);
}

uint8_t SPI_readByte()
{
	USART_TypeDef *spi = USART0;
	uint8_t       rxdata;

	if (spi->STATUS & USART_STATUS_RXDATAV)
	{
		/* Reading out data */
		rxdata = spi->RXDATA;
	}
	return rxdata;
}
void SPI_sendBuffer(char* txBuffer, int bytesToSend)
{
	USART_TypeDef *uart = USART0;
	int           ii;

	/* Sending the data */
	for (ii = 0; ii < bytesToSend;  ii++)
	{
		/* Waiting for the usart to be ready */
		while (!(uart->STATUS & USART_STATUS_TXBL)) ;

		if (txBuffer != 0)
		{
			/* Writing next byte to USART */
			uart->TXDATA = *txBuffer;
			txBuffer++;
		}
		else
		{
			uart->TXDATA = 0;
		}
	}

	/*Waiting for transmission of last byte */
	while (!(uart->STATUS & USART_STATUS_TXC)) ;
}


void SetMode(unsigned char mode)
{
	unsigned char prev=0;
	prev=readReg(RegOpMode);
	prev=(prev&0xF8);
	prev=(prev|mode);
	delay(1);
	writeReg(RegOpMode,prev);
}

void SetFreq(float freq,int channel)
{
	float reg=0;
	uint32_t regI=0;
	unsigned char u1,u2,u3;
	reg=freq/(32000000.0)*524288.0;


	regI=(uint32_t)reg;
	regI+=channel;

	u1=(unsigned char)((regI>>16)&0xFF);
	u2=(unsigned char)((regI>>8)&0xFF);
	u3=(unsigned char)((regI)&0xFF);
	writeReg(RegFrMsb,u1);
	writeReg(RegFrMid,u2);
	writeReg(RegFrLsb,u3);
}


unsigned char v=0;
void ConfigSemtechFX()
{
	SetMode(SLEEP_MODE);
	delay(10);
	writeReg(0x4B,0x19);//0x19 - TCXO, 0x09 - кварц
	v=readReg(0x4B);

	//set FX mode
	writeReg(RegOpMode,0);//high frequency mode enabled
	v=readReg(RegOpMode);//high frequency mode enabled
	delay(1);

	//clk out /32
	writeReg(0x24,0x05);

	//bitrate 1200bod
	writeReg(0x02,0x68);
	writeReg(0x03,0x2A);
	writeReg(0x5D,11);

	//fdev +-300Hz
	writeReg(0x04,0x00);
	writeReg(0x05,0x05);

	//868.8MHz
	writeReg(RegFrMsb,0xD9);
	writeReg(RegFrMid,0x33);
	writeReg(RegFrLsb,0x33);

	//max power
	//writeReg(RegPaConfig,0x7F);//14dBm on RFO_HF
	writeReg(RegPaConfig,0xFF);//17dBm on PA_BOOST
	writeReg(0x0B,0x00);//disable ocp

	//preambule length
	writeReg(0x25,0);
	writeReg(0x26,0x02);

	//disable sync word
	writeReg(0x27,0);
	//packet config
	writeReg(0x30,0);
	writeReg(0x31,0x40);
	writeReg(0x32,50);
	//v=readReg(0x5A);
	//writeReg(0x5A,0x87);

	writeReg(0x36,0x04);//from transmit to idle state

	writeReg(0x4D,0x07);//pa 20dBm on

	writeReg(0x40,0x35);
	writeReg(0x41,0x80);
	v=readReg(0x32);

	v=v;
	SetMode(STANDBY_MODE);

	v=readReg(RegOpMode);//high frequency mode enabled

}

void ConfigSemtech()
{
	SetMode(SLEEP_MODE);
	delay(10);

	//set LORA mode and HF
	writeReg(RegOpMode,0x80|SLEEP_MODE);
	writeReg(0x40,0xff);//diomapping - all off
	writeReg(0x41,0xf0);
	delay(1);

	writeReg(0x4B,0x19);//0x19 - TCXO, 0x09 - кварц
	v=readReg(0x4B);

	//max power
	writeReg(RegPaConfig,0xFF);//17dBm on PA_BOOST
	writeReg(0x4D,0x07);//pa 20dBm on.
	writeReg(0x0B,0x00);//disable ocp

	writeReg(RegModemConfig1,0x54);//44kHz 4/6 expicit header
	writeReg(RegModemConfig2,0xC7);//12 SF and CRC on and timeout 3
	writeReg(0x26,(0x08 | 0x04));//low data rate optimization on + AGC gasin

	//writeReg(0x0C,0x23);//max gain

	writeReg(0x1F,0xFF);//rx timeout

	writeReg(0x20,0x00);//preambuleMsb
	writeReg(0x21,0x08);//pre lsb
	//writeReg(0x22,20);//packet length
	writeReg(0x23,250);//packet length
	writeReg(0x24,0);//hop period
	delay(1);

	SetMode(STANDBY_MODE);
}

void writeReg(uint8_t addr,uint8_t value)
{
	char b[2];
	b[0]=(0x80|addr);
	b[1]=value;
	SPI_sendBuffer(b,2);
	SPI_readByte();//??
	SPI_readByte();
}

uint8_t readReg(uint8_t addr)
{
	uint8_t temp2=0;

	char b[2];
	b[0]=addr;

	SPI_sendBuffer(b,2);
	SPI_readByte();
	temp2=SPI_readByte();
	return temp2;
}

void SendUART(char *str)
{
	int l=0;
	while(str[l]!=0)
	{
		l++;
	}
	for(int i=0;i<l;i++)
	{
		while(!(USART1->STATUS & ( 1 << 6 )))
		{}
		USART1->TXDATA=str[i];
	}

}

