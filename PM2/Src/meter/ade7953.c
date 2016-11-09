#include "defines_HW.h"
#ifdef DD_ADE7953

#include "ade7953.h"

#include "em_cmu.h"
#include "em_gpio.h"

#include "main.h"
#include "timeevents.h"
#include "archive.h"
//#include "pulsecalibration.h"
#include "systimer.h"

#include "dti.h"

#include <stdbool.h>

#define  UNITS_PER_WH   (2000.0)

#define SAGCYC			0x000
#define DISNOLOAD		0x001
#define LCYCMODE		0x004
#define PGA_V			0x007
#define PGA_IA			0x008
#define PGA_IB			0x009
#define WRITE_PROTECT	        0x040
#define LAST_OP			0x0FD
#define Version			0x702
#define EX_REF			0x800
#define ZXTOUT			0x100
#define LINECYC			0x101
#define CONFIG			0x102
#define CF1DEN			0x103
#define CF2DEN			0x104
#define CFMODE			0x107
#define PHCALA			0x108
#define PHCALB			0x109
#define PFA 			0x10A
#define PFB 			0x10B
#define ANGLE_A			0x10C
#define ANGLE_B			0x10D
#define PERIOD			0x10E
#define ALT_OUTPUT		0x110
#define LAST_ADD		0x1FE
#define BOOT			0x120
#define SAGLVL 			0x300
#define ACCMODE 		0x301
#define AP_NOLOAD 		0x303
#define VAR_NOLOAD		0x304
#define VA_NOLOAD		0x305
#define AVA		        0x310
#define BVA			0x311
#define AWATT			0x312
#define BWATT 			0x313
#define AVAR 			0x314
#define BVAR			0x315
#define IA			0x316
#define IB			0x317
#define Vi			0x318
#define IRMSA			0x31A
#define IRMSB			0x31B
#define VRMS			0x31C
#define AENERGYA		0x31E
#define AENERGYB		0x31F
#define RENERGYA		0x320
#define RENERGYB		0x321
#define APENERGYA		0x322
#define APENERGYB		0x323
#define OVLVL			0x324
#define OILVL			0x325
#define VPEAK			0x326
#define RSTVPEAK		0x327
#define IAPEAK			0x328
#define RSTIAPEAK		0x329
#define IBPEAK			0x32A
#define RSTIBPEAK		0x32B
#define IRQENA			0x32C
#define IRQSTATA		0x32D
#define RSTIRQSTATA		0x32E
#define IRQENB			0x32F
#define IRQSTATB		0x330
#define RSTIRQSTATB		0x331
#define CRC			0x37F
#define AIGAIN			0x380
#define AVGAIN			0x381
#define AWGAIN			0x382
#define AVARGAIN		0x383
#define AVAGAIN			0x384
#define AIRMSOS			0x386
#define VRMSOS			0x388
#define AWATTOS			0x389
#define AVAROS			0x38A
#define AVAOS			0x38B
#define BIGAIN			0x38C
#define BVGAIN			0x38D
#define BWGAIN			0x38E
#define BVARGAIN		0x38F
#define BVAGAIN			0x390
#define BIRMSOS			0x392
#define BWATTOS			0x395
#define BVAROS			0x396
#define BVAOS			0x397

//старая Сашина структура, пока не упразднена
//typedef struct {
////  time_t        time;
////  uint32_t 	vrms;		// Voltage RMS
////  uint32_t 	irms;		// Current RMS
//  int32_t 	Pi;	// Active positive energy
//  int32_t 	Qi;	// Reactive positive energy
//  int32_t 	Si;	// Reactive positive energy
//  
////  float         apower_calc;    //Calculated active power
////  float         rpower_calc;    //Calculated reactive power
////  int16_t 	angle;           // Angle between I and U
////  int16_t       pfreq;           //Регистр периода 
////  bool IsCalibrated;
//}ADE_Reg_s;
//
//static METER_NEW_DATA dataNew;

//ADE_Reg_s ADE_Reg;
//const  ADE_Reg_s *ADE_GetADE_Reg(void)           {  return &ADE_Reg;    }

//bool ADE_was_calibrated = 0;
int8_t ADE_index_Cal = -1;

ADE_Inst_Reg    ADE_Inst;

//ADE_Cal_Reg     ADE_Cal;
ADE_Cal_Reg     ADE_Cal_phase[ADE_Cal_PHASE_MAX];

__no_init float ADE_alfa @ (0x20000000 + 6080);

#ifdef OPTO_PORT 
void ADE_SPI_Init()
{
	CMU_ClockEnable(cmuClock_GPIO, true);

    gpio_INI_ADE_CS;
    gpio_INI_ADE_SCK;
    gpio_INI_ADE_SDO;
    gpio_INI_ADE_SDI;
    gpio_OFF_ADE_IRQ;
    gpio_OFF_ADE_RES;
}

void ADE_SPI_Deinit()
{
    gpio_OFF_ADE_CS;
    gpio_OFF_ADE_SCK;
    gpio_OFF_ADE_SDO;
    gpio_OFF_ADE_SDI;
    gpio_OFF_ADE_IRQ;
    gpio_OFF_ADE_RES;
}

uint8_t ADE_SPI_RxTx( uint8_t Byte )
{
    int i;
    uint8_t retval = 0;    // Return value
    gpio_RES_ADE_SCK;
    for (i = 0; i < 8; i++) {
        gpio_RES_ADE_SCK;
        if (Byte & 0x80 ) {
            gpio_SET_ADE_SDO;
        } else {
            gpio_RES_ADE_SDO;
        }
        Byte = Byte << 1;
        gpio_SET_ADE_SCK;
        retval = retval << 1;
        if (gpio_GET_ADE_SDI)  {
            retval |= 0x01;         // If a 1, set next bit to 1
        } else  {
            retval &= 0xFE;         // If a 0, set next bit to 0
        }
    }
    gpio_RES_ADE_SCK;
    return retval;
}

uint32_t ADE_SPI_ReadReg(uint16_t reg)
{
	uint32_t data = 0;
	uint8_t n_bytes = ((reg >> 8)&0x3) + 1; // High nibble in register list happens to correlate with register size (except VERSION and EX_REF!)
    ADE_SPI_Init();
    gpio_RES_ADE_CS;
	ADE_SPI_RxTx(reg>>8);     // Read flag
	ADE_SPI_RxTx(reg&0xFF);   // Register address
	ADE_SPI_RxTx(0x80); // Register address

	while (n_bytes--) {
		data |= ADE_SPI_RxTx(0) << (8 * n_bytes);
	}
    gpio_INI_ADE_CS;
	ADE_SPI_Deinit();
    return data;
}

void ADE_SPI_WriteReg(uint16_t reg, uint32_t data)
{
	uint8_t n_bytes = ((reg >> 8)&0x3) + 1;
    ADE_SPI_Init();
    gpio_RES_ADE_CS;
	ADE_SPI_RxTx(reg>>8);     // Write flag
    ADE_SPI_RxTx(reg&0xFF);   // Register address
	ADE_SPI_RxTx(0x00); // Register address
	
	while (n_bytes--) {
        ADE_SPI_RxTx((data >> (8 * n_bytes)) & 0xFF);
    }
    gpio_INI_ADE_CS;
    ADE_SPI_Deinit();
}
#else
void ADE_SPI_Init()
{
	CMU_ClockEnable(cmuClock_USART0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

    gpio_INI_ADE_CS;
    gpio_INI_ADE_SCK;
    gpio_INI_ADE_SDO;
    gpio_INI_ADE_SDI;
    gpio_INI_ADE_IRQ;

	USART_InitSync_TypeDef uartInit =
	{
		usartDisable,      /* Disable RX/TX when init completed. */                                \
		0,                 /* Use current configured reference clock for configuring baudrate. */ \
		5000000,           /* 5 Mbits/s. */                                                       \
		usartDatabits8,    /* 8 databits. */                                                      \
		true,              /* Master mode. */                                                     \
		true,              /* Send most significant bit first. */                                \
		usartClockMode3,   /* Clock idle high, sample on rising edge. */                           \
		false,             /* Not USART PRS input mode. */                                        \
		usartPrsRxCh0,     /* PRS channel 0. */                                                   \
		false              /* No AUTOTX mode. */                                                  \
	};
	USART_InitSync(USART0, &uartInit);      // Apply configuration struct to USART1
	USART_IntClear(USART0, _USART_IF_MASK); // Clear any USART interrupt flags
	NVIC_ClearPendingIRQ(USART0_RX_IRQn);   // Clear pending RX interrupt flag in NVIC
	NVIC_ClearPendingIRQ(USART0_TX_IRQn);   // Clear pending TX interrupt flag in NVIC

	USART0->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_LOCATION_LOC2;

	USART_Enable(USART0, usartEnable);     // Enable transmitter and receiver

}

void ADE_SPI_Deinit()
{
    USART_Enable(USART0, usartDisable);
    gpio_OFF_ADE_SCK;
    gpio_OFF_ADE_SDO;
    gpio_OFF_ADE_SDI;
    gpio_OFF_ADE_IRQ;
    gpio_OFF_ADE_CS;
//        //GPIO_PinModeSet (ADE_CS.port,  ADE_CS.pin, 	gpioModeDisabled, 	0);
//	GPIO_PinModeSet (ADE_CS.port,  ADE_CS.pin, 	gpioModeInputPull/*gpioModePushPull*/, 	1);
//        GPIO_PinModeSet (ADE_SCK.port, ADE_SCK.pin, 	gpioModeDisabled, 	0);
//	GPIO_PinModeSet (ADE_SDO.port, ADE_SDO.pin, 	gpioModeDisabled, 	0);
//	GPIO_PinModeSet (ADE_SDI.port, ADE_SDI.pin, 	gpioModeDisabled,	0);
//	GPIO_PinModeSet (ADE_IRQ.port, ADE_IRQ.pin, 	gpioModeDisabled,       0);
}

uint32_t ADE_SPI_ReadReg(uint16_t reg)
{
	uint32_t data = 0;
	uint8_t n_bytes = ((reg >> 8)&0x3) + 1; // High nibble in register list happens to correlate with register size (except VERSION and EX_REF!)
    ADE_SPI_Init();
    gpio_RES_ADE_CS;
	//USART0->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
//        GPIO_PinModeSet (ADE_CS.port,  ADE_CS.pin, 	gpioModePushPull, 	0);
	//GPIO_PinOutClear(ADE_CS.port, ADE_CS.pin); // Chip select
	USART_SpiTransfer(USART0, reg>>8);
	USART_SpiTransfer(USART0, reg&0xFF);
	USART_SpiTransfer(USART0, 0x80); // Read flag

	// Read N bytes, MSB first
	while(n_bytes--)
	{
		data |= USART_SpiTransfer(USART0, 0) << (8 * n_bytes);
	}
	//GPIO_PinOutSet(ADE_CS.port, ADE_CS.pin);
//        GPIO_PinModeSet (ADE_CS.port,  ADE_CS.pin, 	gpioModePushPull, 	1);
        //GPIO_PinModeSet (ADE_CS.port,  ADE_CS.pin, 	gpioModeInputPull, 	1);
    gpio_INI_ADE_CS;
	ADE_SPI_Deinit();
        return data;
}

void ADE_SPI_WriteReg(uint16_t reg, uint32_t data)
{
	// ADE has registers of different size, we need to read exact number of bytes.
	// High nibble in register address happens to correlate with register size (except VERSION and EX_REF, f#ck with them)
	uint8_t n_bytes = ((reg >> 8)&0x3) + 1;
      
    ADE_SPI_Init();
    gpio_RES_ADE_CS;
//	GPIO_PinOutClear(ADE_CS.port, ADE_CS.pin); 	// Chip select
	USART_SpiTransfer(USART0, reg>>8);			// Register address
	USART_SpiTransfer(USART0, reg&0xFF);		// Register address
	USART_SpiTransfer(USART0, 0x00); 			// Write flag

	// Write N bytes, MSB first
	while(n_bytes--)	USART_SpiTransfer(USART0, (data >> (8 * n_bytes)) & 0xFF);
    gpio_INI_ADE_CS;
//	GPIO_PinOutSet(ADE_CS.port, ADE_CS.pin);
    ADE_SPI_Deinit();
}
#endif

bool ADE_ReadReg()
{
  static uint32_t    prev_aenergy = 0;
  static uint32_t    prev_renergy = 0;
  static uint32_t    prev_apenergy = 0;
  
  uint8_t phase;
  METER_NEW_DATA *dNew = &meter->data->curr;

// - активная и реактивная электроэнергия
// - напряжение фазное и линейное
// - ток (пофазно)
// - активная, реактивная и полная мощность (пофазно и суммарная величина)
// - коэффициент мощности (пофазно и суммарная величина)
// - частота сети

  for (phase = 0; phase < PHASE_MAX; ++phase) {
    memset(&dNew->phase[phase], 0, sizeof(METER_INSTANTS));
  }
  
  //ADE_Reg.time = time;
//  ADE_Reg.Pi = ADE_SPI_ReadReg(AENERGYA);     //Активная энергия
//  ADE_Reg.Qi = ADE_SPI_ReadReg(RENERGYA);     //Реактивная энергия
//  ADE_Reg.Si = ADE_SPI_ReadReg(APENERGYA);    //Полная энергия
  
  dNew->phase[0].P = dNew->phase[1].P = ADE_SPI_ReadReg(AENERGYA);     //Активная энергия
  dNew->phase[0].Q = dNew->phase[1].Q = ADE_SPI_ReadReg(RENERGYA);     //Реактивная энергия
  dNew->phase[0].S = dNew->phase[1].S = (int32_t)sqrt(dNew->phase[1].P * dNew->phase[1].P + dNew->phase[1].Q * dNew->phase[1].Q);//ADE_SPI_ReadReg(APENERGYA);    //Полная энергия

  dNew->phase[0].U = dNew->phase[1].U = ADE_SPI_ReadReg(VRMS)/10000.0;
  dNew->phase[0].I = dNew->phase[1].I = ADE_SPI_ReadReg(IRMSA)/80000.0;
#ifdef CURRENT_80
  dNew->phase[0].I *= 2;
  dNew->phase[1].I *= 2;
#endif
  dNew->phase[0].F = dNew->phase[1].F = 223000.0/(ADE_SPI_ReadReg(PERIOD) + 1) * 1.0036;
  
  static double VRMS_summ, IRMS_summ;
  static uint32_t aver_counter;
  
    if (ADE_alfa) {
        if (ADE_alfa <= 1) {
            ADE_Inst.aenergy += dNew->phase[1].P;
            ADE_Inst.renergy += fabs(dNew->phase[1].Q);
            ADE_Inst.apenergy += dNew->phase[1].S;
            VRMS_summ += dNew->phase[1].U;
            IRMS_summ += dNew->phase[1].I;
            aver_counter++;
            ADE_Inst.vrms = VRMS_summ/aver_counter; 
            ADE_Inst.irms = IRMS_summ/aver_counter; 
        }
        mQueue_PutMsg(STRIJ_LED_ON_EVENT,0);
    } else {
        ADE_Inst.aenergy = 0;
        ADE_Inst.renergy = 0;
        ADE_Inst.apenergy = 0;
        aver_counter = 0;
        ADE_Inst.vrms = dNew->phase[1].U;
        ADE_Inst.irms = dNew->phase[1].I;
        VRMS_summ = 0;
        IRMS_summ = 0;
//        dNew->phase[0].U = dNew->phase[1].U = ADE_SPI_ReadReg(VRMS)/10000.0;
//#ifdef CURRENT_80
//        dNew->phase[0].I = dNew->phase[1].I = ADE_SPI_ReadReg(IRMSA)/40000.0;
//#else
//        dNew->phase[0].I = dNew->phase[1].I = ADE_SPI_ReadReg(IRMSA)/80000.0;
//#endif
    }
  if(dNew->phase[1].P || (prev_aenergy == 0)) dNew->to_display[0].Pi = dNew->to_display[1].Pi = (float)dNew->phase[0].P * 1800 / UNITS_PER_WH;  
  prev_aenergy = dNew->phase[1].P;
  if(dNew->phase[1].Q || (prev_renergy == 0)) dNew->to_display[0].Qi = dNew->to_display[1].Qi = (float)dNew->phase[0].Q * 1800 / UNITS_PER_WH;  
  prev_renergy = dNew->phase[1].Q; 
  if(dNew->phase[1].S || (prev_apenergy == 0)) dNew->to_display[0].Si = dNew->to_display[1].Si = (float)dNew->phase[0].S * 1800 / UNITS_PER_WH;  
  prev_apenergy = dNew->phase[1].S;
  
  for (phase = 0; phase < PHASE_MAX; ++phase) {
      if (!dNew->to_display[phase].Pi) {
          dNew->phase[phase].cosFi = 1.0;
          continue;
      }
      dNew->phase[phase].cosFi = dNew->to_display[phase].Pi / sqrt(dNew->to_display[phase].Pi * dNew->to_display[phase].Pi + dNew->to_display[phase].Qi * dNew->to_display[phase].Qi);
  }
  
//  ADE_Inst.apower_calc = dNew->to_display[0].Pi;
//  ADE_Inst.rpower_calc = dNew->to_display[0].Qi;
//  ADE_Inst.aparpower_calc = dNew->to_display[0].Si;
//  ADE_Inst.pfreq = dNew->phase[1].F;
//  ADE_Inst.vrms = dNew->phase[0].U;
//  ADE_Inst.irms = dNew->phase[0].I;
//  ADE_Inst.angle = dNew->phase[1].cosFi;
  
//  if(ADE_Inst.apower_calc != 0.0)  ADE_Inst.angle = ADE_Inst.apower_calc/sqrt(ADE_Inst.apower_calc*ADE_Inst.apower_calc + ADE_Inst.rpower_calc*ADE_Inst.rpower_calc);
//  else ADE_Inst.angle = 1;
  
  return true;
}


void ADE_RegsInit(void)
{
//    ADE_alfa = 0;

	ADE_SPI_WriteReg(CONFIG, 0x0080);	    // SW RST
    Delay100Ms(1);  //TD_RTC_Delay(500);
//	uint32_t delay = 20000;
//    while(delay--);
    //while(GPIO_PinInGet(ADE_IRQ.port, ADE_IRQ.pin));	// Wait
    ADE_SPI_ReadReg(RSTIRQSTATA);

    Delay100Ms(1);  //TD_RTC_Delay(500);
	ADE_SPI_WriteReg(0x0FE, 0xAD);                          // Boot procedure
    Delay100Ms(1);  //TD_RTC_Delay(500);
	ADE_SPI_WriteReg(0x120, 0x30);                          // Boot procedure
    Delay100Ms(1);  //TD_RTC_Delay(500);

	ADE_SPI_WriteReg(CONFIG, 	0x0004); 	        // PFA LinCyc enable
	ADE_SPI_WriteReg(PGA_V,  	0x00); 		        // Voltage gain x1 (ADC +/-500mV)
        
    uint8_t ADE_was_calibrated;
    ADE_was_calibrated = ArchiveGetDataRecordCRC( NEW_CAL_ADR, (uint8_t*)ADE_Cal_phase, sizeof(ADE_Cal_phase));
    if (!ADE_was_calibrated) {
        ADE_Set_Default_Cal();
        Settings.Calibration_completed = 0;
    } else {
        ADE_Write_Cal();
    }

	ADE_SPI_WriteReg(VAR_NOLOAD, 	0); 	
#ifdef CURRENT_80
    ADE_SPI_WriteReg(PGA_IA, 	0x02); 		        // Current A gain x8 (ADC +/-62mV)
#else
    ADE_SPI_WriteReg(PGA_IA, 	0x03); 		        // Current A gain x8 (ADC +/-62mV)
#endif
	ADE_SPI_WriteReg(LINECYC, 	200); 		        // Accumulate on integer number of halfcycles for 2s
        
    ADE_SPI_WriteReg(LCYCMODE, 	0x7F); 		        // Enable line cycle accumulation mode

    ADE_Configure_Outs();
//        ADE_SPI_WriteReg(DISNOLOAD, 7); 		        // Disable no-load threshold         
    ADE_SPI_WriteReg(AP_NOLOAD, 100); 
	ADE_SPI_WriteReg(VAR_NOLOAD, 20000);
    ADE_SPI_WriteReg(VA_NOLOAD, 20000);
	ADE_SPI_WriteReg(DISNOLOAD, 0); 		        // Enable no-load threshold
        
    Delay100Ms(5);  //TD_RTC_Delay(500);
    
    ADE_SPI_ReadReg(AENERGYA);
    ADE_SPI_ReadReg(RENERGYA);
    ADE_SPI_ReadReg(APENERGYA);
    
    meter->K = 1.0 / UNITS_PER_WH;

    static uint8_t flag_reg = 0;
    if (!flag_reg) {
        flag_reg = 1;

        DTI_regProperty("ADE_Inst", STRUCT_TYPE, (uint32_t)&ADE_Inst, 0);
        DTI_regProperty("ADE_Cal", STRUCT_TYPE, (uint32_t)&ADE_Cal_phase[0], 0);
        DTI_regProperty("ADE_Crd_A",    FUNCTION_TYPE, (uint32_t)ADE_Read_Cal,  0);
        DTI_regProperty("ADE_Cwr_A",    FUNCTION_TYPE, (uint32_t)ADE_Write_Cal,  0);
//        DTI_regProperty("ADE_CRead", FUNCTION_TYPE , (uint32_t)ADE_Read_Cal, 0);
//        DTI_regProperty("ADE_CWrit", FUNCTION_TYPE , (uint32_t)ADE_Write_Cal, 0);
        DTI_regProperty("ADE_alfa", FLOAT_TYPE, (uint32_t)&ADE_alfa, 0);
    }        
}

void ADE_Configure_Outs()
{
    ADE_SPI_WriteReg(CF1DEN, 	2000 * 1000 / Settings.Pulse0_freq);                 
    
    ADE_SPI_WriteReg(CF2DEN, 	2000 * 1000 / Settings.Pulse1_freq);                 

    ADE_SPI_WriteReg(CFMODE, 	(Settings.Pulse1_energy_type << 4) | Settings.Pulse0_energy_type);        
}


void ADE_Read_Cal()
{
  ADE_Cal_phase[0].aigain = ADE_SPI_ReadReg(AIGAIN);
  ADE_Cal_phase[0].avgain = ADE_SPI_ReadReg(AVGAIN);
  ADE_Cal_phase[0].awgain = ADE_SPI_ReadReg(AWGAIN);
  ADE_Cal_phase[0].avargain = ADE_SPI_ReadReg(AVARGAIN);
  ADE_Cal_phase[0].avagain = ADE_SPI_ReadReg(AVAGAIN);
  ADE_Cal_phase[0].airmsos = ADE_SPI_ReadReg(AIRMSOS);
  ADE_Cal_phase[0].vrmsos = ADE_SPI_ReadReg(VRMSOS);
  ADE_Cal_phase[0].awattos = ADE_SPI_ReadReg(AWATTOS);
  ADE_Cal_phase[0].avaros = ADE_SPI_ReadReg(AVAROS);
  ADE_Cal_phase[0].avaos = ADE_SPI_ReadReg(AVAOS);
  ADE_index_Cal = 0;
}

void ADE_Write_Cal()
{
  ADE_SPI_WriteReg(AIGAIN, ADE_Cal_phase[0].aigain);
  ADE_SPI_WriteReg(AVGAIN, ADE_Cal_phase[0].avgain);
  ADE_SPI_WriteReg(AWGAIN, ADE_Cal_phase[0].awgain);
  ADE_SPI_WriteReg(AVARGAIN, ADE_Cal_phase[0].avargain);
  ADE_SPI_WriteReg(AVAGAIN, ADE_Cal_phase[0].avagain);
  ADE_SPI_WriteReg(AIRMSOS, ADE_Cal_phase[0].airmsos);
  ADE_SPI_WriteReg(VRMSOS, ADE_Cal_phase[0].vrmsos);
  ADE_SPI_WriteReg(AWATTOS, ADE_Cal_phase[0].awattos);
  ADE_SPI_WriteReg(AVAROS, ADE_Cal_phase[0].avaros);
  ADE_SPI_WriteReg(AVAOS, ADE_Cal_phase[0].avaos);
//  ADE_was_calibrated = 1;  
}

void ADE_Set_Default_Cal()
{
    ADE_Read_Cal();
    ADE_Cal_phase[0].aigain     = 0x5129DD; //0x539583;
    ADE_Cal_phase[0].avgain     = 0x335850; //0x32e523;
    ADE_Cal_phase[0].awgain     = 0x117D45; //0x111e2a; //0x230000;
    ADE_Cal_phase[0].avargain   = 0x15F9AF; //0x1574f1; //0x2bdff3;
    ADE_Cal_phase[0].avagain    = 0x400000;
    ADE_Cal_phase[0].airmsos    = 0x000000;
    ADE_Cal_phase[0].vrmsos     = 0x000000;
    ADE_Cal_phase[0].awattos    = 0x000000;
    ADE_Cal_phase[0].avaros     = 0x000000;
    ADE_Cal_phase[0].avaos      = 0x000000;
    ADE_Write_Cal();   
}

#endif
