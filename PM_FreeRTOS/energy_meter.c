#include "em_gpio.h"
#include "e_meter.h"
#include "em_cmu.h"
#include "em_rmu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "stlcd.h"
//#include "u8g.h"
#include "string.h"

#include "semtech.h"
#include "unb.h"
#include "u8g.h"
#include <stdio.h>
#include <time.h>

#define SELF_TEST
#define SHOW_TIME
#define DEBUG_PAGE

#define TARIF_NUM 3
#define MEASURES 5

#define TIMEZONE 3
#define TIMEFIX TIMEZONE*60*60

#define PW_INT_PERIOD 4
#define WT_P_IMPS 1160//11.88 Вт\Имп\с
#define IPM_P_10WTH 2974 //2864 Импульсов на 10 вт часов

#define SCREEN_TIME 5000
#define SEND_DELAY 27000
//#define BUTTONS_CHECK

struct tm *tm_local;
time_t unix_time=1442466000+TIMEFIX;  //Sun, 20 Sep 2015 05:00:00 GMT
extern u8g_t u8g;
extern uint32_t AdcData;
extern uint32_t ButtonPressed;
uint32_t PwLimit=0xFFFFFFFF;
uint32_t PwCur=0;

uint8_t RSSI,curRSSI,SNR;
uint8_t packetRec[128], reply;


uint32_t INP1_CNT, INP2_CNT;
const char * tarif_names[] = {"Сумма", "Пик", "Полупик", "Ночь"};
const char * measure_names[] = {"Всего", "Сутки", "Неделя", "Месяц", "Год"};
uint8_t tarif_cur=1;
uint32_t PwCons[TARIF_NUM+1][MEASURES]; //day week month year summ

void SwitchOn();
void SwitchOff();

void delay(uint32_t msec)
{vTaskDelay(msec/portTICK_RATE_MS);
}

void PowerMonitor ()
{  
  if (PwCur>PwLimit)
  {
    PwCur=PwCur;
    SwitchOff();
  }
  if (ButtonPressed==1)
    SwitchOn();
  if (ButtonPressed==2)
    SwitchOff();
  vTaskDelay(100/portTICK_RATE_MS);
  
}

void taskRadioSend (void *pParameters)
{
  pParameters = pParameters;   
  
  uint32_t send_time, send_pw,temp;
  uint8_t pl[8]={0xFE,0xFE,0xFE,0xFE};
  uint8_t i;
  UNB_Init(0);
  for (;;)
  {
    for (i=0;i<TARIF_NUM+1;i++)
    {
      if(i==0)
        pl[0]=10; //14 15 30 31 46 47 62 63
      if(i==1)
        pl[0]=14; //14 15 30 31 46 47 62 63
      if(i==2)
        pl[0]=30; //14 15 30 31 46 47 62 63
      if(i==3)
        pl[0]=46; //14 15 30 31 46 47 62 63
      
      send_time = (unix_time-TIMEFIX)/300;
    pl[1] = (send_time>>16)&0xFF;
    pl[2] = (send_time>>8)&0xFF;
    pl[3] = (send_time>>0)&0xFF;
    send_pw = PwCons[i][0];
    pl[4] = (send_pw>>24)&0xFF;
    pl[5] = (send_pw>>16)&0xFF;
    pl[6] = (send_pw>>8)&0xFF;
    pl[7] = (send_pw>>0)&0xFF;
   // reply=0;
   // vTaskDelay(3000/portTICK_RATE_MS);
   //memcpy(pl,&INP1_CNT,4);
     //memset(packetRec, 0x00, sizeof(packetRec));
      reply = UNB_SendPacket(PROTOCOL_E, pl, 8);
      
      if (reply>3)
      {
        if (packetRec[3]==0xF5) //time
        {
          temp=packetRec[7]<<24 | packetRec[8]<<16 | packetRec[9]<<8 | packetRec[10];
            unix_time = temp + TIMEFIX;
        }
        if (packetRec[3]==0xF6) //time
        {
          temp=packetRec[7]<<24 | packetRec[8]<<16 | packetRec[9]<<8 | packetRec[10];
            
            if (temp == 0)
              SwitchOff();
            if ((temp>0) && (PwLimit==0))
              SwitchOn();
            PwLimit = temp;
            vTaskDelay(100/portTICK_RATE_MS);
        }
      }
        
    vTaskDelay(SEND_DELAY/portTICK_RATE_MS);
    }
  }
}
  

#define PPS_OUT
void taskTimeMonitor (void *pParameters)
{
   pParameters = pParameters;   /* to quiet warnings */
   time_t prev_t;
        tarif_cur=GetTarifFromTime(unix_time);
        
  
for (;;)
{ 
  ext_ctrl (PE_PULSE_OUT, PE_ON);
  vTaskDelay(400/portTICK_RATE_MS);
  unix_time++; //колхоз
  if (prev_t!=unix_time)
  {tm_local=gmtime(&unix_time);
   prev_t=unix_time;
   //tarif_cur=1; //надо сделать тарифное расписание. если неизвестно что то Т1
  }
  ext_ctrl (PE_PULSE_OUT, PE_OFF);
   vTaskDelay(550/portTICK_RATE_MS); //пока такая хрень

   
    

}
}

void taskPowerCount (void *pParameters)
{
     pParameters = pParameters;   /* to quiet warnings */
     #ifndef PW_CALIBRATE

     uint32_t PrInp0, CurInp0, Inc0, IncGl=0;
     uint32_t DiffCons[PW_INT_PERIOD];
     uint32_t PwAgv;
     int i;
     //char text[128];
     for (i=0;i<PW_INT_PERIOD;i++)
        DiffCons[i]=0;
    #endif
for (;;)
{ 
#ifndef PW_CALIBRATE
  CurInp0 = INP1_CNT;
  Inc0 = CurInp0 - PrInp0;
  if (INP1_CNT>=IPM_P_10WTH)
  {IncGl=INP1_CNT/IPM_P_10WTH;
  INP1_CNT=INP1_CNT%IPM_P_10WTH;
  }
//  PwCur=Inc0;
for (i=0;i<MEASURES;i++)
  {
    PwCons[tarif_cur][i]+=IncGl*10;
    PwCons[0][i]+=IncGl*10;
  }
  IncGl=0;
  PrInp0=CurInp0;
  PwAgv=0;
  for (i=1;i<PW_INT_PERIOD;i++)
  {
    PwAgv+=DiffCons[i];
    DiffCons[i-1]=DiffCons[i];
  }
  DiffCons[i-1]=Inc0;
  PwAgv+=Inc0;
  PwCur=PwAgv*WT_P_IMPS/PW_INT_PERIOD;

  vTaskDelay(990/portTICK_RATE_MS);
#endif
}
}


void taskDisplay (void *pParameters)
{
  pParameters = pParameters;   /* to quiet warnings */
  TickType_t xTicksToWait = SCREEN_TIME/portTICK_RATE_MS;
  TimeOut_t xTimeOut;
    char text[128];
    int trf,i;
  //uint32_t prev_s=0;
#ifdef SELF_TEST
   // display_text("Диагностика", 10,0);
  sprintf(text,"%08x",RMU_ResetCauseGet());
  display_text(text,0,0);
  for (int st=0;st<=100;st+=7)
  {
    sprintf(text,"%3d%",st);
    display_text(text,0,20);
    vTaskDelay(100/portTICK_RATE_MS);
  }
  display_text(" ОК ", 0,20);
    vTaskDelay(500/portTICK_RATE_MS);

#endif
  vTaskDelay(2000/portTICK_RATE_MS);
    lcd_clear(1);
    
  for (;;)
  {
#if defined(DEBUG_PAGE) || defined(PW_CALIBRATE)
    for (i=0;i<20*2;i++)
    {
      lcd_clear(1);
      sprintf (text, "em strij debug");
      display_text(text, 1,0);
      sprintf (text, (char*)((const char*)"%02u:%02u:%02u"), tm_local->tm_hour, tm_local->tm_min, tm_local->tm_sec);
      display_text(text, 1,1);
      sprintf (text, "%02u.%02u.%u", tm_local->tm_mday, tm_local->tm_mon+1, tm_local->tm_year+1900);	// ДД.ММ.ГГГГ
      display_text(text, 1,2);
      sprintf (text, "R=%d:%02x%02x%02x%02x%02x%02x%02x%02x", reply, \
       packetRec[0], packetRec[1], packetRec[2], packetRec[3], \
       packetRec[4], packetRec[5], packetRec[6], packetRec[7] );	// ДД.ММ.ГГГГ
      display_text(text, 1,3);
      sprintf (text, "INP1 = %d", INP1_CNT);	// ДД.ММ.ГГГГ
      display_text(text, 1,4);
      sprintf (text, "INP2 = %d kP=%.3f", INP2_CNT,INP1_CNT>0?(float)INP1_CNT/INP2_CNT:0.0);	// ДД.ММ.ГГГГ
      display_text(text, 1,5);
      sprintf (text, "strijtime %06x", (unix_time-TIMEFIX)/300);	// ДД.ММ.ГГГГ
      display_text(text, 1,6);
      sprintf (text, "Button = %d", ButtonPressed);	// ДД.ММ.ГГГГ
      display_text(text, 1,7);
       vTaskSetTimeOutState( &xTimeOut );
       xTicksToWait = SCREEN_TIME/(20)/portTICK_RATE_MS;
      while(1)
       {
         if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
           break;
       }

    }
#endif
#ifndef PW_CALIBRATE
    
    for (trf=0;trf<=TARIF_NUM;trf++)
    {
     
       for (i=1;i<MEASURES;i++)
       {
        lcd_clear(1);
      
       //display_text(text, 1,1);
       u8g_SetFont(&u8g,u8g_font_unifont_0_8);
       u8g_SetFontPosTop(&u8g);
       
       sprintf(text,"T%d %s",trf, tarif_names[trf]);
       u8g_DrawStr(&u8g,0, 0, text); 
       sprintf(text,"%-7s: %d.%02d",measure_names[i], PwCons[trf][i]/1000,(PwCons[trf][i]%1000)/10); 
   
       u8g_DrawStr(&u8g,0, 17, text); 
   
       u8g_SetFont(&u8g,u8g_font_fur20);
       u8g_SetFontPosBottom(&u8g);
       sprintf(text,"%d.%02d",PwCons[trf][i]/1000,(PwCons[trf][i]%1000)/10); 
       u8g_DrawStr(&u8g,0, 63,text);  
       vTaskSetTimeOutState( &xTimeOut );
       xTicksToWait = SCREEN_TIME/(MEASURES-1)/portTICK_RATE_MS;
       while(1)
       {
         if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
           break;
         #ifdef BUTTONS_CHECK
         if (ButtonPressed==1)
         {vTaskDelay(50/portTICK_RATE_MS);
           break;
         }
#endif
       }
//       #ifdef BUTTONS_CHECK
 //         if (ButtonPressed==1)
//          { ButtonPressed=0; break; }
//#endif
#endif
      }
   
    }
#ifdef SHOW_TIME
    
    for (i=0;i<SCREEN_TIME/1000;i++)
    {
      lcd_clear(1);
      u8g_SetFont(&u8g,u8g_font_unifont_0_8);
      u8g_SetFontPosTop(&u8g);
      sprintf(text,"Дата\\время");
      u8g_DrawStr(&u8g,0, 0, text); 
      
      u8g_SetFont(&u8g,u8g_font_fur17);
       u8g_SetFontPosBottom(&u8g);
       sprintf(text,"%d.%02d",PwCons[trf][i]/1000,(PwCons[trf][i]%1000)/10); 
       
    sprintf (text, (char*)((const char*)"%02u:%02u:%02u"), tm_local->tm_hour, tm_local->tm_min, tm_local->tm_sec);
   u8g_DrawStr(&u8g,0, 40, text);  
    sprintf (text, "%02u.%02u.%u", tm_local->tm_mday, tm_local->tm_mon+1, tm_local->tm_year+1900);	// ДД.ММ.ГГГГ
    u8g_DrawStr(&u8g,0, 63, text); 
           vTaskSetTimeOutState( &xTimeOut );
       xTicksToWait = 1000/portTICK_RATE_MS;
       while(1)
       {
         if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
           break;
         #ifdef BUTTONS_CHECK
         if (ButtonPressed==1)
         {vTaskDelay(50/portTICK_RATE_MS);
           break;
         }
#endif
       }
//       #ifdef BUTTONS_CHECK
//           if (ButtonPressed==1)
//          { ButtonPressed=0; vTaskDelay(50/portTICK_RATE_MS); break; }
//#endif
    }
#endif
    lcd_clear(1);
    
   
     for (i=0;i<SCREEN_TIME/1000;i++)
{
      lcd_clear(1);
      u8g_SetFont(&u8g,u8g_font_unifont_0_8);
      u8g_SetFontPosTop(&u8g);
      sprintf(text,"Мощность");
      u8g_DrawStr(&u8g,0, 0, text); 
      if (PwLimit!=0xFFFFFFFF) 
      sprintf(text,"Ограничение %d Вт",PwLimit);
      else 
        sprintf(text,"Ограничения нет");
       u8g_DrawStr(&u8g,0, 17, text);
      u8g_SetFont(&u8g,u8g_font_fur17);
       u8g_SetFontPosBottom(&u8g);
       sprintf(text,"%d.%02d",PwCur/100,PwCur%100); 
       
     u8g_DrawStr(&u8g,0, 63, text); 
           vTaskSetTimeOutState( &xTimeOut );
       xTicksToWait = 1000/portTICK_RATE_MS;
       while(1)
       {
         if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
           break;
         #ifdef BUTTONS_CHECK
         if (ButtonPressed==1)
         {vTaskDelay(50/portTICK_RATE_MS);
           break;
         }
#endif
       }
//       #ifdef BUTTONS_CHECK
//           if (ButtonPressed==1)
//          { ButtonPressed=0; break; }
//#endif
    }
  }
}
int CheckExtPower()
{
/*                 st7565_command (CMD_DISPLAY_OFF);
		             //DelayNOP (1000000);
                             EXT_PHASE_A_SENS_INIT;													// устанвливаем режим pin-а для мониторинга напряжения по фазе A (вход)
		DelayNOP (1000000);
		phase_status = GET_PHASE_A_STATUS;
                // (0..7) индикатор наличия напряжения по фазе A
		GPIO_PinModeSet (ST765R_GPIO_A0_PORT, ST765R_GPIO_A0, gpioModePushPull, 0);	// восстанавливаем режим pin-а для работы с LCD (выход)
                st7565_init ();
  st7565_command (CMD_DISPLAY_ON);
 st7565_command (CMD_SET_ALLPTS_NORMAL);
  st7565_set_brightness (0x06);
  
  return phase_status;	
  */
  return 0;
}
uint16_t GetTarifFromTime(time_t time)
{
  uint16_t t=tm_local->tm_hour;
  if ( (t>=7) && (t<10))
    return 1; //T1
  if ((t>=17) && (t<21))
    return 1; //T1
  if ((t>=10) && (t<17))
    return 1; //T2
  if ((t>=21) && (t<23))
    return 1; //T2
  if ((t>=23))
    return 3; //T3
  if ((t<7))
    return 3; //T3
  return 1; //что-то пошло не так
}


void INPirqSetup (void)
{
	// Set up a user hook on INP interrupt
	// Note: both inputs are EVEN
	//int INP_type;
  CMU_ClockEnable (cmuClock_GPIO, true);

//	#define GPIO_OUTPUT_MODE gpioModeWiredAndPullUp
//	#define GPIO_OUTPUT_MODE gpioModePushPull
	#define GPIO_OUTPUT_MODE gpioModePushPullDrive
	
        //EXT_PHASE_A_SENS_INIT;														// +3B_FL line (индикатор наличия фазного напряжения)
        //GPIO_PinModeSet (BFL_PORT, BFL_BIT, gpioModeInput, 0);
	GPIO_PinModeSet (POW_PORT, POW_BIT, gpioModePushPull, 1);					// LCD_POW
		// INTERNAL RELAY
	//GPIO_PinModeSet (ROFF_PORT,  ROFF_BIT, gpioModeWiredAnd, 0);				// INTERNAL RELAY
	RELAY_OFF_CLEAR;
//GPIO_PinModeSet (ST765R_GPIO_A0_PORT, ST765R_GPIO_A0, gpioModePushPull, 0);

	//GPIO_PinModeSet (SENS_PORT, SENS_BIT, gpioModeInputPull, 1);				// case open sensor

	/* Configure INP1 as input to drive pulse counter, energy */
	GPIO_PinModeSet (INP1_PORT, INP1_BIT, gpioModeInputPull, 1);	

	//GPIO_PinModeSet (INP1_PORT, INP1_BIT, gpioModeInput, 1);					// ok
	//GPIO_PinModeSet (INP1_PORT, INP1_BIT, gpioModeInputPullFilter, 1);
	//GPIO_PinModeSet (INP1_PORT, INP1_BIT, gpioModePushPull, 1);

	//INP_type = 2; //TD GPIO ODD HOOK ? wtf
//	TD_GPIO_SetCallback (INP_type, INP_Interrupt, INP1_MASK|INP2_MASK);
	GPIO_IntConfig (INP1_PORT, INP1_BIT, true, false, true);					// port, bit, use_rising, use_falling, enable
 
	//GPIO_IntConfig (INP2_PORT, INP2_BIT, false, true, true);					// port, bit, use_rising, use_falling, enable
        #ifdef PW_CALIBRATE

        GPIO_PinModeSet (INP2_PORT, INP2_BIT, gpioModeInputPull, 1);   // ok
        GPIO_IntConfig (INP2_PORT, INP2_BIT, false, true, true);	
#endif
	INPirqEnable (true);

	IRQn_Type INP_irq_parity = GPIO_ODD_IRQn;
	NVIC_ClearPendingIRQ (INP_irq_parity);
	NVIC_EnableIRQ (INP_irq_parity);
}

void INPirqEnable (bool en)
{

	uint32_t inp_mask=INP1_MASK|INP2_MASK;

	if (en) {
		GPIO_IntClear (inp_mask);
		GPIO_IntEnable (inp_mask);
	}
	else {
		GPIO_IntDisable (inp_mask);
	}
}

void DelayNOP (uint32_t i)
{
        while (i--) asm volatile("nop");
}
static void SPI_pause (void)
{
	asm volatile("nop");
}


void ext_write (uint8_t byte) //TODO clean up
{
	//GPIO_PinOutSet(LCD_POWER_PORT, LCD_POWER_PIN);
	//SPI_pause ();SPI_pause ();
	//ext_init ();

	for (int i=0; i <8; i++)
	{
		if ((byte >> i) & 1)
		{
		GPIO_PinOutSet (PE_DATA_PORT, PE_DATA_PIN);
		}
		else
		{
			GPIO_PinOutClear (PE_DATA_PORT, PE_DATA_PIN);
		}

		SPI_pause ();
		GPIO_PinOutSet (PE_CLK_PORT, PE_CLK_PIN);
		SPI_pause ();
		GPIO_PinOutClear (PE_CLK_PORT, PE_CLK_PIN);
	}

	GPIO_PinOutSet (PE_STR_PORT, PE_STR_PIN);
	SPI_pause ();
	GPIO_PinOutClear (PE_STR_PORT, PE_STR_PIN);
	SPI_pause ();
}

uint8_t ext_ctrl (uint8_t obj, uint8_t status)
{
	static uint8_t st = 0;
#if (0)
	if (status ==PE_GET) {
		status = 0;
		#if ENABLE_LOGO_CONTROL >0
		if (obj&PE_LOGO_LED)    status |= PE_LOGO_LED;
		#endif
		if (obj&PE_RS485_TX_EN) status |= PE_RS485_TX_EN;
		if (obj&PE_PULSE_OUT)   status |= PE_PULSE_OUT;
		if (obj&PE_LCD_RES)     status |= PE_LCD_RES;
		if (obj&PE_RELAY_ON)    status |= PE_RELAY_ON;
		if (obj&PE_BACKLIGHT)   status |= PE_BACKLIGHT;
		if (obj&PE_RED_LED)     status |= PE_RED_LED;
		return status;
	}
#endif
	#if ENABLE_LOGO_CONTROL >0
	if (obj&PE_LOGO_LED)    { if (status ==PE_ON) st |= PE_LOGO_LED;    else st &= ~PE_LOGO_LED; }
	#endif
	if (obj&PE_RS485_TX_EN) { if (status ==PE_ON) st |= PE_RS485_TX_EN; else st &= ~PE_RS485_TX_EN; }
	if (obj&PE_PULSE_OUT)   { if (status ==PE_ON) st |= PE_PULSE_OUT;   else st &= ~PE_PULSE_OUT; }
	if (obj&PE_LCD_RES)     { if (status ==PE_ON) st |= PE_LCD_RES;     else st &= ~PE_LCD_RES; }
	if (obj&PE_RELAY_ON)    { if (status ==PE_ON) st |= PE_RELAY_ON;    else st &= ~PE_RELAY_ON; }
	if (obj&PE_BACKLIGHT)   { if (status ==PE_ON) st |= PE_BACKLIGHT;   else st &= ~PE_BACKLIGHT; }
	if (obj&PE_RED_LED)     { if (status ==PE_ON) st |= PE_RED_LED;     else st &= ~PE_RED_LED; }
	ext_init ();
	ext_write (st);

	return st;
}

void ext_init (void)
{
	// GPIO init
	CMU_ClockEnable (cmuClock_GPIO, true);

//	#define GPIO_OUTPUT_MODE gpioModeWiredAndPullUp
//	#define GPIO_OUTPUT_MODE gpioModePushPull
	#define GPIO_OUTPUT_MODE gpioModePushPullDrive
	GPIO_PinModeSet (PE_DATA_PORT, PE_DATA_PIN, GPIO_OUTPUT_MODE, 0);			// (D) SDO - TIM2
	GPIO_PinModeSet (PE_CLK_PORT,  PE_CLK_PIN,  GPIO_OUTPUT_MODE, 0);			// (CP) SCK - IO1
	GPIO_PinModeSet (PE_STR_PORT,  PE_STR_PIN,  GPIO_OUTPUT_MODE, 0);			// (STR) CS1 - ADC1

//	ext_ctrl (PE_RESET_ALL, 0);
}


void SwitchOn()
{
  
  ext_ctrl (PE_RELAY_ON, PE_OFF);
 ext_ctrl (PE_RELAY_ON, PE_ON);
DelayNOP (500000);
ext_ctrl (PE_RELAY_ON, PE_OFF);

}
void SwitchOff()
{
 GPIO_PinModeSet (ROFF_PORT,  ROFF_BIT, gpioModePushPull, 1);			
  RELAY_OFF_SET;
  DelayNOP (500000);
  RELAY_OFF_CLEAR;
  GPIO_PinModeSet (ROFF_PORT,  ROFF_BIT, gpioModeDisabled, 0);		
}
