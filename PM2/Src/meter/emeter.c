#include "em_gpio.h"
#include "emeter.h"
#include "em_cmu.h"
#include "em_rmu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include "u8g.h"
#include <stdio.h>
#include <time.h>
#include "triggersevents.h"

#include "main.h"
#include "ade7953.h"
#include "archive.h"


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

#define SCREEN_TIME 1000
#define SEND_DELAY 27000
//#define BUTTONS_CHECK

//struct tm *tm_local;
//time_t unix_time=1442466000+TIMEFIX;  //Sun, 20 Sep 2015 05:00:00 GMT
extern u8g_t u8g;
extern uint32_t AdcData;
uint32_t PwLimit=0xFFFFFFFF;
uint32_t PwCur=0;

uint8_t RSSI,curRSSI,SNR;
uint8_t packetRec[128], reply;

uint32_t INP1_CNT, INP2_CNT;
const char * tarif_names[] = {"Сумма", "Пик", "Полупик", "Ночь"};
const char * measure_names[] = {"Всего", "Сутки", "Неделя", "Месяц", "Год"};
uint8_t tarif_cur=1;
uint32_t PwCons[TARIF_NUM+1][MEASURES]; //day week month year summ
uint32_t ButtonPressed;

// TEMP
uint32_t rms_v, rms_i, v_peak, accmode;
int32_t e_active, e_reactive, e_apparent, inst_active, inst_reactive, inst_apparent;
float pfa;
float line_freq;
// TEMP
extern uint32_t pulsecounter;

void taskDisplay (void *pParameters)
{
  pParameters = pParameters;   /* to quiet warnings */
  TickType_t xTicksToWait = SCREEN_TIME/portTICK_RATE_MS;
  TimeOut_t xTimeOut;
  char text[128];
  int trf;
  
  pulsecounter = 0;
  
  for (;;)
  {
    for (trf=0;trf<4;trf++)
    {
      u8g_FirstPage(&u8g);
      do
      {
        u8g_SetFont(&u8g,u8g_font_unifont_0_8);
        u8g_SetFontPosTop(&u8g);
       // snprintf (text, sizeof(text), "%s", ADE_GetArchive()->t[trf] .name);
        u8g_DrawStr(&u8g, 0, 0, text);
       // snprintf (text, sizeof(text),  "A+ %08.0f Втч", ADE_GetArchive()->t[trf].A_p[0]/1000.0);
        u8g_DrawStr(&u8g, 0, 12, text);
       // snprintf (text, sizeof(text),  "A- %08.0f Втч", ADE_GetArchive()->t[trf].A_n[0]/1000.0);
        u8g_DrawStr(&u8g, 0, 24, text);
       // snprintf (text, sizeof(text),  "R+ %08.0f Варч", ADE_GetArchive()->t[trf].R_p[0]/1000.0);
        u8g_DrawStr(&u8g, 0, 36, text);
       // snprintf (text, sizeof(text),  "R- %08.0f Варч", ADE_GetArchive()->t[trf].R_n[0]/1000.0);
        u8g_DrawStr(&u8g, 0, 48, text);
      }while(u8g_NextPage(&u8g));
      
      vTaskSetTimeOutState( &xTimeOut );
      xTicksToWait = 4000/portTICK_RATE_MS;
      while(1)
      {
        if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )	break;
        if (Button1GetEvent() == pdTRUE)	break;
      }
    }
    
    //#if defined(DEBUG_PAGE) || defined(PW_CALIBRATE)
    //		for (i=0;i<20*2;i++)
    //		{
    //			lcd_clear(1);
    //			sprintf (text, "em strij debug");
    //			display_text(text, 1,0);
    //			sprintf (text, (char*)((const char*)"%02u:%02u:%02u"), tm_local->tm_hour, tm_local->tm_min, tm_local->tm_sec);
    //			display_text(text, 1,1);
    //			sprintf (text, "%02u.%02u.%u", tm_local->tm_mday, tm_local->tm_mon+1, tm_local->tm_year+1900);	// ДД.ММ.ГГГГ
    //			display_text(text, 1,2);
    //			sprintf (text, "R=%d:%02x%02x%02x%02x%02x%02x%02x%02x", reply, \
    //				packetRec[0], packetRec[1], packetRec[2], packetRec[3], \
    //					packetRec[4], packetRec[5], packetRec[6], packetRec[7] );	// ДД.ММ.ГГГГ
    //			display_text(text, 1,3);
    //			sprintf (text, "INP1 = %d", INP1_CNT);	// ДД.ММ.ГГГГ
    //			display_text(text, 1,4);
    //			sprintf (text, "INP2 = %d kP=%.3f", INP2_CNT,INP1_CNT>0?(float)INP1_CNT/INP2_CNT:0.0);	// ДД.ММ.ГГГГ
    //			display_text(text, 1,5);
    //			sprintf (text, "strijtime %06x", (unix_time-TIMEFIX)/300);	// ДД.ММ.ГГГГ
    //			display_text(text, 1,6);
    //			sprintf (text, "Button = %d", ButtonPressed);	// ДД.ММ.ГГГГ
    //			display_text(text, 1,7);
    //			vTaskSetTimeOutState( &xTimeOut );
    //			xTicksToWait = SCREEN_TIME/(20)/portTICK_RATE_MS;
    //			while(1)
    //			{
    //				if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
    //					break;
    //			}
    //		}
    //#endif
    
    //#ifndef PW_CALIBRATE
    //
    //		for (trf=0;trf<=4;trf++)
    //		{
    //
    //			for (i=1;i<4;i++)
    //			{
    //				lcd_clear(1);
    //
    //				u8g_SetFont(&u8g,u8g_font_unifont_0_8);
    //				u8g_SetFontPosTop(&u8g);
    //
    //				sprintf(text,"T%d %s",trf, tarif_names[trf]);
    //				u8g_DrawStr(&u8g,0, 0, text);
    //				sprintf(text,"%-7s: %d.%02d",measure_names[i], PwCons[trf][i]/1000,(PwCons[trf][i]%1000)/10);
    //
    //				u8g_DrawStr(&u8g,0, 17, text);
    //
    //				u8g_SetFont(&u8g,u8g_font_fur20);
    //				u8g_SetFontPosBottom(&u8g);
    //				sprintf(text,"%d.%02d",PwCons[trf][i]/1000,(PwCons[trf][i]%1000)/10);
    //				u8g_DrawStr(&u8g,0, 63,text);
    //				vTaskSetTimeOutState( &xTimeOut );
    //				xTicksToWait = SCREEN_TIME/(MEASURES-1)/portTICK_RATE_MS;
    //				while(1)
    //				{
    //					if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
    //						break;
    //#ifdef BUTTONS_CHECK
    //					if (ButtonPressed==1)
    //					{vTaskDelay(50/portTICK_RATE_MS);
    //					break;
    //					}
    //#endif
    //				}
    //#endif
    //			}
    //		}
    
    //#ifdef SHOW_TIME
    //		for (i=0;i<SCREEN_TIME/1000;i++)
    //		{
    //			lcd_clear(1);
    //			u8g_SetFont(&u8g,u8g_font_unifont_0_8);
    //			u8g_SetFontPosTop(&u8g);
    //			sprintf(text,"Дата\\время");
    //			u8g_DrawStr(&u8g,0, 0, text);
    //
    //			u8g_SetFont(&u8g,u8g_font_fur17);
    //			u8g_SetFontPosBottom(&u8g);
    //			sprintf(text,"%d.%02d",PwCons[trf][i]/1000,(PwCons[trf][i]%1000)/10);
    //
    //			sprintf (text, (char*)((const char*)"%02u:%02u:%02u"), tm_local->tm_hour, tm_local->tm_min, tm_local->tm_sec);
    //			u8g_DrawStr(&u8g,0, 40, text);
    //			sprintf (text, "%02u.%02u.%u", tm_local->tm_mday, tm_local->tm_mon+1, tm_local->tm_year+1900);	// ДД.ММ.ГГГГ
    //			u8g_DrawStr(&u8g,0, 63, text);
    //			vTaskSetTimeOutState( &xTimeOut );
    //			xTicksToWait = 1000/portTICK_RATE_MS;
    //			while(1)
    //			{
    //				if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
    //					break;
    //#ifdef BUTTONS_CHECK
    //				if (ButtonPressed==1)
    //				{vTaskDelay(50/portTICK_RATE_MS);
    //				break;
    //				}
    //#endif
    //			}
    //		}
    //#endif
    //		lcd_clear(1);
    
    
    //		for (i=0;i<SCREEN_TIME/1000;i++)
    //		{
    //			lcd_clear(1);
    //			u8g_SetFont(&u8g,u8g_font_unifont_0_8);
    //			u8g_SetFontPosTop(&u8g);
    //			sprintf(text,"Мощность");
    //			u8g_DrawStr(&u8g,0, 0, text);
    //			if (PwLimit!=0xFFFFFFFF)
    //				sprintf(text,"Ограничение %d Вт",PwLimit);
    //			else
    //				sprintf(text,"Ограничения нет");
    //			u8g_DrawStr(&u8g,0, 17, text);
    //			u8g_SetFont(&u8g,u8g_font_fur17);
    //			u8g_SetFontPosBottom(&u8g);
    //			sprintf(text,"%d.%02d",PwCur/100,PwCur%100);
    //
    //			u8g_DrawStr(&u8g,0, 63, text);
    //			vTaskSetTimeOutState( &xTimeOut );
    //			xTicksToWait = 1000/portTICK_RATE_MS;
    //			while(1)
    //			{
    //				if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
    //					break;
    //#ifdef BUTTONS_CHECK
    //				if (ButtonPressed==1)
    //				{vTaskDelay(50/portTICK_RATE_MS);
    //				break;
    //				}
    //#endif
    //			}
    //		}
  }
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