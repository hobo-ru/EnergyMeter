
#define SLEEP_MODE 0x00
#define STANDBY_MODE 0x01
#define FSTx_MODE 0x02 
#define Tx_MODE 0x03
#define FSRx_MODE 0x04
#define RxCont_MODE 0x05
#define RxSingle_MODE 0x06
#define CAD_MODE 0x07


#define Fifo 0x00
#define RegOpMode 0x01
#define RegFrMsb 0x06
#define RegFrMid 0x07
#define RegFrLsb 0x08
#define RegPaConfig 0x09
#define RegModemConfig1 0x1D
#define RegModemConfig2 0x1E

#define XTAL_FIX 0//(-4125)//(29400 /*-3000*/)	//manual xtal bias correction

void SPI_setup(void);
unsigned char SPI_readByte(void);
void SPI_sendBuffer(char* txBuffer, int bytesToSend);
void ConfigSemtech(void);
void ConfigSemtechFX(void);
void SetMode(unsigned char mode);
void SetFreq(float freq,int channel);

void ResetRF();
void ConfigGPIOforSemtech();
void RFswitchPowerON();
void RFswitchPowerOFF();


void SendWithFX(unsigned long ID,unsigned char iter,float freq,unsigned char* packet,int length,unsigned char confirmationNeeded,unsigned char protocol);
void SendInLORA(float freq,unsigned char* packet,int length);
int ReceiveInLORA(float freq,unsigned char *packet,unsigned char* RSSI,unsigned char* curRSSI,unsigned char* SNR, int length, uint32_t address);

void SendWithFX_carrier(float freq, uint32_t ms);
void SendWithFX_carrier_stop();

void writeReg(unsigned char addr,unsigned char value);
unsigned char readReg(unsigned char addr);

void SendUART(char *str);
