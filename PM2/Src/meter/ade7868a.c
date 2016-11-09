#include "defines_HW.h"
#ifdef DD_ADE7868A

#include "ade7868a.h"

#include "em_cmu.h"
#include "em_gpio.h"

#include "main.h"
#include "timeevents.h"
#include "archive.h"
//#include "pulsecalibration.h"
#include "systimer.h"
#include "dti.h"

#include <stdbool.h>

#define  UNITS_PER_WH   (500.0)

// калибровки
#define AIGAIN          0x4380  // 24bit // Phase A current gain adjust
#define AVGAIN          0x4381  // 24bit // Phase A voltage gain adjust
#define BIGAIN          0x4382  // 24bit // Phase B current gain adjust
#define BVGAIN          0x4383  // 24bit // Phase B voltage gain adjust
#define CIGAIN          0x4384  // 24bit // Phase C current gain adjust
#define CVGAIN          0x4385  // 24bit // Phase C voltage gain adjust
#define NIGAIN          0x4386  // 24bit // Neutral current gain adjust
#define AIRMSOS         0x4387  // 24bit // Phase A current rms offset
#define AVRMSOS         0x4388  // 24bit // Phase A voltage rms offset
#define BIRMSOS         0x4389  // 24bit // Phase B current rms offset
#define BVRMSOS         0x438A  // 24bit // Phase B voltage rms offset
#define CIRMSOS         0x438B  // 24bit // Phase C current rms offset
#define CVRMSOS         0x438C  // 24bit // Phase C voltage rms offset
#define NIRMSOS         0x438D  // 24bit // Neutral current rms offset
#define AVAGAIN         0x438E  // 24bit // Phase A apparent power gain adjust
#define BVAGAIN         0x438F  // 24bit // Phase B apparent power gain adjust
#define CVAGAIN         0x4390  // 24bit // Phase C apparent power gain adjust
#define AWGAIN          0x4391  // 24bit // Phase A total active power gain adjust
#define AWATTOS         0x4392  // 24bit // Phase A total active power offset adjust
#define BWGAIN          0x4393  // 24bit // Phase B total active power gain adjust
#define BWATTOS         0x4394  // 24bit // Phase B total active power offset adjust
#define CWGAIN          0x4395  // 24bit // Phase C total active power gain adjust
#define CWATTOS         0x4396  // 24bit // Phase C total active power offset adjust
#define AVARGAIN        0x4397  // 24bit // Phase A total reactive power gain adjust
#define AVAROS          0x4398  // 24bit // Phase A total reactive power offset adjust
#define BVARGAIN        0x4399  // 24bit // Phase B total reactive power gain adjust
#define BVAROS          0x439A  // 24bit // Phase B total reactive power offset adjust
#define CVARGAIN        0x439B  // 24bit // Phase C total reactive power gain adjust
#define CVAROS          0x439C  // 24bit // Phase C total reactive power offset adjust
//#define AFWGAIN         0x439D  // 24bit // Phase A fundamental active power gain adjust. Location reserved for ADE7854, ADE7858, and ADE7868
//#define AFWATTOS        0x439E  // 24bit // Phase A fundamental active power offset adjust. Location reserved for ADE7854, ADE7858, and ADE7868
//#define BFWGAIN         0x439F  // 24bit // Phase B fundamental active power gain adjust. Location reserved for ADE7854, ADE7858, and ADE7868
//#define BFWATTOS        0x43A0  // 24bit // Phase B fundamental active power offset adjust. Location reserved for ADE7854, ADE7858, and ADE7868
//#define CFWGAIN         0x43A1  // 24bit // Phase C fundamental active power gain adjust. Location reserved for ADE7854, ADE7858, and ADE7868
//#define CFWATTOS        0x43A2  // 24bit // Phase C fundamental active power offset adjust. Location reserved for ADE7854, ADE7858, and ADE7868
//#define AFVARGAIN       0x43A3  // 24bit // Phase A fundamental reactive power gain adjust (ADE7878 only)
//#define AFVAROS         0x43A4  // 24bit // Phase A fundamental reactive power offset adjust (ADE7878 only)
//#define BFVARGAIN       0x43A5  // 24bit // Phase B fundamental reactive power gain adjust (ADE7878 only)
//#define BFVAROS         0x43A6  // 24bit // Phase B fundamental reactive power offset adjust (ADE7878 only)
//#define CFVARGAIN       0x43A7  // 24bit // Phase C fundamental reactive power gain adjust (ADE7878 only)
//#define CFVAROS         0x43A8  // 24bit // Phase C fundamental reactive power offset adjust (ADE7878 only)
// (29 + 12) * 24bit = 123byte    (29 + 12) * 4byte = 164byte

#define VATHR1          0x43A9
#define VATHR0          0x43AA
#define WTHR1           0x43AB
#define WTHR0           0x43AC
#define VARTHR1         0x43AD
#define VARTHR0         0x43AE

#define VANOLOAD        0x43B0
#define APNOLOAD        0x43B1
#define VARNOLOAD       0x43B2

//#define VLEVEL          0x43B3

#define DICOEFF         0x43B5

#define HPFDIS          0x43B6

#define ISUMLVL         0x43B8  // Threshold used in comparison between the sum of phase currents and the neutral current (ADE7868 and ADE7878 only).
#define ISUM            0x43BF  // Sum of IAWV, IBWV, and ICWV registers (ADE7868 and ADE7878 only).

// значения RMS
#define AIRMS           0x43C0  // Phase A current rms value
#define AVRMS           0x43C1  // Phase A voltage rms value
#define BIRMS           0x43C2  // Phase B current rms value
#define BVRMS           0x43C3  // Phase B voltage rms value
#define CIRMS           0x43C4  // Phase C current rms value
#define CVRMS           0x43C5  // Phase C voltage rms value
#define NIRMS           0x43C6  // Neutral current rms value

#define Run             0xE228  // Run register starts and stops the DSP
// значения энергии Billable Registers
#define AWATTHR         0xE400  // Phase A total active energy accumulation
#define BWATTHR         0xE401  // Phase B total active energy accumulation
#define CWATTHR         0xE402  // Phase C total active energy accumulation
//#define AFWATTHR        0xE403  // Phase A fundamental active energy accumulation
//#define BFWATTHR        0xE404  // Phase B fundamental active energy accumulation
//#define CFWATTHR        0xE405  // Phase C fundamental active energy accumulation
#define AVARHR          0xE406  // Phase A total reactive energy accumulation
#define BVARHR          0xE407  // Phase B total reactive energy accumulation
#define CVARHR          0xE408  // Phase C total reactive energy accumulation
//#define AFVARHR         0xE409  // Phase A fundamental reactive energy accumulation
//#define BFVARHR         0xE40A  // Phase B fundamental reactive energy accumulation
//#define CFVARHR         0xE40B  // Phase C fundamental reactive energy accumulation
#define AVAHR           0xE40C  // Phase A apparent energy accumulation
#define BVAHR           0xE40D  // Phase B apparent energy accumulation
#define CVAHR           0xE40E  // Phase C apparent energy accumulation

// Configuration and Power Quality Registers
#define IPEAK           0xE500  // Current peak register
#define VPEAK           0xE501  // Voltage peak register
#define STATUS0         0xE502  // Interrupt Status Register
#define STATUS1         0xE503  // Interrupt Status Register
#define AIMAV           0xE504  // Phase A current mean absolute value computed during PSM0 and PSM1 modes
#define BIMAV           0xE505  // Phase B current mean absolute value computed during PSM0 and PSM1 modes
#define CIMAV           0xE506  // Phase C current mean absolute value computed during PSM0 and PSM1 modes
#define OILVL           0xE507  // Overcurrent threshold
#define OVLVL           0xE508  // Overvoltage threshold
#define SAGLVL          0xE509  // Voltage sag level threshold
#define MASK0           0xE50A  // Interrupt Enable Register
#define MASK1           0xE50B  // Interrupt Enable Register
// WAVEFORM мгновенные значения every 125us (8kHz rate)
#define IAWV            0xE50C  // Instantaneous value of Phase A current
#define IBWV            0xE50D  // Instantaneous value of Phase B current
#define ICWV            0xE50E  // Instantaneous value of Phase C current
#define INWV            0xE50F  // Instantaneous value of neutral current
#define VAWV            0xE510  // Instantaneous value of Phase A voltage
#define VBWV            0xE511  // Instantaneous value of Phase B voltage
#define VCWV            0xE512  // Instantaneous value of Phase C voltage
#define AWATT           0xE513  // Instantaneous value of Phase A total active power
#define BWATT           0xE514  // Instantaneous value of Phase B total active power
#define CWATT           0xE515  // Instantaneous value of Phase C total active power
#define AVAR            0xE516  // Instantaneous value of Phase A total reactive power
#define BVAR            0xE517  // Instantaneous value of Phase B total reactive power
#define CVAR            0xE518  // Instantaneous value of Phase C total reactive power
#define AVA             0xE519  // Instantaneous value of Phase A apparent power
#define BVA             0xE51A  // Instantaneous value of Phase B apparent power
#define CVA             0xE51B  // Instantaneous value of Phase C apparent power

#define CHECKSUM        0xE51F
#define VNOM            0xE520

#define IARMS_LRIP      0xE530  // 1.024 sec average of Phase A current rms
#define VARMS_LRIP      0xE531  // 1.024 sec average of Phase A voltage rms
#define IBRMS_LRIP      0xE532  // 1.024 sec average of Phase B current rms
#define VBRMS_LRIP      0xE533  // 1.024 sec average of Phase B voltage rms
#define ICRMS_LRIP      0xE534  // 1.024 sec average of Phase C current rms
#define VCRMS_LRIP      0xE535  // 1.024 sec average of Phase C voltage rms
#define INRMS_LRIP      0xE536  // 1.024 sec average of Phase neutral current rms

#define PHSTATUS        0xE600
// cosfi
#define ANGLE0          0xE601  // Time Delay 0. See the Time Interval Between Phases section for details
#define ANGLE1          0xE602  // Time Delay 1. See the Time Interval Between Phases section for details
#define ANGLE2          0xE603  // Time Delay 2. See the Time Interval Between Phases section for details

#define PERIOD          0xE607  // Network line period

#define PHNOLOAD        0xE608

#define LINECYC         0xE60C  // Line cycle accumulation mode count
#define ZXTOUT          0xE60D  // Zero-crossing timeout count
#define COMPMODE        0xE60E  // Computation-mode register
#define Gain            0xE60F  // PGA gains at ADC inputs (PGA1 входной усилитель)
#define CFMODE          0xE610  // CFx configuration register
#define CF1DEN          0xE611  // CF1 denominator
#define CF2DEN          0xE612  // CF2 denominator
#define CF3DEN          0xE613  // CF3 denominator
#define APHCAL          0xE614  // Phase calibration of Phase A
#define BPHCAL          0xE615  // Phase calibration of Phase B
#define CPHCAL          0xE616  // Phase calibration of Phase C
#define PHSIGN          0xE617  // Power sign register (переворот фаз)
#define CONFIG          0xE618

#define LAST_ADDR       0xE6FE  // The address of the register successfully accessed during the last read/write operation.
#define LAST_RWDATA_16  0xE6FF  // Contains the data from the last successful 16-bit register communication

#define MMODE           0xE700  // Measurement mode // измерялка частоты
#define ACCMODE         0xE701
#define LCYCMODE        0xE702
#define PEAKCYC         0xE703
#define SAGCYC          0xE704
#define CFCYC           0xE705
#define HSDC_CFG        0xE706
#define Version         0xE707
#define CONFIG_A        0xE740  // Configuration registers for the power filtering.
#define LAST_RWDATA_8   0xE7FC  // Contains the data from the last successful 8-bit register communication.
#define LAST_OP         0xE7FD  // Indicates the type, read or write, of the last successful read/write operation.
#define LPOILVL         0xEC00
#define CONFIG2         0xEC01

//bool ADE_was_calibrated = 0;
int8_t ADE_index_Cal = -1;

ADE_Inst_Reg    ADE_Inst;

ADE_Cal_Reg     ADE_Cal_phase[ADE_Cal_PHASE_MAX];
//ADE_Cal_Reg     *ADE_Cal = ADE_Cal_phase;

//#pragma location = 6080
__no_init float ADE_alfa @ (0x20000000 + 6080);

void ADE_SPI_Init()
{
//	CMU_ClockEnable(cmuClock_USART0, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

    gpio_INI_ADE_CS;
    gpio_INI_ADE_SCK;
    gpio_INI_ADE_SDO;
    gpio_INI_ADE_SDI;
//    gpio_INI_ADE_IRQ;
    gpio_OFF_ADE_IRQ;
    gpio_OFF_ADE_RES;

//	USART_InitSync_TypeDef uartInit =
//	{
//		usartDisable,      /* Disable RX/TX when init completed. */                                \
//		0,                 /* Use current configured reference clock for configuring baudrate. */ \
//		5000000,           /* 5 Mbits/s. */                                                       \
//		usartDatabits8,    /* 8 databits. */                                                      \
//		true,              /* Master mode. */                                                     \
//		true,              /* Send most significant bit first. */                                \
//		usartClockMode3,   /* Clock idle high, sample on rising edge. */                           \
//		false,             /* Not USART PRS input mode. */                                        \
//		usartPrsRxCh0,     /* PRS channel 0. */                                                   \
//		false              /* No AUTOTX mode. */                                                  \
//	};
//	USART_InitSync(USART0, &uartInit);      // Apply configuration struct to USART1
//	USART_IntClear(USART0, _USART_IF_MASK); // Clear any USART interrupt flags
//	NVIC_ClearPendingIRQ(USART0_RX_IRQn);   // Clear pending RX interrupt flag in NVIC
//	NVIC_ClearPendingIRQ(USART0_TX_IRQn);   // Clear pending TX interrupt flag in NVIC
//
//	USART0->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_LOCATION_LOC2;
//
//	USART_Enable(USART0, usartEnable);     // Enable transmitter and receiver

}

void ADE_SPI_Deinit()
{
//    USART_Enable(USART0, usartDisable);
    gpio_OFF_ADE_CS;
    gpio_OFF_ADE_SCK;
    gpio_OFF_ADE_SDO;
    gpio_OFF_ADE_SDI;
    gpio_OFF_ADE_IRQ;
    gpio_OFF_ADE_RES;
    //GPIO_PinModeSet (ADE_CS.port,  ADE_CS.pin, 	gpioModeDisabled, 	0);
//	GPIO_PinModeSet (ADE_CS.port,  ADE_CS.pin, 	gpioModeInputPull/*gpioModePushPull*/, 	1);
//        
//        GPIO_PinModeSet (ADE_SCK.port, ADE_SCK.pin, 	gpioModeDisabled, 	0);
//	GPIO_PinModeSet (ADE_SDO.port, ADE_SDO.pin, 	gpioModeDisabled, 	0);
//	GPIO_PinModeSet (ADE_SDI.port, ADE_SDI.pin, 	gpioModeDisabled,	0);
//	GPIO_PinModeSet (ADE_IRQ.port, ADE_IRQ.pin, 	gpioModeDisabled,       0);
}

//uint8_t FLASH_ByteIn( void )
//{
//    int i;
//    uint8_t retval = 0;    // Return value
//    gpio_RES_FLASH_SCL;
//    for ( i = 0; i < 8; i++ )    {
//        retval = retval << 1;
//        gpio_SET_FLASH_SCL;
//        if (gpio_GET_FLASH_SO)  {
//            retval |= 0x01;         // If a 1, set next bit to 1
//        } else  {
//            retval &= 0xFE;         // If a 0, set next bit to 0
//        }
//        gpio_RES_FLASH_SCL;
//    }
//    return retval;
//}

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
	uint8_t n_bytes = 4;
    if((reg >= 0xE600) || (reg == Run)) n_bytes = 2;
    if(reg >= 0xE700) n_bytes = 1;
    ADE_SPI_Init();
    gpio_RES_ADE_CS;
//	USART_SpiTransfer(USART0, 0x01); // Read flag
//	USART_SpiTransfer(USART0, reg>>8);
//	USART_SpiTransfer(USART0, reg&0xFF);
	ADE_SPI_RxTx(0x01);     // Read flag
	ADE_SPI_RxTx(reg>>8);   // Register address
	ADE_SPI_RxTx(reg&0xFF); // Register address

	// Read N bytes, MSB first
	while (n_bytes--) {
//		data |= USART_SpiTransfer(USART0, 0) << (8 * n_bytes);
		data |= ADE_SPI_RxTx(0) << (8 * n_bytes);
	}
    gpio_INI_ADE_CS;
	ADE_SPI_Deinit();
    return data;
}

void ADE_SPI_WriteReg(uint16_t reg, uint32_t data)
{
	// ADE has registers of different size, we need to read exact number of bytes.
	// High nibble in register address happens to correlate with register size (except VERSION and EX_REF, f#ck with them)
	
    uint8_t n_bytes = 4;
    if((reg >= 0xE600) || (reg == Run)) n_bytes = 2;
    if(reg >= 0xE700) n_bytes = 1;
      
    ADE_SPI_Init();
    gpio_RES_ADE_CS;
//    USART_SpiTransfer(USART0, 0x00); 		// Write flag
//    USART_SpiTransfer(USART0, reg>>8);		// Register address
//    USART_SpiTransfer(USART0, reg&0xFF);		// Register address
	ADE_SPI_RxTx(0x00);     // Write flag
    ADE_SPI_RxTx(reg>>8);   // Register address
	ADE_SPI_RxTx(reg&0xFF); // Register address
	
	// Write N bytes, MSB first
	while (n_bytes--) {
//        USART_SpiTransfer(USART0, (data >> (8 * n_bytes)) & 0xFF);
        ADE_SPI_RxTx((data >> (8 * n_bytes)) & 0xFF);
    }
    gpio_INI_ADE_CS;
	//GPIO_PinOutSet(ADE_CS.port, ADE_CS.pin);
    ADE_SPI_Deinit();
}

int32_t ADE_SPI_ReadReg_i24(uint16_t reg)
{
    uint32_t d = ADE_SPI_ReadReg(reg);
    
    if (d > 0x007FFFFF) {
        return -(int32_t)(d & 0x00FFFFFF);
    }
    return d;
}

bool ADE_ReadReg()
{
    static uint32_t prev_Penergy[PHASE_MAX] = {0,0,0,0};
    static uint32_t prev_Qenergy[PHASE_MAX] = {0,0,0,0};
    static uint32_t prev_Senergy[PHASE_MAX] = {0,0,0,0};
    static double VRMS_summ = 0;
    static double IRMS_summ = 0;
//    static double VRMS_summ[PHASE_MAX] = {0,0,0,0};
//    static double IRMS_summ[PHASE_MAX] = {0,0,0,0};
    static uint32_t aver_counter;

    METER_NEW_DATA *dNew = &meter->data->curr;
    uint8_t phase; 
    for (phase = 0; phase < PHASE_MAX; ++phase) {
        memset(&dNew->phase[phase], 0, sizeof(METER_INSTANTS));
    }

// - активная и реактивная электроэнергия
// - напряжение фазное и линейное
// - ток (пофазно)
// - активная, реактивная и полная мощность (пофазно и суммарная величина)
// - коэффициент мощности (пофазно и суммарная величина)
// - частота сети
  
    dNew->phase[1].P = ADE_SPI_ReadReg(AWATTHR);      //Активная энергия
    dNew->phase[1].Q = ADE_SPI_ReadReg(AVARHR);       //Реактивная энергия
    dNew->phase[1].S = ADE_SPI_ReadReg(AVAHR);        //Полная энергия
    dNew->phase[2].P = ADE_SPI_ReadReg(BWATTHR);      //Активная энергия
    dNew->phase[2].Q = ADE_SPI_ReadReg(BVARHR);       //Реактивная энергия
    dNew->phase[2].S = ADE_SPI_ReadReg(BVAHR);        //Полная энергия
    dNew->phase[3].P = ADE_SPI_ReadReg(CWATTHR);      //Активная энергия
    dNew->phase[3].Q = ADE_SPI_ReadReg(CVARHR);       //Реактивная энергия
    dNew->phase[3].S = ADE_SPI_ReadReg(CVAHR);        //Полная энергия

    dNew->phase[1].U = ADE_SPI_ReadReg_i24(AVRMS)/5000.0;
    dNew->phase[2].U = ADE_SPI_ReadReg_i24(BVRMS)/5000.0;
    dNew->phase[3].U = ADE_SPI_ReadReg_i24(CVRMS)/5000.0;
    dNew->phase[1].I = ADE_SPI_ReadReg_i24(AIRMS)/50000.0; // 50000.0
    dNew->phase[2].I = ADE_SPI_ReadReg_i24(BIRMS)/50000.0;
    dNew->phase[3].I = ADE_SPI_ReadReg_i24(CIRMS)/50000.0;
    // / 40000 = 5A  1520000 = 5A
//#ifdef CURRENT_80
//#else
//    dNew->phase[1].I /= 2.0;
//    dNew->phase[2].I /= 2.0;
//    dNew->phase[3].I /= 2.0;
//#endif
    dNew->phase[0].I = ADE_SPI_ReadReg_i24(NIRMS)/20000.0; // ток нейтрали
    dNew->phase[0].F = 256000.0/(ADE_SPI_ReadReg(PERIOD) + 1);

  //ADE_Reg.time = time;
//  ADE_Reg.aenergy_p = ADE_SPI_ReadReg(CWATTHR);
//  ADE_Reg.renergy_p = ADE_SPI_ReadReg(CVARHR);
  //ADE_Reg.aenergy_p = ADE_SPI_ReadReg(AENERGYA);        //Активная энергия
  //ADE_Reg.renergy_p = ADE_SPI_ReadReg(RENERGYA);        //Реактивная энергия
  //ADE_Reg.apenergy_p = ADE_SPI_ReadReg(APENERGYA);        //Полная энергия
    
//    MASK1 Register (Address 0xE50B) // 19 SEQERR // 1 = неверное чередование фаз
 
    if(ADE_alfa) {
        if(ADE_alfa <= 1) {
            phase = ADE_index_Cal+1;
            if (ADE_index_Cal >= 0 && ADE_index_Cal <= 2) {
                ADE_Inst.aenergy += dNew->phase[phase].P;
                ADE_Inst.renergy += fabs(dNew->phase[phase].Q);
                ADE_Inst.apenergy += dNew->phase[phase].S;
                VRMS_summ += dNew->phase[phase].U;
                IRMS_summ += dNew->phase[phase].I;
            } else
            if (ADE_index_Cal == 3) {
                VRMS_summ += 0;
                IRMS_summ += dNew->phase[0].I;
            }
            aver_counter++;
            ADE_Inst.vrms = VRMS_summ/aver_counter; 
            ADE_Inst.irms = IRMS_summ/aver_counter; 
        }
        mQueue_PutMsg(STRIJ_LED_ON_EVENT,0);
    } else {
        phase = ADE_index_Cal+1;
        aver_counter = 0;
        VRMS_summ = 0;
        IRMS_summ = 0;
        if (ADE_index_Cal >= 0 && ADE_index_Cal <= 2) {
            ADE_Inst.vrms = dNew->phase[phase].U;
            ADE_Inst.irms = dNew->phase[phase].I;
        }else
        if (ADE_index_Cal == 3) {
            ADE_Inst.vrms = 0;
            ADE_Inst.irms = dNew->phase[0].I;
        }
        ADE_Inst.aenergy = 0;
        ADE_Inst.renergy = 0;
        ADE_Inst.apenergy = 0;
    }

    dNew->phase[0].U = 0;
    
    dNew->to_display[0].Pi = 0;
    dNew->to_display[0].Qi = 0;
    dNew->to_display[0].Si = 0;
    // счеталочка
    for (phase = 1; phase < PHASE_MAX; ++phase) {
        if(dNew->phase[phase].P || (prev_Penergy[phase] == 0)) dNew->to_display[phase].Pi = (float)dNew->phase[phase].P * 3600 / UNITS_PER_WH;  
        prev_Penergy[phase] = dNew->phase[phase].P;
        if(dNew->phase[phase].Q || (prev_Qenergy[phase] == 0)) dNew->to_display[phase].Qi = (float)dNew->phase[phase].Q * 3600 / UNITS_PER_WH;  
        prev_Qenergy[phase] = dNew->phase[phase].Q; 
        if(dNew->phase[phase].S || (prev_Senergy[phase] == 0)) dNew->to_display[phase].Si = (float)dNew->phase[phase].S * 3600 / UNITS_PER_WH;  
        prev_Senergy[phase] = dNew->phase[phase].S;
        
        dNew->phase[0].U += dNew->phase[phase].U / 3; // среднее напряжение
        //dNew->phase[0].I += dNew->phase[phase].I; // общий ток равен сумме токов

        dNew->to_display[0].Pi += dNew->to_display[phase].Pi;
        dNew->to_display[0].Qi += dNew->to_display[phase].Qi;
        dNew->to_display[0].Si += dNew->to_display[phase].Si;
        
        dNew->phase[0].P += dNew->phase[phase].P;
        dNew->phase[0].Q += dNew->phase[phase].Q;
        dNew->phase[0].S += dNew->phase[phase].S;
        
        dNew->phase[phase].F = dNew->phase[0].F;
        dNew->phase[phase].cosFi = !dNew->to_display[phase].Pi ? 1.0 : dNew->to_display[phase].Pi / sqrt(dNew->to_display[phase].Pi * dNew->to_display[phase].Pi + dNew->to_display[phase].Qi * dNew->to_display[phase].Qi);
        dNew->phase[0].cosFi += dNew->phase[phase].cosFi / 3; // среднее
    }
    
    static int8_t phase_freq_old = -1;
    static int8_t phase_freq_new = -1;
    if (dNew->phase[1].U > 25) phase_freq_new = 0; // phase A
    else
    if (dNew->phase[2].U > 25) phase_freq_new = 1; // phase B
    else
    if (dNew->phase[3].U > 25) phase_freq_new = 2; // phase C
    if (phase_freq_old != phase_freq_new) {
        phase_freq_old = phase_freq_new;
        ADE_SPI_WriteReg(MMODE, phase_freq_new & 0x03);
    }

//
//  ADE_Inst.pfreq = 223000.0/(ADE_SPI_ReadReg(PERIOD) + 1) * 1.0036;
//  
//  if(ADE_Inst.apower_calc != 0.0)  ADE_Inst.angle = ADE_Inst.apower_calc/sqrt(ADE_Inst.apower_calc*ADE_Inst.apower_calc + ADE_Inst.rpower_calc*ADE_Inst.rpower_calc);
//  else ADE_Inst.angle = 1;

  
    return true;
}

void ADE_RegsInit(void)
{
//    To quickly setup the ADE7878, execute the following steps:
//    1. Select the PGA gains in the phase currents, voltages, and neutral current channels: Bits [2:0] (PGA1), Bits [5:3] (PGA2) and Bits [8:6] (PGA3) in the Gain register.
//    2. If Rogowski coils are used, enable the digital integrators in the phase and neutral currents: Bit 0 (INTEN) set to 1 in CONFIG register.
//    3. If fn=60 Hz, set Bit 14 (SELFREQ) in COMPMODE register (ADE7878 only).
//    4. Initialize WTHR1 and WTHR0 registers based on Equation 25. 
//       Make VARTHR1 (ADE7858, ADE7868, and ADE7878 only) and VATHR1 equal to WTHR1 and VARTHR0 (ADE7858, ADE7868, and ADE7878 only) and VATHR0 equal to WTHR0.
//    5. Initialize CF1DEN, CF2DEN, and CF3DEN based on Equation 26.
//    6. Initialize VLEVEL (ADE7878 only) and VNOM registers based on Equation 21 and Equation 42.
//    7. Enable the data memory RAM protection by writing 0xAD to an internal 8-bit register located at Address 0xE7FE followed by a write of 0x80 to an internal 8-bit register located at Address 0xE7E3.
//    8. Start the DSP by setting Run=1.
//    9. Read the energy registers xWATTHR, xVARHR (ADE7858, ADE7868, and ADE7878 only), xVAHR, xFWATTHR, and xFVARHR (ADE7878 only) to erase their content and start energy accumulation from a known state.
//    10. Enable the CF1, CF2 and CF3 frequency converter outputs by clearing bits 9, 10 and 11 (CF1DIS, CF2DIS, and CF3DIS) to 0 in CFMODE register.

//    ADE_alfa = 0;
    
    ADE_SPI_WriteReg(Run,  	0); //Stop DSP

//	ADE_SPI_WriteReg(CONFIG, 0x0080);	    // SW RST
//	{uint32_t delay = 100000; while(delay--);}

//    ADE_SPI_WriteReg(Run,  	0); //Stop DSP
    
	ADE_SPI_WriteReg(CONFIG, 0x0080);	    // SW RST
	{uint32_t delay = 100000; while(delay--);}

//	ADE_SPI_WriteReg(CONFIG, 	0x0004); 	        // PFA LinCyc enable

        ADE_SPI_WriteReg(WTHR1,  	0x0);
        ADE_SPI_WriteReg(WTHR0,  	0x01FF6A6B);
        ADE_SPI_WriteReg(VARTHR1,  	0);
        ADE_SPI_WriteReg(VARTHR0,  	0x01FF6A6B);
        ADE_SPI_WriteReg(VATHR1,  	0);
        ADE_SPI_WriteReg(VATHR0,  	0x01FF6A6B);
        //VATHR = PMAX = 33,516,139
        
        ADE_SPI_WriteReg(Gain, 0 + (4 <<3) + (0 << 6));        //Phase current gain = 0, neutral current gain = 16, valtage gain = 1
	//ADE_SPI_WriteReg(PGA_V,  	0x00); 		        // Voltage gain x1 (ADC +/-500mV)
        
//        ADE_Cal_Reg creg[4];
//        uint32_t creg_size = sizeof(ADE_Cal_Reg) * 4;
        uint8_t ADE_was_calibrated;
        ADE_was_calibrated = ArchiveGetDataRecordCRC( NEW_CAL_ADR, (uint8_t*)ADE_Cal_phase, sizeof(ADE_Cal_phase));
        if (!ADE_was_calibrated) {
            ADE_Set_Default_Cal();
            Settings.Calibration_completed = 0;
        } else {
//            memmove(&ADE_Cal, &creg[0], sizeof(ADE_Cal_Reg));
            ADE_wr_Cal_A();
//            memmove(&ADE_Cal, &creg[1], sizeof(ADE_Cal_Reg));
            ADE_wr_Cal_B();
//            memmove(&ADE_Cal, &creg[2], sizeof(ADE_Cal_Reg));
            ADE_wr_Cal_C();
//            memmove(&ADE_Cal, &creg[3], sizeof(ADE_Cal_Reg));
            ADE_wr_Cal_N();
        }
        
//        ADE_SPI_WriteReg(AFWGAIN, 0x800000);
//        ADE_SPI_WriteReg(BFWGAIN, 0x800000);
//        ADE_SPI_WriteReg(CFWGAIN, 0x800000);
//        ADE_SPI_WriteReg(AFVARGAIN, 0x800000);
//        ADE_SPI_WriteReg(BFVARGAIN, 0x800000);
//        ADE_SPI_WriteReg(CFVARGAIN, 0x800000);
        
        //todo
    //    if(ADE_was_calibrated) ADE_Write_Cal();
   //     else {ADE_Set_Default_Cal(); ADE_was_calibrated = 0;}

	//ADE_SPI_WriteReg(VAR_NOLOAD, 	0); 	
#ifdef CURRENT_80
        //ADE_SPI_WriteReg(PGA_IA, 	0x02); 		        // Current A gain x8 (ADC +/-62mV)
#else
        //ADE_SPI_WriteReg(PGA_IA, 	0x03); 		        // Current A gain x8 (ADC +/-62mV)
#endif
        ADE_SPI_WriteReg(LINECYC, 	200); 		        // Accumulate on integer number of halfcycles for 2s
//        ADE_SPI_WriteReg(LCYCMODE, 	0x7F); 		        // Enable line cycle accumulation mode
        ADE_SPI_WriteReg(LCYCMODE, 	0x40); 		        // Enable line cycle accumulation mode
        ADE_Configure_Outs();
        ADE_SPI_WriteReg(Run,  	1);                             //Start DSP
//        ADE_SPI_WriteReg(DISNOLOAD, 7); 		        // Disable no-load threshold         
        //ADE_SPI_WriteReg(AP_NOLOAD, 100); 
	//ADE_SPI_WriteReg(VAR_NOLOAD, 20000);
        //ADE_SPI_WriteReg(VA_NOLOAD, 20000);
	//ADE_SPI_WriteReg(DISNOLOAD, 0); 		        // Enable no-load threshold
        
        DelayMs(250); //TD_RTC_Delay(100);
        DelayMs(250); 
        
        ADE_SPI_ReadReg(AWATTHR);
        ADE_SPI_ReadReg(BWATTHR);
        ADE_SPI_ReadReg(CWATTHR);
        ADE_SPI_ReadReg(AVARHR);
        ADE_SPI_ReadReg(BVARHR);
        ADE_SPI_ReadReg(CVARHR);
        
        //ADE_SPI_ReadReg(AENERGYA);
        //ADE_SPI_ReadReg(RENERGYA);
        //ADE_SPI_ReadReg(APENERGYA);
        
    DTI_regProperty("ADE_Inst",     STRUCT_TYPE,   (uint32_t)&ADE_Inst,     0);
    DTI_regProperty("ADE_Cal",      STRUCT_TYPE,   (uint32_t)&ADE_Cal_phase[0], 0);
    DTI_regProperty("ADE_Crd_A",    FUNCTION_TYPE, (uint32_t)ADE_rd_Cal_A,  0);
    DTI_regProperty("ADE_Cwr_A",    FUNCTION_TYPE, (uint32_t)ADE_wr_Cal_A,  0);
    DTI_regProperty("ADE_Crd_B",    FUNCTION_TYPE, (uint32_t)ADE_rd_Cal_B,  0);
    DTI_regProperty("ADE_Cwr_B",    FUNCTION_TYPE, (uint32_t)ADE_wr_Cal_B,  0);
    DTI_regProperty("ADE_Crd_C",    FUNCTION_TYPE, (uint32_t)ADE_rd_Cal_C,  0);
    DTI_regProperty("ADE_Cwr_C",    FUNCTION_TYPE, (uint32_t)ADE_wr_Cal_C,  0);
    DTI_regProperty("ADE_Crd_N",    FUNCTION_TYPE, (uint32_t)ADE_rd_Cal_N,  0);
    DTI_regProperty("ADE_Cwr_N",    FUNCTION_TYPE, (uint32_t)ADE_wr_Cal_N,  0);
    DTI_regProperty("ADE_alfa",     FLOAT_TYPE,    (uint32_t)&ADE_alfa,     0);
}

void ADE_Configure_Outs()
{
    ADE_SPI_WriteReg(CF1DEN, 	1000 * 1000 * 50 / 57 / Settings.Pulse0_freq);                 
    ADE_SPI_WriteReg(CF2DEN, 	1000 * 1000 * 50 / 57 / Settings.Pulse1_freq);                 
//    ADE_SPI_WriteReg(CF1DEN, 	((uint32_t)UNITS_PER_WH) * 1000 / Settings.Pulse0_freq);                 
//    ADE_SPI_WriteReg(CF2DEN, 	((uint32_t)UNITS_PER_WH) * 1000 / Settings.Pulse1_freq);                 
    ADE_SPI_WriteReg(CFMODE, 	(Settings.Pulse1_energy_type << 3) | Settings.Pulse0_energy_type | (1<<11));        
}

void ADE_rd_Cal_A()
{
    ADE_Cal_phase[0].aigain  = ADE_SPI_ReadReg(AIGAIN);
    ADE_Cal_phase[0].avgain  = ADE_SPI_ReadReg(AVGAIN);
    ADE_Cal_phase[0].awgain  = ADE_SPI_ReadReg(AWGAIN);
    ADE_Cal_phase[0].avargain= ADE_SPI_ReadReg(AVARGAIN);
    ADE_Cal_phase[0].avagain = ADE_SPI_ReadReg(AVAGAIN);
    
    ADE_Cal_phase[0].airmsos = ADE_SPI_ReadReg(AIRMSOS);
    ADE_Cal_phase[0].vrmsos  = ADE_SPI_ReadReg(AVRMSOS);
    ADE_Cal_phase[0].awattos = ADE_SPI_ReadReg(AWATTOS);
    ADE_Cal_phase[0].avaros  = ADE_SPI_ReadReg(AVAROS);
    ADE_Cal_phase[0].avaos   = 0;//ADE_SPI_ReadReg(AVAOS);
    ADE_index_Cal = 0;
}

void ADE_wr_Cal_A()
{
    ADE_SPI_WriteReg(AIGAIN,    ADE_Cal_phase[0].aigain);
    ADE_SPI_WriteReg(AVGAIN,    ADE_Cal_phase[0].avgain);
    ADE_SPI_WriteReg(AWGAIN,    ADE_Cal_phase[0].awgain);
    ADE_SPI_WriteReg(AVARGAIN,  ADE_Cal_phase[0].avargain);
    ADE_SPI_WriteReg(AVAGAIN,   ADE_Cal_phase[0].avagain);
    
    ADE_SPI_WriteReg(AIRMSOS,   ADE_Cal_phase[0].airmsos);
    ADE_SPI_WriteReg(AVRMSOS,   ADE_Cal_phase[0].vrmsos);
    ADE_SPI_WriteReg(AWATTOS,   ADE_Cal_phase[0].awattos);
    ADE_SPI_WriteReg(AVAROS,    ADE_Cal_phase[0].avaros);
    //ADE_SPI_WriteReg(AVAOS, ADE_Cal.avaos);
//    ADE_was_calibrated |= 1 << 0;
}

void ADE_rd_Cal_B()
{
    ADE_Cal_phase[1].aigain  = ADE_SPI_ReadReg(BIGAIN);
    ADE_Cal_phase[1].avgain  = ADE_SPI_ReadReg(BVGAIN);
    ADE_Cal_phase[1].awgain  = ADE_SPI_ReadReg(BWGAIN);
    ADE_Cal_phase[1].avargain= ADE_SPI_ReadReg(BVARGAIN);
    ADE_Cal_phase[1].avagain = ADE_SPI_ReadReg(BVAGAIN);
    
    ADE_Cal_phase[1].airmsos = ADE_SPI_ReadReg(BIRMSOS);
    ADE_Cal_phase[1].vrmsos  = ADE_SPI_ReadReg(BVRMSOS);
    ADE_Cal_phase[1].awattos = ADE_SPI_ReadReg(BWATTOS);
    ADE_Cal_phase[1].avaros  = ADE_SPI_ReadReg(BVAROS);
    ADE_Cal_phase[1].avaos   = 0;//ADE_SPI_ReadReg(AVAOS);
    ADE_index_Cal = 1;
}

void ADE_wr_Cal_B()
{
    ADE_SPI_WriteReg(BIGAIN,    ADE_Cal_phase[1].aigain);
    ADE_SPI_WriteReg(BVGAIN,    ADE_Cal_phase[1].avgain);
    ADE_SPI_WriteReg(BWGAIN,    ADE_Cal_phase[1].awgain);
    ADE_SPI_WriteReg(BVARGAIN,  ADE_Cal_phase[1].avargain);
    ADE_SPI_WriteReg(BVAGAIN,   ADE_Cal_phase[1].avagain);
    
    ADE_SPI_WriteReg(BIRMSOS,   ADE_Cal_phase[1].airmsos);
    ADE_SPI_WriteReg(BVRMSOS,   ADE_Cal_phase[1].vrmsos);
    ADE_SPI_WriteReg(BWATTOS,   ADE_Cal_phase[1].awattos);
    ADE_SPI_WriteReg(BVAROS,    ADE_Cal_phase[1].avaros);
    //ADE_SPI_WriteReg(AVAOS, ADE_Cal.avaos);
//    ADE_was_calibrated |= 1 << 1;  
}

void ADE_rd_Cal_C()
{
    ADE_Cal_phase[2].aigain  = ADE_SPI_ReadReg(CIGAIN);
    ADE_Cal_phase[2].avgain  = ADE_SPI_ReadReg(CVGAIN);
    ADE_Cal_phase[2].awgain  = ADE_SPI_ReadReg(CWGAIN);
    ADE_Cal_phase[2].avargain= ADE_SPI_ReadReg(CVARGAIN);
    ADE_Cal_phase[2].avagain = ADE_SPI_ReadReg(CVAGAIN);
    
    ADE_Cal_phase[2].airmsos = ADE_SPI_ReadReg(CIRMSOS);
    ADE_Cal_phase[2].vrmsos  = ADE_SPI_ReadReg(CVRMSOS);
    ADE_Cal_phase[2].awattos = ADE_SPI_ReadReg(CWATTOS);
    ADE_Cal_phase[2].avaros  = ADE_SPI_ReadReg(CVAROS);
    ADE_Cal_phase[2].avaos   = 0;//ADE_SPI_ReadReg(AVAOS);
    ADE_index_Cal = 2;
}

void ADE_wr_Cal_C()
{
    ADE_SPI_WriteReg(CIGAIN,    ADE_Cal_phase[2].aigain);
    ADE_SPI_WriteReg(CVGAIN,    ADE_Cal_phase[2].avgain);
    ADE_SPI_WriteReg(CWGAIN,    ADE_Cal_phase[2].awgain);
    ADE_SPI_WriteReg(CVARGAIN,  ADE_Cal_phase[2].avargain);
    ADE_SPI_WriteReg(CVAGAIN,   ADE_Cal_phase[2].avagain);
    
    ADE_SPI_WriteReg(CIRMSOS,   ADE_Cal_phase[2].airmsos);
    ADE_SPI_WriteReg(CVRMSOS,   ADE_Cal_phase[2].vrmsos);
    ADE_SPI_WriteReg(CWATTOS,   ADE_Cal_phase[2].awattos);
    ADE_SPI_WriteReg(CVAROS,    ADE_Cal_phase[2].avaros);
    //ADE_SPI_WriteReg(AVAOS, ADE_Cal.avaos);
//    ADE_was_calibrated |= 1 << 2; 
}

void ADE_rd_Cal_N()
{
    ADE_Cal_phase[3].aigain  = ADE_SPI_ReadReg(NIGAIN);
    ADE_Cal_phase[3].avgain  = 0;
    ADE_Cal_phase[3].awgain  = 0;
    ADE_Cal_phase[3].avargain= 0;
    ADE_Cal_phase[3].avagain = 0;
    
    ADE_Cal_phase[3].airmsos = ADE_SPI_ReadReg(NIRMSOS);
    ADE_Cal_phase[3].vrmsos  = 0;
    ADE_Cal_phase[3].awattos = 0;
    ADE_Cal_phase[3].avaros  = 0;
    ADE_Cal_phase[3].avaos   = 0;
    ADE_index_Cal = 3;
}

void ADE_wr_Cal_N()
{
    ADE_SPI_WriteReg(NIGAIN,    ADE_Cal_phase[3].aigain);
    ADE_SPI_WriteReg(NIRMSOS,   ADE_Cal_phase[3].airmsos);
//    ADE_was_calibrated |= 1 << 3; 
}

void ADE_Set_Default_Cal()
{
    //ADE_Read_Cal();

    int i;
    for (i = 0; i < ADE_Cal_PHASE_MAX - 1; ++i) {
        ADE_Cal_phase[i].aigain  = 0x0FFE51F5;
        ADE_Cal_phase[i].avgain  = 0x0FF5A7C3;
        ADE_Cal_phase[i].awgain  = 0x0007C26B;
        ADE_Cal_phase[i].avargain= 0x0029BC5E;
        ADE_Cal_phase[i].avagain = 0x000CF55E;
        
        ADE_Cal_phase[i].airmsos = 0;
        ADE_Cal_phase[i].vrmsos  = 0;
        ADE_Cal_phase[i].awattos = 0;
        ADE_Cal_phase[i].avaros  = 0;
    }
    ADE_Cal_phase[ADE_Cal_PHASE_MAX - 1].aigain  = 0x0FF6EF1F;
    ADE_wr_Cal_A();
    ADE_wr_Cal_B();
    ADE_wr_Cal_C();
    ADE_wr_Cal_N();
}

#endif
