#include "display.h"

#include "u8g.h"
#include "u8g_arm.h"

#include "meterage.h"
//#include "axem.h"
#include "modem.h"
//#include "cover_magnet.h"

#include <time.h>
#include <stdio.h>
//#include "pulsecalibration.h"
#include "archive.h"
#include "defines_HW.h"
//#include "adders.h"
#include "dti.h"
//#include "tarif.h"
#include "tariff.h"
#include "triggersevents.h"
#include "electro_v5.h"
#include "em_wdog.h"

#include "meter.h"
#include "modem.h"
#include "sensors_external.h"
#include "defines_HW.h"

#define RADIAN          (PI/180.0f)
#define T_DELAY         ( (360.0f * 50.0f) / 223000.0f )
#define AMPER_K         ( 2.13f )

#define energy_show(a) ((float)a) * meter->K / 1000.0

bool Display_Mode = DISLAY_MODE_AUTO;
uint8_t display_mode_timer = 0;

uint64_t Auto_Disp_Pattern = DEFAULT_AUTO_DISPLAY;
uint8_t Auto_Disp_pos = 0;
uint32_t display_auto_timer = 0;

#define TARIF_SCR       27

static const unsigned char manual_menu[] = {
0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, EOL,
23, 24, EOL,
25, 26, EOL,
16, 17, 18, 19, 20, 21, 22, EOL,
28, 29, 30, 31, 32, 33, 34, EOL,
35, 36, 37, 38, 39, 40, 41, EOL,
42, 43, 44, 45, 46, 47, 48, EOL,
TARIF_SCR,EOL,
EOS
}; 

uint8_t Man_Disp_pos = 0;
uint8_t Man_Disp_subpos = 0;

uint8_t Display_tarif_zone = 0;

static bool DisplayStatus;
static u8g_t u8g;

const char * tariff_names[] = {"—ÛÏÏ‡", "T1", "T2", "T3", "T4"};
#ifdef DD_ADE7868A
const char * phase_names[] = {"—ÛÏ.‘‡Á", "‘1", "‘2", "‘3", "‘4"};
#else
const char * phase_names[] = {"", "", "", "", ""};
#endif

void Display_Set_Mode(bool mode)
{
  Display_Mode = mode;
  if(mode == DISLAY_MODE_MANUAL) display_mode_timer = DISPLAY_MODE_TIMEOUT;
}


void Display_Auto_Main()
{
  if(display_mode_timer)
  {
    if(--display_mode_timer == 0) 
    {
      Display_Mode = DISLAY_MODE_AUTO;
      Auto_Disp_pos = 0;
      Man_Disp_pos = 0;
      Man_Disp_subpos = 0;
      
    }
  }

  if( !Display_IsOn() ) return;    
  if((!(display_auto_timer++%DISPLAY_SHOW_TIME)) && (Display_Mode == DISLAY_MODE_AUTO))
  {
      Display_Show_Auto_Next();
  }
  else Display_Draw(DRAW_SAME_SCREEN_CMD);

}


void Display_Show_Auto_Next()
{
    if( !DisplayStatus ) return;
    do 
    {
      Auto_Disp_pos++;
    }
    while(!((1L << (Auto_Disp_pos%56)) & Auto_Disp_Pattern ) || !Display_Draw(Auto_Disp_pos%56));  

}

uint8_t Display_Get_Menu_Item(uint8_t menu, uint8_t submenu)
{
  uint8_t m, s;
  uint16_t ptr = 0;
  
  m = 0;
  while(manual_menu[ptr] != EOS)
  {
    s = 0;
    while(manual_menu[ptr] != EOL)
    {
      if((m == menu) && (s == submenu)) return manual_menu[ptr];
      s++;
      ptr++;
    }
    m++;
    ptr++;
  }
  return NO_MENU_ITEM;
  
}

void Display_Show_Manual(uint8_t key)
{
  uint8_t scr;
  bool no_tarif = 0;
  switch(key)
  {
  case BLUE_BUTTON_CLICK:
  case RED_BUTTON_CLICK:
    do
    {
      if((key == BLUE_BUTTON_CLICK)&&!no_tarif)  {Man_Disp_pos++; Man_Disp_subpos = 0;}
      else  Man_Disp_subpos++;
      no_tarif = 1;
      if((scr = Display_Get_Menu_Item(Man_Disp_pos, Man_Disp_subpos)) == NO_MENU_ITEM)
      {
        if(key == BLUE_BUTTON_CLICK)  Man_Disp_pos = 0;
        else Man_Disp_subpos = 0;
        scr = Display_Get_Menu_Item(Man_Disp_pos, Man_Disp_subpos);
      }
      if((scr == TARIF_SCR)&&(key == RED_BUTTON_CLICK))
      {
//        if(++Display_tarif_zone == Archive_GetTarifSettings()->TarifZoneTotal) Display_tarif_zone = 0;
        if(++Display_tarif_zone >= TARIFF_ZONE_MAX) Display_tarif_zone = 0;
      }
      else Display_tarif_zone = 0;
    }
    while(!Display_Draw(scr));
    
    if( !CheckPowerLine() ) Schedule_PutUniqe( DISPLAY_OFF_EVENT, __time32(0) + 15 );
  break;
    
  case BLUE_BUTTON_LONG: 
//     if (!AXSEM_packets_to_send) {
//        MeterageHourlyDelta();
//     }
     mQueue_PutMsg( SEND_CURR_PARAM_EVENT, 0 ); 

    break;
  case RED_BUTTON_LONG:
     mQueue_PutMsg(STRIJ_LED_SHORT_ON_EVENT, 0);
     if (DL_Mode != SLIP_FASTDL) {
         mQueue_PutMsg( SET_FAST_DL, 0 );
         Schedule_PutUniqe( RESET_FAST_DL, __time32(0) + 180);
     } else {
         mQueue_PutMsg( RESET_FAST_DL, 0 );
     }
     break;
  default:
    break;
  }
}

uint32_t iamp = 0;
bool Display_Draw( uint8_t Screen )
{
    static uint8_t last_screen = 0;
    char          x, y, text[20];
    time_t        time;
    uint8_t phase = 0;
    uint8_t tariff = 0;
  
    if(Screen == DRAW_SAME_SCREEN_CMD ) Screen = last_screen;
    else last_screen = Screen;
    uint8_t tarif_num = 0;
    for (tariff = 1; tariff < TARIFF_MAX; tariff++) {
      if (TARIFF_is_use(tariff)) tarif_num++;
    }

    switch( Screen ) {
    case 0:
    case 4:
    case 8: 
    case 12: {
        tariff = 1;
        if (TARIFF_is_use(tariff) && tarif_num > 1) break;
        return false; 
    }
    case 1:
    case 5:
    case 9:        
    case 13: {
        tariff = 2;
        if (TARIFF_is_use(tariff) && tarif_num > 1) break;
        return false; 
    }
    case 2:
    case 6:
    case 10:        
    case 14: {
        tariff = 3;
        if (TARIFF_is_use(tariff) && tarif_num > 1) break;
        return false; 
    }
    case 3: 
    case 7:
    case 11:
    case 15: {
        tariff = 0;
        break;
    }
    
    case 16: // P‡ÍÚ.
    case 17: // PÂaÍ.
    case 18: // PÔÓÎÌ.
    case 19: // U
    case 20: // I
    case 21: //  ÏÓ˘Ì.
    case 22: {// ◊‡ÒÚ.
        phase = 0;
        break;
    }
    case 28: // P‡ÍÚ.
    case 29: // PÂaÍ.
    case 30: // PÔÓÎÌ.
    case 31: // U
    case 32: // I
    case 33: //  ÏÓ˘Ì.
    case 34: {// ◊‡ÒÚ.
#ifdef DD_ADE7868A
        phase = 1;
        break;
#else
        return false;
#endif
    }
    case 35: // P‡ÍÚ.
    case 36: // PÂaÍ.
    case 37: // PÔÓÎÌ.
    case 38: // U
    case 39: // I
    case 40: //  ÏÓ˘Ì.
    case 41: {// ◊‡ÒÚ.
#ifdef DD_ADE7868A
        phase = 2;
        break;
#else
        return false;
#endif
    }
    case 42: // P‡ÍÚ.
    case 43: // PÂaÍ.
    case 44: // PÔÓÎÌ.
    case 45: // U
    case 46: // I
    case 47: //  ÏÓ˘Ì.
    case 48: {// ◊‡ÒÚ.
#ifdef DD_ADE7868A
        phase = 3;
        break;
#else
        return false;
#endif
    }
    }
  
  //if(Adder) Adder->Apparent = sqrt(Adder->Active_P*Adder->Active_P + Adder->Reactive_P*Adder->Reactive_P + Adder->Reactive_N*Adder->Reactive_N);
  
  u8g_FirstPage(&u8g);

  do {
    switch( Screen ) {
      case 0:
      case 1:
      case 2:
      case 3:
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", tariff_names[tariff] );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        snprintf( text, sizeof(text), "%s", "Í¬Ú◊" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );

        u8g_SetFont( &u8g,u8g_font_profont29r );
        snprintf( text, sizeof(text), "%.1f", energy_show(meter->data->itgr[tariff][phase].P_p) );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 64, text );
        break;

    case 4:
    case 5:
    case 6:
    case 7:  
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", tariff_names[tariff] );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        snprintf( text, sizeof(text), "%s", "+Í¬‡◊" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );

        u8g_SetFont( &u8g,u8g_font_profont29r );
        snprintf( text, sizeof(text), "%.1f", energy_show(meter->data->itgr[tariff][phase].Q_p) );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 64, text );
        break;
    case 8:
    case 9:
    case 10:
    case 11:  
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", tariff_names[tariff] );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        snprintf( text, sizeof(text), "%s", "-Í¬‡◊" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );

        u8g_SetFont( &u8g,u8g_font_profont29r );
        snprintf( text, sizeof(text), "%.1f", energy_show(meter->data->itgr[tariff][phase].Q_n) );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 64, text );
        break;
    case 12:
    case 13:
    case 14:
    case 15:  
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", tariff_names[tariff] );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        snprintf( text, sizeof(text), "%s", "Í¬A◊" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );

        u8g_SetFont( &u8g,u8g_font_profont29r );
        snprintf( text, sizeof(text), "%.1f", energy_show(meter->data->itgr[tariff][phase].S) );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 64, text );      
        break;
           
      case 16:
      case 28:
      case 35:
      case 42:
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "P‡ÍÚ." );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        snprintf( text, sizeof(text), "%s", phase_names[phase] );
        u8g_DrawStr( &u8g, 0, 35, text );
        
        snprintf( text, sizeof(text), "%s", "¬Ú" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );

        u8g_SetFont( &u8g, u8g_font_profont29r );
        snprintf( text, sizeof(text), "%.0f", meter->data->curr.to_display[phase].Pi );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 64, text );
        break;
      case 17:
      case 29:
      case 36:
      case 43:
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "PÂaÍ." );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        snprintf( text, sizeof(text), "%s", phase_names[phase] );
        u8g_DrawStr( &u8g, 0, 35, text );
        
        snprintf( text, sizeof(text), "%s", "¬‡" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );

        u8g_SetFont( &u8g, u8g_font_profont29r );
        snprintf( text, sizeof(text), "%.0f", meter->data->curr.to_display[phase].Qi );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 64, text );
        break;
      case 18:
      case 30:
      case 37:
      case 44:
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "PÔÓÎÌ." );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        snprintf( text, sizeof(text), "%s", phase_names[phase] );
        u8g_DrawStr( &u8g, 0, 35, text );
        
        snprintf( text, sizeof(text), "%s", "¬¿" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );

        u8g_SetFont( &u8g, u8g_font_profont29r );
        snprintf( text, sizeof(text), "%.0f", sqrt(meter->data->curr.to_display[phase].Pi*meter->data->curr.to_display[phase].Pi+
                                                   meter->data->curr.to_display[phase].Qi*meter->data->curr.to_display[phase].Qi) );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 64, text );
        break;
      case 19: 
      case 31:
      case 38:
      case 45:
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "U" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        snprintf( text, sizeof(text), "%s", phase_names[phase] );
        u8g_DrawStr( &u8g, 0, 35, text );
        
        snprintf( text, sizeof(text), "%s", "¬" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );

        u8g_SetFont( &u8g, u8g_font_profont29r );
        snprintf( text, sizeof(text), "%.1f", meter->data->curr.phase[phase].U );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 64, text );
        break;
      case 20:
      case 32:
      case 39:
      case 46:
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "I" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        snprintf( text, sizeof(text), "%s", phase_names[phase] );
        u8g_DrawStr( &u8g, 0, 35, text );
        
        //u8g_SetFont( &u8g,u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "A" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );

        u8g_SetFont( &u8g, u8g_font_profont29r );
        snprintf( text, sizeof(text), "%.2f", meter->data->curr.phase[phase].I );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 64, text );
        break;
      case 21:
      case 33:
      case 40:
      case 47:
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", " ÏÓ˘Ì." );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        snprintf( text, sizeof(text), "%s", phase_names[phase] );
        u8g_DrawStr( &u8g, 0, 35, text );
        
        /*8g_SetFont( &u8g,u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "A" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );*/

        u8g_SetFont( &u8g, u8g_font_profont29r );
        snprintf( text, sizeof(text), "%.3f", cos(meter->data->curr.phase[phase].cosFi/180.0*3.1415) );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 64, text );
        break;
      case 22:
      case 34:
      case 41:
      case 48:
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "◊‡ÒÚ." );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        snprintf( text, sizeof(text), "%s", phase_names[phase] );
        u8g_DrawStr( &u8g, 0, 35, text );
        
        //u8g_SetFont( &u8g,u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "√ˆ" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );

        u8g_SetFont( &u8g, u8g_font_profont29r );
        snprintf( text, sizeof(text), "%.2f", meter->data->curr.phase[phase].F );  
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 64, text );
        break;     
      case 23:
        u8g_SetFont( &u8g,u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "¬ÂÏˇ " );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        
        time = __time32(0);
        u8g_SetFont( &u8g,u8g_font_profont29r );
        snprintf( text, sizeof(text), "%02d:%02d:%02d", localtime(&time)->tm_hour, localtime(&time)->tm_min, localtime(&time)->tm_sec );
        u8g_DrawStr( &u8g, 0, 64, text );
        break;
      case 24:
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "ƒ‡Ú‡" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        
     
        time = __time32(0);
        u8g_SetFont( &u8g, u8g_font_profont29r );
        snprintf( text, sizeof(text), "%02d.%02d.%02d", localtime(&time)->tm_mday, localtime(&time)->tm_mon+1, (1900+localtime(&time)->tm_year)-2000 );
        u8g_DrawStr( &u8g, 0, 64, text );
        break;
      case 25:
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "¿‰ÂÒ" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        

        u8g_DrawStr( &u8g, 0, 37, "RS-485" );
        snprintf( text, sizeof(text), "%d", Settings.DTI_address);
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 37, text );  
        
        u8g_DrawStr( &u8g, 0, 59, "Strij" );
        snprintf( text, sizeof(text), "%x", Settings.ProtC_ID);
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 60, text );
        break;
      case 26: {
        u8g_SetFont( &u8g, u8g_font_profont22r );
        snprintf( text, sizeof(text), "%s", "¬Â.œŒ" );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text);        

        u8g_DrawStr( &u8g, 0, 37, "Soft.rev." );
        float sr = (float)DTI_info.software_rev + (float)DTI_info.software_subrev/100;
        snprintf( text, sizeof(text), "%0.2f", sr);
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 37, text );  
        
        u8g_DrawStr( &u8g, 0, 59, "Hard.rev." );
        float hr = (float)DTI_info.hardware_rev/100;
        snprintf( text, sizeof(text), "%0.2f", hr);
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 60, text );
        break;
      }
    case TARIF_SCR: {
        uint8_t tariff = meter->tariff_conv(__time32(0));
        if (tariff >= TARIFF_MAX) return false;
        if (!tariff) tariff++;

        u8g_SetFont( &u8g, u8g_font_profont22r );
        
        snprintf( text, sizeof(text), "“ÂÍ.%s", tariff_names[tariff] );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 15, text );        
        snprintf( text, sizeof(text), "%s «ÓÌ‡ %d", tariff_names[TARIFF_from_zone(Display_tarif_zone)], Display_tarif_zone + 1 );
        u8g_DrawStr( &u8g, 128-u8g_GetStrWidth(&u8g, text), 35, text );

        u8g_SetFont( &u8g,u8g_font_profont29r );
        
//        snprintf( text, sizeof(text), "%02d:%02d:00",pTarif[Display_tarif_zone].time_zone.start/3600, (pTarif[Display_tarif_zone].time_zone.start%3600)/60 );
        snprintf( text, sizeof(text), "%02d:%02d:00", Display_tarif_zone / 2, (Display_tarif_zone % 2) * 30 );
        u8g_DrawStr( &u8g, 0, 64, text );
        break;
    }
    default: return false;
    }
    //Draw all status
    x = y = 0;
    
    if (Settings.Flash_error) {
        u8g_DrawXBM( &u8g, x, y, flash_error_width, flash_error_height, flash_error_bits );
        x += ( flash_error_width + 2 );
    }
     if( sensors_ext_curr.state_cover == 1 ) {
        u8g_DrawXBM( &u8g, x, y, unlock_width, unlock_height, unlock_bits );
        x += ( unlock_width + 2 );
     }
     
     if( sensors_ext_curr.state_hall == 1 ) {
        u8g_DrawXBM( &u8g, x, y, magnet_width, magnet_height, magnet_bits );
        x += ( magnet_width + 2 );
     }
     
     if(DL_Mode == SLIP_FASTDL) {
        u8g_DrawXBM( &u8g, x, y, wrench_width, wrench_height, wrench_bits );
        x += ( wrench_width+ 2 );
     }
     
     if( Modem_IsSending() ) {
        u8g_DrawXBM( &u8g, x, y, wifi_width, wifi_height, wifi_bits );
        x += ( wifi_width + 2 );
     }

     if( Modem_IsWaitingAck() ) {
        u8g_DrawXBM( &u8g, x, y, back_width, back_height, back_bits );
        x += ( back_width + 2 );
     }
     
     switch ( GetPowerLimitSettings() ) {
      case  PWR_LIMIT_ON:
        u8g_DrawXBM( &u8g, x, y, limit_width, limit_height, limit_bits );
        x += ( limit_width + 2 );
        break;
     case  POWER_OFF:
        u8g_DrawXBM( &u8g, x, y, exclamation_width, exclamation_height, exclamation_bits );
        x += ( limit_width + 2 );
        break;
     }
     if( !GetRelayStatus() ) {
        u8g_DrawXBM( &u8g, x, y, off_width, off_height, off_bits );
        x += ( off_width + 2 );
     }
  } while( u8g_NextPage( &u8g ) );
  return true;
}


void Display_Init( void )
{

  DisplayStatus = false;

}


bool Display_IsOn( void )
{
  return DisplayStatus;
}

//void Display_ReleaseCom(void)
//{
//  if( DisplayStatus )
//    u8g_ReleaseCom(&u8g, &u8g_dev_st7565_lm6059_2x_sw_spi);
//}
//
//void Display_RestoreCom(void)
//{
//  if( DisplayStatus )
//    u8g_RestoreCom(&u8g, &u8g_dev_st7565_lm6059_2x_sw_spi);
//}

void Display_SetOn(void)
{
  if( !DisplayStatus ) {
    DisplayStatus = true;    
    u8g_InitComFn(&u8g, &u8g_dev_st7565_lm6059_2x_sw_spi, u8g_com_sw_spi_fn);
    Auto_Disp_pos = -1;
    display_auto_timer = 1;

  }
}
void Display_SetOff(void)
{
  if(DisplayStatus) {
    DisplayStatus = false;    
    u8g_StopCom(&u8g, &u8g_dev_st7565_lm6059_2x_sw_spi);

  }
}

void black_screen(uint32_t delaY)
{
    if(!DisplayStatus) return;
    u8g_FirstPage( &u8g );
    do
    {
      u8g_DrawBox(&u8g, 0, 0, 128, 64);
    }while( u8g_NextPage(&u8g) );

    while(delaY--) WDOG_Feed();
}
