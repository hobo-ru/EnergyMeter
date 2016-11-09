#include "main.h"

#include "defines_HW.h"
#include "archive.h"
#include "mqueue.h"
#include "meterage.h"
#include "scheduler.h"
#include "modem.h"
//#include "tarif.h"
#include "triggersevents.h"
#include "pwr_managment.h"
#include "display.h"

#include <stdlib.h>
#include <em_rmu.h>
#include <em_wdog.h>
//#include "cover_magnet.h"
//#include "event.h"
#include "electro_v5.h"

//#include "pulsecalibration.h"
#include "leuart.h"
#include "dti.h"
#include "systimer.h"
#include "at25sf041.h"
#include "modem_uart.h"
//#include "axem.h"
#include "modem.h"
#include "meter.h"
#include "meter_archive.h"
#include "tariff.h"
#include "sensors_external.h"

//time_t CUR_time;  // delete later


__no_init uint32_t rst_reason;

void RTC_EverySecondEvent()
{
  mQueue_PutMsg(RTC_SECOND_EVENT, 0);
}

void HAL_RTC_Init()
{
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_CORELE, true);
  
  TD_RTC_Init(0);
  TD_RTC_SetUserHandler(RTC_EverySecondEvent);
}

//void PinsDisable()
//{
//  GPIO_PinModeSet( RELAYON.port,   RELAYON.pin,   gpioModePushPull, 0 );
//  GPIO_PinModeSet( RELAYOFF.port,  RELAYOFF.pin,  gpioModePushPull, 0 );
//  GPIO_PinModeSet( ADE_FREQ1.port, ADE_FREQ1.pin, gpioModePushPull, 0 );
//  GPIO_PinModeSet( ADE_RESET.port, ADE_RESET.pin, gpioModePushPull, 0 );
//  GPIO_PinModeSet( ADE_IRQ.port,   ADE_IRQ.pin,   gpioModePushPull, 0 );
//  GPIO_PinModeSet( ADE_CS.port,    ADE_CS.pin,    gpioModePushPull, 0 );
//  GPIO_PinModeSet( ADE_SCK.port,   ADE_SCK.pin,   gpioModePushPull, 0 );
//  GPIO_PinModeSet( ADE_SDI.port,   ADE_SDI.pin,   gpioModePushPull, 0 );
//  GPIO_PinModeSet( ADE_SDO.port,   ADE_SDO.pin,   gpioModePushPull, 0 );
//}


void setTime(uint32_t time)
{
   ARCH_emit_event(ME_update_rts_old, 0);
   Schedule_CorrectTime( time );
   //Meterage_CorrectTime( time );
   TD_RTC_SetTime( time );
   Scheduler_Start();
   ARCH_emit_event(ME_update_rts, 0);
//   Event_Send( TIME_CORRECTION_ARCH_EVENT, __time32(0) );
}

//uint32_t while_counter = 0;
//uint8_t test_send_sleep = 0;
uint8_t off_axsem = 0;

void OPTO_TEST(void) 
{
    OptoUART_PutTx(0xA5);
}

int main( void )
{
  mQueueMsg_t recv_msg;
//  bool IsArchive;
  rst_reason = RMU_ResetCauseGet();
  CHIP_Init();
  HAL_RTC_Init();
  
//  AxemReset();

  ADE_alfa = 0;
  
  //Triggers_Init();
//#ifdef VER_3_0
//  GPIO_PinModeSet( AXSEM_RESET.port, AXSEM_RESET.pin, gpioModePushPull, 0 );
//  uint64_t t = 10000;
//  while(--t);
//  GPIO_PinModeSet( AXSEM_RESET.port, AXSEM_RESET.pin, gpioModeInputPull, 1 );
//#endif  

  CMU_ClockEnable( cmuClock_GPIO, true );
//  gpio_SET_EXT_PWR;
//  GPIO_PinModeSet( EXT_PWR.port, EXT_PWR.pin, gpioModeInputPull, 0 );
//  GPIO_PinModeSet( EXT_PWR.port, EXT_PWR.pin, gpioModeInputPull, 0 );

  gpio_INI_FLASH_POW;
  mQueue_Init();
  Scheduler_Init();
  PwrManager_Init();
//  SetDeepPwrSleep();
  InitSystemTimer();
  SENSORS_EXT_init();
  
//  AxemReset();
  ModemUART_Init();
  OptoUART_Init();
//  Modem_Init( CheckPowerLine() );
  ModemUART_SLIP_Send(AXEM_CMD_ECHO, 0, 0);
                      
  METER_init();
  meter->K = 1.0 / 2000.0;
//  meter->conv_energ = ADE_data;
  meter->tariff_conv = TARIFF_from_time;// Tarif_timestamp;
  meter->event_arch_data = METER_event_arch_data;
  meter->event_arch = ARCH_emit_event_ext;
//  ARCH_init();
//  arch->save = ARCH_save_rec;
//  arch->load = ARCH_load_rec;
  ARCH_update_indexs();
  DTI_regProperty("pM_INTEGS", STRUCT_TYPE, (uint32_t)meter->data->itgr, 0);
  DTI_regProperty("pM_CURR", STRUCT_TYPE, (uint32_t)&meter->data->curr, 0);
  DTI_regProperty("pM_PPOWST", STRUCT_TYPE, (uint32_t)&meter->data->aDataSet[1], 0);

  TARIFF_init();
  Archive_Init();
  //Tarif_Init();  
//  Meterage_Init( IsArchive );

  Triggers_Init();
  Display_Init();

  WDOG_Init_TypeDef wdog = WDOG_INIT_DEFAULT;
  wdog.perSel = wdogPeriod_4k;
  WDOG_Init( &wdog );
  WDOG_Enable( true );
  AXSEM_Reset();
  NOW = __time32(0);
  ARCH_emit_event(ME_power_on, 0xFFFF);
//  Event_Send( PROGRAMM_START_ARCH_EVENT, __time32(0));
  Dispatcher_PowerLine();
  mQueue_PutMsg( PWR_ONLINE_EVENT, 0 );
 
  LeUART_Init();
  DTI_Init();
  //SysTimer_SetCallback(&DTI_Poll, 10);
  //SysTimer_SetCallback(&SENSORS_EXT_Dispatcher, 10);
  //SysTimer_SetCallback(&OPTO_TEST, 1);

  //register set/get current time properties
  DTI_regProperty("CUR_time", TIME_TYPE, (uint32_t)&NOW, 0);
  DTI_regProperty("setTime", FUNCTION_TYPE, (uint32_t)setTime, 0);
  
  DTI_regProperty("pSENS_cur", STRUCT_TYPE, (uint32_t)&sensors_ext_curr, 0);
//  static time_t hold_time = 0, hold_time_blue = 0;

//  bool old_power_state = CheckPowerLine();
  gpio_OFF_SWIO;
  
  while(1)  {
//      while_counter++;
    while( mQueue_GetMsg( &recv_msg ) ) {
        switch( recv_msg.type ) {
        case RTC_SECOND_EVENT: {
            NOW = __time32(0);
          
            Dispatcher_PowerLine();
            DTI_Timer_Dispatcher();

            if (CheckPowerLine() && PwrManager_GetStatus()) {
                ADE_ReadReg();
                if (!ADE_alfa) {
                    METER_upd(NOW);
                }
                Meterage_ADE(NOW);
            }
            if (CheckPowerLine()) {
                if (AXSEM_ACK_timeout >= 10) {
                    AXSEM_ACK_timeout = 0;
                    AXSEM_Reset();
                    ModemUART_SLIP_Send(AXEM_CMD_SET_MODE, &DL_Mode, 1);
                }
                AXSEM_ACK_timeout++;
                AXSEM_ACK_timeout = 1;
//                ModemState = MODEM_WAIT_ACK;
                ModemUART_SLIP_Send(AXEM_CMD_SET_MODE, 0, 0); // read DL_Mode NBFi
                if (!Settings.Calibration_completed) {
                    if (DL_Mode != SLIP_FASTDL) {
                        DL_Mode = SLIP_FASTDL;
                        ModemUART_SLIP_Send(AXEM_CMD_SET_MODE, &DL_Mode, 1);
                    }
                }

//                test_send_sleep = 1;
                
//                Archive_SaveSettings(); // test
//                if ((NOW % 3) == 0) { // test
//                    static uint8_t rele_test = 0;
//                    rele_test = 1 - rele_test;
//                    if (rele_test) {
//                        SwitchRelayOn();
//                    } else {
//                        SwitchRelayOff();
//                    }
//                }
                Archive_Send_Dispatcher();
                if (ADE_alfa) {
                    mQueue_PutMsg(STRIJ_LED_ON_EVENT,0);
                }
            } else {
//                if (test_send_sleep == 1 || test_send_sleep == 2) {
//                    test_send_sleep = 2;
//                    ModemUART_SLIP_Send(AXEM_CMD_GET_PACKETS_TO_SEND, 0, 0);
//                }
            }
//          if(CheckPowerLine() && !ADE_was_calibrated&&!(NOW%10))
//          {
//             mQueue_PutMsg( SET_FAST_DL, 0 );
//             access_timer = ACCESS_TIMEOUT;
//             //if((Settings.ProtC_ID != 0) &&(Settings.ProtC_ID != -1)) Settings.DTI_address = Settings.ProtC_ID&0xff;
//          }
          
          Display_Auto_Main();

          mQueue_PutMsg( SCHEDULE_EVENT, 0 );
          mQueue_PutMsg( WATCH_DOG_EVENT, 0 );

          if (TestButtonLong(&btn_red, btn_blue.state)) {
              Display_Show_Manual(RED_BUTTON_LONG);
          }
          if (TestButtonLong(&btn_blue, btn_red.state)) {
              Display_Show_Manual(BLUE_BUTTON_LONG);
          }
          
          if (CheckPowerLine() && (NOW % 10) == 0) {
              RepeatRelaySwitch(0);
              if ((FLASH_ReadStatus_1()&0x80) || (FLASH_ReadStatus_1()&1)) {
                  Settings.Flash_error = 1;
              } else {
                  Settings.Flash_error = 0;
              }
          }
//          if (DL_Mode == SLIP_FASTDL) { // ddos
//              uint8_t drbuffer[50];
//              memset(drbuffer, 0, sizeof(drbuffer));
//              ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, drbuffer, sizeof(drbuffer));
//          }

//          if(CheckPowerLine() && (hold_time) &&(__time32(0) - hold_time > 3)&&GPIO_PinInGet( BUTTON1.port, BUTTON1.pin ) == 0) {
//          if(CheckPowerLine() && (hold_time) &&(__time32(0) - hold_time > 3) && !gpio_GET_BUTTON1) {
//          if(CheckPowerLine() && (hold_time) &&(__time32(0) - btn_red.time_event > 3) && btn_red.state) {
////            if(GPIO_PinInGet( BUTTON2.port, BUTTON2.pin ) == 0)
//            if (btn_blue.state) {
//                if(__time32(0) - hold_time > 10)  SCB->AIRCR = 0x05FA0004; 
//                else break;
//            }
//            hold_time = 0;
//            Display_Show_Manual(RED_BUTTON_LONG);           
//          } 
          
//          if(CheckPowerLine() && (hold_time_blue) &&(__time32(0) - hold_time_blue > 3)&&GPIO_PinInGet( BUTTON2.port, BUTTON2.pin ) == 0) {
//          if(CheckPowerLine() && (hold_time_blue) &&(__time32(0) - hold_time_blue > 3) && !gpio_GET_BUTTON2) {
////            if(GPIO_PinInGet( BUTTON1.port, BUTTON1.pin ) == 0) break;
//            if (!gpio_GET_BUTTON1) break;
//            hold_time_blue = 0;
//            Display_Show_Manual(BLUE_BUTTON_LONG);           
//          } 
          
//          AxemEverySecond();
          
          //if( !CheckPowerLine()) 
          SENSORS_EXT_Dispatcher();
          
//          OptoUART_PutTx(0xA5);
          
//          if (!CheckPowerLine() && off_axsem < 3) {
//              off_axsem++;
//              uint8_t DL_Mode_tmp = SLIP_DRX;
//              ModemUART_SLIP_Send(AXEM_CMD_SET_MODE, &DL_Mode_tmp, 1);
//          }
//          if (CheckPowerLine()) {
//              off_axsem = 0; 
//          }

        break;
        }

//        case DAILY_STATISTIC_EVENT:
          //Archive_SaveDaily( __time32(0) );
//          MeterageDailyStatistic();
//          Schedule_PutNextDayStart( DAILY_STATISTIC_EVENT, __time32(0) + 1 );
//          Schedule_PutNextDayRegular( SEND_DAY_REGULAR_EVENT );
//          break;

//        case QUALITY_STATISTIC_EVENT:
          //MeterageQualityStatistic();
//          Schedule_PutUniqe( QUALITY_STATISTIC_EVENT,  GetStartNextHour( __time32(0) ) );
//          break;

//        case LD_PROFILE_STATISTIC_EVENT:
          //MeterageLdProfileStatistic();
//          Schedule_PutUniqe( LD_PROFILE_STATISTIC_EVENT,  GetStartNextHalfHour( __time32(0) ) + 1 );
//          break;

        case SCHEDULE_EVENT:
          Scheduler();
          break;

//        case PARSE_RECV_EVENT:
//          Modem_ExecuteAxemCallback();
//          break;
 
        case TIME_SYNC_EVENT:
          ARCH_emit_event(ME_update_rts_old, 0);
          Schedule_CorrectTime( recv_msg.Data );
          //Meterage_CorrectTime( recv_msg.Data );
          TD_RTC_SetTime( recv_msg.Data );
          Scheduler_Start();
          ARCH_emit_event(ME_update_rts, 0);
          //Event_Send( TIME_CORRECTION_ARCH_EVENT, __time32(0) );
          break;

        case SET_PWR_LIMIT_EVENT:
          SetPowerLimit( (uint32_t)Settings.Power_limit/*recv_msg.Data*/ );
          RepeatRelaySwitch(5);
          mQueue_PutMsg( LIMITATION_EVENT, (uint32_t)Settings.Power_limit/*recv_msg.Data*/);
          break;

        case LIMITATION_EVENT: {
            if( !recv_msg.Data ) {                  // off
                ARCH_emit_event(ME_load_set_new, 0);
//                ARCH_emit_event(ME_over_load, 0);
//                Event_Send( PWR_OFF_ARCH_EVENT, recv_msg.Data );
            } else if( recv_msg.Data == 0xFFFF ) {  // on
                ARCH_emit_event(ME_load_set_new, 0xFFFF);
//                ARCH_emit_event(ME_under_load, 0xFFFF);
//                Event_Send( PWR_LIMITATION_OFF_ARCH_EVENT, recv_msg.Data );
            } else {                                // limit
                ARCH_emit_event(ME_load_set_new, recv_msg.Data & 0xFFFF);
//                Event_Send( PWR_LIMITATION_ON_ARCH_EVENT, recv_msg.Data );
            }
            break;
        }

        case SEND_CURR_PARAM_EVENT:
          if (!CheckPowerLine()) break;
          if (DL_Mode == SLIP_FASTDL) {
              Schedule_PutUniqeEventsScatterRndTime( SEND_CURR_PARAM_EVENT, 60*1, 0, 200 );
              break;
          }
          mQueue_PutMsg(STRIJ_LED_SHORT_ON_EVENT, 0);
          
          { // send 
              uint8_t drbuffer[256];
              uint8_t drbuffer_len;
              drbuffer_len = Electro_MakeNbfiCurr(drbuffer, EL_V5_NBFI_DAILY, NOW);
              ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, drbuffer, drbuffer_len);
//              ModemState = MODEM_SEND_PAYLOAD;
//              while (Electro_MakeCurrentAdders(drbuffer, sizeof(drbuffer))) {
//                  ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, drbuffer, sizeof(drbuffer));
//                  Electro_SetNextAdders();
//              }
//              ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, drbuffer, sizeof(drbuffer));
              drbuffer_len = Electro_MakeProfileSettings(drbuffer);
              ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, drbuffer, drbuffer_len);
          }

//          if( CheckPowerLine() ){
//            if((DL_Mode == SLIP_FASTDL) || Modem_SendAdders() ) {
//              Schedule_PutUniqeEventsScatterRndTime( SEND_CURR_PARAM_EVENT, 60*5/*600*/, 0/*200*/, 200 );
//            }
//          }
        break;
        
        case SEND_TARIFS_EVENT:
          if (!CheckPowerLine()) break;
//          if (DL_Mode == SLIP_FASTDL) {
//              Schedule_PutUniqeEventsScatterRndTime( SEND_TARIFS_EVENT, 60*5/*600*/, 0/*200*/, 200 );
//              break;
//          }

          mQueue_PutMsg(STRIJ_LED_SHORT_ON_EVENT, 0);
          { // send 
              uint8_t drbuffer[8];
              uint8_t tariff_local = 1;
//              ModemState = MODEM_SEND_PAYLOAD;
              for (tariff_local = 1; tariff_local < TARIFF_MAX; tariff_local++) {
                  Electro_MakeTarifs(drbuffer, tariff_local);
                  ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, drbuffer, sizeof(drbuffer));
              }
//              while (Electro_MakeTarifs(drbuffer, sizeof(drbuffer))) {
//                  ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, drbuffer, sizeof(drbuffer));
//                  Electro_SetNextTarifs();
//              }
//              ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, drbuffer, sizeof(drbuffer));
          }
//          if( CheckPowerLine()) {
//            if( (DL_Mode == SLIP_FASTDL) || Modem_SendTarifs() ) {
//              Schedule_PutUniqeEventsScatterRndTime( SEND_TARIFS_EVENT, 60*5/*600*/, 0/*200*/, 200 );
//            }
//          }
        break;

//        case SEND_DAY_REGULAR_EVENT:
//          if (!CheckPowerLine()) break;
//          if (DL_Mode == SLIP_FASTDL) {
//              Schedule_PutUniqeEventsScatterRndTime( SEND_DAY_REGULAR_EVENT, 900, 200, 180 );
//              break;
//          }

//          { // send 
//              uint8_t drbuffer[8];
////              ModemState = MODEM_SEND_PAYLOAD;
//              while (Electro_MakeDayRegular(drbuffer, sizeof(drbuffer))) {
//                  ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, drbuffer, sizeof(drbuffer));
//                  Electro_SetNextRegularDay();
//              }
//              ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, drbuffer, sizeof(drbuffer));
//          }
//          Schedule_PutNextDayRegular( SEND_DAY_REGULAR_EVENT );
          
//          while (Modem_SendDayRegular()) {
//              Electro_MakeDayRegular( uint8_t *Buffer, size_t BufferLenght );
//          }
//          if((DL_Mode == SLIP_FASTDL) || Modem_SendDayRegular() ) {
//              Schedule_PutUniqeEventsScatterRndTime( SEND_DAY_REGULAR_EVENT, 900, 200, 180 );
//          } else {
//              Schedule_PutNextDayRegular( SEND_DAY_REGULAR_EVENT );
//          }
//          break;

//        case SEND_POST_MSG_EVENT:
//          if( CheckPowerLine() ) {
//          if((DL_Mode == SLIP_FASTDL) || Modem_SendPostEvent() )
//            Schedule_PutUniqeEventsScatterRndTime( SEND_POST_MSG_EVENT, 60*10/*60*60*/, 0/*200*/, 200 );
//          }
//          break;

        case DISPLAY_OFF_EVENT:
          if(CheckPowerLine()) break;
          Display_SetOff();
          mQueue_PutMsg( DEINIT_MAGNET_COVER_EVENT, 0 );
          break;

        case RED_BUTTON_EVENT:            //red button
//          hold_time = __time32(0);  
          if( !CheckPowerLine() ) Display_SetOn();
          break;
        case RED_BUTTON_RELEASE_EVENT:          
          Display_Show_Manual(RED_BUTTON_CLICK);
          //SwitchRelayOn();
          /*if( CheckPowerLine() && hold_time)
          {
            
            if( !Electro_GetCurrentAddersCnt() ) 
            {
                    MeterageHourlyDelta();
            }
            mQueue_PutMsg( SEND_CURR_PARAM_EVENT, 0 ); 
          } */              
//          if(hold_time) Display_Show_Manual(RED_BUTTON_CLICK);
//          hold_time = 0;
        break;
        case BLUE_BUTTON_EVENT:            //blue button
//          hold_time_blue = __time32(0); 
          if( !CheckPowerLine() ) Display_SetOn();
        break;
        case BLUE_BUTTON_RELEASE_EVENT:            //blue button release
          Display_Show_Manual(BLUE_BUTTON_CLICK);
//          if(hold_time_blue) Display_Show_Manual(BLUE_BUTTON_CLICK);
//          hold_time_blue = 0;
          break;
          
//        case INIT_COVER_EVENT:          Cover_Init();           break;
//        case INIT_MAGNET_EVENT:         Magnet_Init();          break;
//        case DEINIT_COVER_EVENT:        Cover_Deinit();         break;   
//        case DEINIT_MAGNET_EVENT:       Magnet_Deinit();        break;
//        case DEINIT_MAGNET_COVER_EVENT: CoverMagnet_Deinit();   break;
          
        case PWR_ONLINE_EVENT:
          AXSEM_ACK_timeout = 0;
          off_axsem = 0; 
          if (CheckPowerLine()) {
            gpio_INI_FLASH_POW;
            FLASH_Init_SPI();
            AXSEM_Reset();
            //Modem_SetEnergyMode( MODEM_NORMAL_MODE );
            //Modem_RX_OnOff(DL_Mode);
            //DL_Mode = SLIP_CRX;
            ModemUART_SLIP_Send(AXEM_CMD_SET_MODE, &DL_Mode, 1);
            Display_SetOn();
            FLASH_ResumeFromDeepSleep();
            ADE_SPI_Init();
            ADE_RegsInit();
            SetTrivialPwrSleep();
            PwrManager_SetStatus( true );
            Scheduler_Start();            
//            Schedule_PutUniqe( DISPLAY_NEXT_EVENT,  __time32(0) + DISPLAY_SHOW_TIME );
//            Meterage_ClearAll();
            LeUART_Init();
            gpio_SET_ADE_FREQ0;
//            GPIO_IntConfig( ADE_FREQ0.port, ADE_FREQ0.pin, true, true, true );
            black_screen(500000); 
            Display_Show_Auto_Next();
            display_auto_timer = 1;
//            Display_SetNextScreen();
            gpio_INI_LCD_BACKLIGHT;
            OptoUART_Init();
            ARCH_emit_event(ME_power_on, 0);
//            Event_Send( LINE_PWR_ON_ARCH_EVENT, __time32(0) );
            Schedule_PutUniqe( REQUEST_ID ,  __time32(0) + 1);
            if(Settings.Calibration_completed || Settings.DTI_address) SwitchRelayOn();
            else SwitchRelayOff();
          } else {
//              break;
            gpio_OFF_LCD_BACKLIGHT;
            gpio_OFF_STRIJ_LOGO;
            PwrManager_SetStatus( false );
            AXSEM_Off();
//            uint8_t DL_Mode_tmp = SLIP_DRX;
//            ModemUART_SLIP_Send(AXEM_CMD_SET_MODE, &DL_Mode_tmp, 1);
            //Modem_RX_OnOff(SLIP_DRX);
            ModemState = MODEM_READY;
//            ModemSendState = SEND_READY;
            LeUART_DeInit();
            ADE_SPI_Deinit();
            FLASH_Init_SPI();
            uint8_t flash_sleep_counter = 0;
            for (flash_sleep_counter = 0; flash_sleep_counter < 100; flash_sleep_counter++) {
                if (FLASH_GoToDeepSleep()) {
                    break;
                } else {
                    WDOG_Feed();
                    DelayMs(3);
                }
            }
            OptoUART_Deinit();
            //Calibration_Deinit();
            SetDeepPwrSleep();
            black_screen(0); 
            Display_SetOff();
            Scheduler_ClearAll();
            ARCH_emit_event(ME_power_off, 0);
            //Event_Send( LINE_PWR_OFF_ARCH_EVENT, __time32(0) );
            //GPIO_PinModeSet(STRIJ_LOGO.port, STRIJ_LOGO.pin, gpioModePushPull,  1);
            gpio_OFF_ADE_FREQ0;
            gpio_OFF_IMPLED;
            gpio_OFF_IMPOUT;
            FLASH_Deinit_SPI();
            gpio_OFF_FLASH_POW;
            gpio_OFF_AXSEM_RESET;
            //GPIO_PinModeSet(STRIJ_LOGO.port, STRIJ_LOGO.pin, gpioModeDisabled,  0);
            //CoverMagnet_Deinit();
          }
          break;


      
      
      case STRIJ_LED_ON_EVENT:
          if( CheckPowerLine() ) {
            CMU_ClockEnable( cmuClock_GPIO, true );
            gpio_INI_STRIJ_LOGO;
            Schedule_PutUniqe( STRIJ_LED_OFF_EVENT, __time32(0) + 4 );
          }
          break;
          
          case STRIJ_LED_SHORT_ON_EVENT :
          if( CheckPowerLine() ) {
            CMU_ClockEnable( cmuClock_GPIO, true );
            gpio_INI_STRIJ_LOGO;
            uint32_t delay = 100000;
            while(delay--);
            gpio_OFF_STRIJ_LOGO;
            //mQueue_PutMsg(STRIJ_LED_SHORT_ON_EVENT, 0);
          }
          break;
          
        case STRIJ_LED_OFF_EVENT:
            gpio_OFF_STRIJ_LOGO;
          break;

//        case MODEM_DROP_STATUS_EVENT:
//          mQueue_PutMsg( STRIJ_LED_OFF_EVENT, 0 );
//          
//          switch( Modem_ThrowStatus() ) {
//            case SEND_POST:
//              //Event_SetNextEventInBuffer();
//              Schedule_PutUniqeEventsScatterRndTime( SEND_POST_MSG_EVENT, 900, 300, 200 );
//              break;
//            case SEND_REGULAR:
//              Electro_SetNextRegularDay();
//              Schedule_PutUniqeEventsScatterRndTime( SEND_DAY_REGULAR_EVENT, 900, 300, 200 );
//              break;
//            case SEND_ADDERS:
//              Electro_SetNextAdders();
//              Schedule_PutUniqeEventsScatterRndTime( SEND_CURR_PARAM_EVENT, 60*5/*600*/, 0/*200*/, 200 );
//              break;
//          case SEND_TARIFS:
//              Electro_SetNextTarifs();
//              Schedule_PutUniqeEventsScatterRndTime( SEND_TARIFS_EVENT, 60*5/*600*/, 0/*200*/, 200 );
//              break;
//          }
//          break;
          
//      case AXEM_ASK_EVENT:
//        break;
//      case AXEM_NACK_EVENT:
//        break;

      /*case PULSE_CALIBRATION_EVENT:
        ADE_SetNewCalibration( Calibration_Calculate( recv_msg.Data ) );
        break;*/

        case WATCH_DOG_EVENT:
            WDOG_Feed();
        break;
      
//        case LEUART_RECV_EVENT:
//          //RS485_Parse( recv_msg.Data & 0xFF );
//          break;

//        case RS_485_CMD_EVENT:
//          //RS485_ExecuteCmd();
//          break;          
        case STORE_NEW_TARIF:
          Archive_SaveSettings();
          //Tarif_SaveTarif();
          ARCH_emit_event(ME_tariff_new, 0);
//          Event_Send( SET_NEW_TARIF_ARCH_EVENT, __time32(0) );
          break;
        case RELAY_ON:
          SwitchRelayOn();
          break;
        case RELAY_OFF:
          SwitchRelayOff();
          break;
        case SAVE_SETTINGS :
          Archive_SaveSettings();
          ADE_Configure_Outs();     
          break;      
        case REQUEST_ID:
          ModemUART_SLIP_Send(AXEM_CMD_SEND_GET_SERIAL, 0, 0);
          break;
        case SET_FAST_DL : {
            if (CheckPowerLine()) {
                DL_Mode = SLIP_FASTDL;
                ModemUART_SLIP_Send(AXEM_CMD_SET_MODE, &DL_Mode, 1);
                mQueue_PutMsg(STRIJ_LED_SHORT_ON_EVENT, 0);
            }
//            if(CheckPowerLine()) Modem_RX_OnOff(DL_Mode);
            break;
        }
        case RESET_FAST_DL:
          DL_Mode = SLIP_CRX;
          ModemUART_SLIP_Send(AXEM_CMD_SET_MODE, &DL_Mode, 1);
          mQueue_PutMsg(STRIJ_LED_SHORT_ON_EVENT, 0);
          //Modem_RX_OnOff(DL_Mode);
          ADE_alfa = 0;
        break;
        case UPDATE_CHECK:
           if (Settings.Flash_error) break;
         DTI_Enable = 0;
         if (checkForUpdate(UPGRADE_START_ADR)) SCB->AIRCR = 0x05FA0004;        //рестарт если есть обновления
         //FLASH_EraseBlock_64K(BASE_FIRM_START_ADR);
         if (Settings.Calibration_completed) copyFirmwareToFlash(BASE_FIRM_START_ADR);
         Schedule_PutUniqe( UPDATE_CHECK,   __time32(0) + 60*30);  
         DTI_Enable = 1;
        break;
        case CLEAR_UPDATE_AREA:
            FLASH_EraseBlock_64K(UPGRADE_START_ADR);
        break;
      }
    }
    ModemUART_SLIP_Dispatcher(&modem_slip_rxbuf, Modem_SetDLPacket);
    DTI_Poll();
    TD_RTC_Sleep();
  }

}
