/**************************************************************************//**
 * @file
 * @brief FreeRTOS Tickless Demo for Energy Micro EFM32-Gxxx-DK Development Kit
 * @version 4.0.0
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>

#include "stlcd.h"
   
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "croutine.h"

#include "em_rtc.h"
#include "em_chip.h"
#include "em_adc.h"
#include "bsp_trace.h"

#include "sleep.h"

#include "u8g.h"
#include "u8g_arm.h"

#define STACK_SIZE_FOR_TASK    (configMINIMAL_STACK_SIZE + 10)
#define TASK_PRIORITY          (tskIDLE_PRIORITY + 1)
#define DELAY ( 700 / portTICK_RATE_MS )

#include "e_meter.h"
//PRIVILEGED_DATA static volatile TickType_t xTickCount 				= ( TickType_t ) 0U;
extern TickType_t xTickCount 				= ( TickType_t ) 0U;
uint32_t sTicks;

/* Semaphores used by application to synchronize two tasks */
xSemaphoreHandle sem;
/* Text to display */

char text[128];
/* Counter start value*/
int count = 0;
uint32_t soft_time=0;
static volatile bool adcConversionComplete = false;
extern volatile uint32_t rtc_seconds;

/**************************************************************************//**
 * @brief Count task which is preparing next number to display
 * @param *pParameters pointer to parameters passed to the function
 *****************************************************************************/

uint32_t SoftRTCTimer=0;

#define BFL_PORT				SDA_PORT										// +3B_FL line (индикатор наличия напряжения внешнего электропитания, переключить "как вход" на время неактивного состояния LCD)
#define BFL_BIT					SDA_BIT
#define BFL_PIN					SDA_PIN
#define BFL_MASK				SDA_MASK
#define EXT_PHASE_A_SENS_INIT	GPIO_PinModeSet (BFL_PORT, BFL_BIT, gpioModeInput, 1)
#define GET_PHASE_A_STATUS		GPIO_PinInGet (BFL_PORT, BFL_BIT)
#define ST765R_GPIO_A0   		SDA_BIT											// A0
#define ST765R_GPIO_A0_PORT   	SDA_PORT

volatile int phase_status;

uint32_t AdcData=0;
uint32_t ButtonPressed=0;
static void BtnsCheck(void *pParameters)
{
  pParameters = pParameters;   /* to quiet warnings */
   uint32_t vData;
  for (;;)
  {
   
	/* Sample ADC */
	adcConversionComplete = false;
	ADC_Start (ADC0, adcStartSingle);
	while (!adcConversionComplete) {			// may hand-up (?)
		vData++;
		if (vData >5000000) break;//return (uint32_t)(-1);
//		EMU_EnterEM1 ();
	}
	AdcData = ADC_DataSingleGet(ADC0);
        if (AdcData<100)
          ButtonPressed=1;
        else if ( (AdcData>600) && (AdcData<1000))
            ButtonPressed=2;
        else if ( (AdcData>1800) && (AdcData<2200))
            ButtonPressed=3;
        else ButtonPressed=0;
  }
}

static void LcdRedraw(void *pParameters)
{
  pParameters = pParameters;   /* to quiet warnings */
  for (;;)
  {
   //if (pdTRUE == xSemaphoreTake(sem, portMAX_DELAY))	
      GPIO_PinModeSet (ST765R_GPIO_A0_PORT,	ST765R_GPIO_A0, 	gpioModePushPull, 0);	//      - SDA
      __display_write_buffer();
      GPIO_PinModeSet (ST765R_GPIO_A0_PORT,	ST765R_GPIO_A0, 	gpioModeInputPull, 0);	//      - SDA
      sTicks = xTickCount/portTICK_RATE_MS;
      vTaskDelay(333/portTICK_RATE_MS);
      sTicks = xTickCount/portTICK_RATE_MS;
      PowerMonitor();
   
  }
}

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
void adcSetup (void);
u8g_t u8g;

void SwitchOn();
void SwitchOff();
int main(void)
{
  /* Chip errata */
  CHIP_Init();


  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Initialize SLEEP driver, no calbacks are used */
  SLEEP_Init(NULL, NULL);
#if (configSLEEP_MODE < 3)
  /* do not let to sleep deeper than define */
  SLEEP_SleepBlockBegin((SLEEP_EnergyMode_t)(configSLEEP_MODE+1));
#endif

  //u8g_InitComFn(&u8g, &u8g_dev_st7565_dogm128_sw_spi, u8g_com_st7565em);
// st7565_init ();
//  st7565_command (CMD_DISPLAY_ON);
//  st7565_command (CMD_SET_ALLPTS_NORMAL);
//  st7565_set_brightness (0x06);
  st7565_init ();
  st7565_command (CMD_DISPLAY_ON);
  st7565_command (CMD_SET_ALLPTS_NORMAL);
  st7565_set_brightness (0x06); 
  __display_write_buffer();
INPirqSetup();
GPIO_PinModeSet (ROFF_PORT,  ROFF_BIT, gpioModeDisabled, 0);		
ext_init();

 __display_write_buffer();
  u8g_Init(&u8g, &u8g_dev_vs);
  u8g_SetVirtualScreenDimension(&u8g, 128, 64);

  adcSetup ();
  //__display_write_buffer();
  /* Initialize the LCD driver */

  /* Create standard binary semaphore */
//  vSemaphoreCreateBinary(sem);

  
  //xTaskCreate(SoftwareRTC, (const char *) "SoftwareRTC", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(taskPowerCount, (const char *) "taskPowerCount", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY+1, NULL);
  xTaskCreate(taskTimeMonitor, (const char *) "TimeMonitor", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY+1, NULL);
  
  xTaskCreate(taskDisplay, (const char *) "Display Service", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(LcdRedraw, (const char *) "LcdRedraw", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);

  //xTaskCreate(taskPowerMonitor, (const char *) "taskPowerMonitor", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(BtnsCheck, (const char *) "ButtonsCheck", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  xTaskCreate(taskRadioSend, (const char *) "taskRadioSend", STACK_SIZE_FOR_TASK, NULL, TASK_PRIORITY, NULL);
  
 
  
  //
  /* Start FreeRTOS Scheduler */
  vTaskStartScheduler();

  return 0;
}

void ADC0_IRQHandler(void)
{
  uint32_t flags;

  /* Clear interrupt flags */
  flags = ADC_IntGet (ADC0);
  ADC_IntClear (ADC0, flags);

  adcConversionComplete = true;
}
void adcSetup (void)
{
	/* Enable ADC clock */
	CMU_ClockEnable (cmuClock_ADC0, true);

	/* Initiate ADC peripheral */
	ADC_Init_TypeDef init = ADC_INIT_DEFAULT;
	ADC_Init (ADC0, &init);

	/* Setup single conversions for Ch6 */
	ADC_InitSingle_TypeDef initSingle = ADC_INITSINGLE_DEFAULT;
	initSingle.prsSel    = adcPRSSELCh6;
	initSingle.acqTime   = adcAcqTime16;
	initSingle.reference = adcRefVDD;
	initSingle.input     = adcSingleInpCh6;
	ADC_InitSingle (ADC0, &initSingle);

	/* Manually set some calibration values */
	ADC0->CAL = (0x7C << _ADC_CAL_SINGLEOFFSET_SHIFT) | (0x1F << _ADC_CAL_SINGLEGAIN_SHIFT);

	/* Enable interrupt on completed conversion */
	ADC_IntEnable (ADC0, ADC_IEN_SINGLE);
	NVIC_ClearPendingIRQ (ADC0_IRQn);
	NVIC_EnableIRQ (ADC0_IRQn);
}