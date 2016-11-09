// Counts pulses on RS232_RX pin, used for calibration from reference meter
#include "pulsecalibration.h"
#include "main.h"

#include "em_device.h"
#include "em_gpio.h"
#include "em_chip.h"
#include "em_cmu.h"

#include "gpiointerrupt.h"
#include "meterage.h"
#include "mqueue.h"
#include "archive.h"
#include "display.h"
#include "ade7953.h"
//#include "leuart.h"


#define PULSE_INPUT 	                RS485_RX
#define PERIOD_IMP                      32000
#define KILO_WATT                       1000
#define TRY_COUNT                       3200


#define CALIBRATION_PERIOD              ((PERIOD_IMP/KILO_WATT)*TRY_COUNT)

typedef enum {
  WAIT_CALIB
  , START_CALIB
  , END_CALIB
}CALIB_STATES_E;


volatile uint32_t CalibAccum = 0;
volatile uint32_t pulsecounter = 0, RepeatCnt = 0;

bool NeedCalibration = false;
volatile CALIB_STATES_E CalibStates = WAIT_CALIB;


uint32_t Calibration_GetPulse( void )
{
  volatile uint32_t  param;
  param = pulsecounter;
  return param;
}

uint32_t Calibration_GetAccum( void )
{
  volatile uint32_t  param; 
  param = CalibAccum;
  return param;
}


void Calibration_StartNew( void )               { pulsecounter = 0; }
bool Calibration_IsNeedCalibration( void )      { return NeedCalibration; }



static void Calibration_PulseCallback( uint8_t pin )
{
  register int32_t reg;
 
  if( pin == PULSE_INPUT.pin ) 
  switch( CalibStates ) {
    case WAIT_CALIB:
      reg = ADE_SPI_ReadReg(AENERGYA);
      CalibAccum = reg;
      pulsecounter++;
      if( pulsecounter > 320 ) {
        pulsecounter = 0;
        CalibStates = START_CALIB;
        CalibAccum = 0;
      }
      break;
      
    case START_CALIB:
    if( !pulsecounter ) {
      ADE_SPI_ReadReg(AENERGYA);
      CalibAccum = 0;
    }
      
    reg = ADE_SPI_ReadReg(AENERGYA);
    if( reg >= 0 ) {
      CalibAccum += reg;
      pulsecounter++;
    } 
      //LeUART_SendCalibr(reg, ADE_SPI_ReadReg(IRMSA), ADE_SPI_ReadReg(VRMS), pulsecounter);
      
    if( pulsecounter > TRY_COUNT ) {
      mQueue_PutMsg( PULSE_CALIBRATION_EVENT, CalibAccum );
      CalibStates = END_CALIB;
    }
      break;
    
    case END_CALIB: 
      reg = ADE_SPI_ReadReg(AENERGYA);
      break;

    default: CalibStates = WAIT_CALIB; break;
  }
  

}

void Calibration_Init( bool enable )
{
  if( enable ) {
//LeUART_Init();    
	GPIO_PinModeSet( PULSE_INPUT.port, PULSE_INPUT.pin, gpioModeInputPull, 1 ); // RS485 RX
	GPIOINT_CallbackRegister( PULSE_INPUT.pin, Calibration_PulseCallback );
	GPIO_IntConfig( PULSE_INPUT.port, PULSE_INPUT.pin, true, false, true );
        Calibration_StartNew();
        NeedCalibration = true;
        RepeatCnt = 0;
        pulsecounter = 0;
      
  } else Calibration_Deinit();
}

void Calibration_Deinit( void )
{
        if ( !NeedCalibration ) return;
	GPIO_PinModeSet( PULSE_INPUT.port, PULSE_INPUT.pin, gpioModeDisabled, 0 ); // RS485 RX
	GPIOINT_CallbackRegister( PULSE_INPUT.pin, NULL );
	GPIO_IntConfig( PULSE_INPUT.port, PULSE_INPUT.pin, false, false, false );
        pulsecounter = 0;
        RepeatCnt = 0;
        NeedCalibration = false;
//LeUART_Deinit();
        //LeUART_TxDisable();
}


uint32_t Calibration_Calculate( uint32_t param )
{
  float KoefCalib, middle_watt, watt_k;
  
  middle_watt =(float)param / (float)TRY_COUNT;
  watt_k = middle_watt * ((float)PERIOD_IMP / (float)KILO_WATT);

  KoefCalib = ((float)Archive_GetCalibrateSettings()->EnGain * (float)Archive_GetCalibrateSettings()->WattH ) / watt_k;
  Archive_SetEnGain( (uint32_t)KoefCalib );

  if( Archive_GetCalibrateSettings()->EnGain < 0x200000
     ||  Archive_GetCalibrateSettings()->EnGain > 0x600000 ) {
     Archive_SetEnGain( 0x400000 );
     Archive_SetWattH( 2000.0f );
     CalibStates = WAIT_CALIB;
     pulsecounter = 0;
     return 0x400000;
  }
  
  int32_t calib_error = (int32_t)watt_k - (uint32_t)Archive_GetCalibrateSettings()->WattH;
  
  if( (calib_error > 2) || (calib_error < -2 )) {
      CalibStates = WAIT_CALIB;
      pulsecounter = 0;
     return (uint32_t)KoefCalib;
  }

    Calibration_Deinit();
    Display_ReleaseCom();
    
//    if( ( Archive_CalcTarifCrc() == Archive_GetTarifSettings()->crc ) && Archive_GetTarifSettings()->TarifTitleTotal && Archive_GetTarifSettings()->TarifZoneTotal ) 
//    {
      Archive_SaveSettings(true, true);
//    } else Archive_SaveSettings(true, false);    
    
    Display_RestoreCom();
    //ADE_SetCalibrate(true);

  return (uint32_t)KoefCalib;
}
