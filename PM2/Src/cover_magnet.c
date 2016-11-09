#include "cover_magnet.h"
#include <em_cmu.h>
#include <em_adc.h>
#include <em_gpio.h>
#include "main.h"
#include "archive.h"
//#include "event.h"

static bool InitCover=false, InitMagnet=false, InitCoverMagnet = false, MagnetEvent = false;
static uint8_t MagnetCnt = 0;

void CoverMagnet_Init(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);
  gpio_INI_S3S4_SUP;
//  GPIO_PinModeSet(S3S4_SUP.port, S3S4_SUP.pin, gpioModePushPull, 1);
  InitCoverMagnet = true;
}

void CoverMagnet_Deinit(void)
{
  if(InitCover) Cover_Deinit();
  if(InitMagnet) Magnet_Deinit();
  
  if( !InitCoverMagnet ) return;
  gpio_OFF_S3S4_SUP;
//  GPIO_PinModeSet(S3S4_SUP.port, S3S4_SUP.pin, gpioModeDisabled, 0);
  InitCoverMagnet = false;
}

void Cover_Init(void)
{
  if( !InitCoverMagnet ) CoverMagnet_Init();
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_ADC0, true);
  
  ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;
  
  /* Init common settings for both single conversion and scan mode */
  init.timebase = ADC_TimebaseCalc(0);
  /* Might as well finish conversion as quickly as possibly since polling */
  /* for completion. */
  /* Set ADC clock to 7 MHz, use default HFPERCLK */
  init.prescale = ADC_PrescaleCalc(7000000, 0);

  /* WARMUPMODE must be set to Normal according to ref manual before */
  /* entering EM2. In this example, the warmup time is not a big problem */
  /* due to relatively infrequent polling. Leave at default NORMAL, */

  ADC_Init(ADC0, &init);

  /* Init for single conversion use, measure VDD/3 with 1.25 reference. */
  singleInit.reference  = adcRef1V25;
  singleInit.input      = adcSingleInpCh1; // PE13
  singleInit.resolution = adcRes12Bit;

  /* The datasheet specifies a minimum aquisition time when sampling vdd/3 */
  /* 32 cycles should be safe for all ADC clock frequencies */
  singleInit.acqTime = adcAcqTime32;
  ADC_InitSingle(ADC0, &singleInit);

  ADC_Start(ADC0, adcStartSingle);  
  InitCover = true;  
}

void Cover_Deinit(void)
{
  if ( !InitCover ) return;
  CMU_ClockEnable(cmuClock_ADC0, false); 
  gpio_OFF_S3S4_SUP;
//  GPIO_PinModeSet(S3S4_SUP.port, S3S4_SUP.pin, gpioModeDisabled, 0);
  InitCover = false;  
}

bool Cover_IsOpen(void)
{
  return true;     //функция отключена
//  static bool CoverEvent = false, CoverStart = false;
//  uint32_t power;
//  if( !InitCover ) Cover_Init();
//
//  /* Wait while conversion is active */
//  while (ADC0->STATUS & ADC_STATUS_SINGLEACT) ;
//
//  /* Get ADC result */
//  
//  power = ADC_DataSingleGet(ADC0);
//  
//  if( !CoverStart ) {   //подавление выдачи ивента при закрытой крышке и рестарте
//    CoverStart = true;
//    if( power > 100 )
//      CoverEvent = false;
//  }
//  
//  if( power > 100 ) {
//     if( CoverEvent ) {
//      CoverEvent = false;
//      Event_Send(COVER_CLOSE_ARCH_EVENT, __time32(0));
//    }
//  } else {
//    if( !CoverEvent ) {
//      CoverEvent = 1;      
//      Event_Send(COVER_OPEN_ARCH_EVENT, __time32(0));
//    }
//  }
//  return (power > 100);
}



void Magnet_Init(void)
{
  if( !InitCoverMagnet ) CoverMagnet_Init();
  CMU_ClockEnable(cmuClock_GPIO, true);
  gpio_INI_S4;
//  GPIO_PinModeSet(S4.port, S4.pin, gpioModeWiredAndPullUpFilter, 1);
  InitMagnet = true;
}
void Magnet_Deinit(void)
{
  if( !InitMagnet ) return;
  gpio_OFF_S4;
//  GPIO_PinModeSet(S4.port, S4.pin, gpioModeDisabled, 0); 
  InitMagnet = false;
}

bool Magnet_IsCloser(void)
{
  bool pin;
  if( !InitCoverMagnet ) CoverMagnet_Init();
  if( !InitMagnet ) Magnet_Init();

  pin = gpio_GET_S4;
//  pin = GPIO_PinInGet(S4.port, S4.pin);

  if( !pin ) {
    MagnetCnt++;
    if( MagnetCnt > 10 ) {
      if ( !MagnetEvent ) {
        Event_Send(MAGNET_CLOSER_ARCH_EVENT, __time32(0));
        MagnetEvent = true;
      }
       MagnetCnt--;
    }
  } else {
    MagnetCnt = 0;
    MagnetEvent = false;
  }
  
  return pin;
}