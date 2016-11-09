// Code for processing buttons, triggers, etc.
//
// S.Omelchenko		14.10.2015
#include "triggersevents.h"
#include "main.h"

#include "mqueue.h"
#include "meterage.h"
#include "display.h"

//volatile BUTTON_STATES_E BlueButtonState = BUTTON_WAIT, RedButtonState = BUTTON_WAIT;
BUTTON_state btn_red = {0, 0};
BUTTON_state btn_blue = {0, 0};

void Buttons_Delay()
{
  uint32_t delay = 100000;
  while(delay--);
  Display_Set_Mode(DISLAY_MODE_MANUAL);
}

//****************************************************************
// Red button
//****************************************************************
void RedButtonCallback( uint8_t pin )
{
    if (!gpio_GET_BUTTON1) {
        if (!btn_red.state) {
            btn_red.time_event = __time32(0);
            btn_red.state = 1;
            mQueue_PutMsg( RED_BUTTON_EVENT, 0 );
            Buttons_Delay();
        }
    } else {
        if (btn_red.state) {
            btn_red.time_event = 0;
            btn_red.state = 0;
            mQueue_PutMsg( RED_BUTTON_RELEASE_EVENT, 0);
            Buttons_Delay();
        }
    }
}

//****************************************************************
// Blue button
//****************************************************************
void BlueButtonCallback( uint8_t pin )
{
    if (!gpio_GET_BUTTON2) {
        if (!btn_blue.state) {
            btn_blue.time_event = __time32(0);
            btn_blue.state = 1;
            mQueue_PutMsg( BLUE_BUTTON_EVENT, 0 );  
            Buttons_Delay();
        }
    } else {
        if (btn_blue.state) {
            btn_blue.time_event = 0;
            btn_blue.state = 0;
            mQueue_PutMsg( BLUE_BUTTON_RELEASE_EVENT, 0);  
            Buttons_Delay();
        }
    } 
}

uint8_t TestButtonLong(BUTTON_state * btn, uint8_t other_state) 
{
    if (!CheckPowerLine()) return 0;
    if (!btn->time_event) return 0;
    if (__time32(0) - btn->time_event <= 2) return 0;
    if (!btn->state) return 0;
    
    if (other_state) {
        if (btn == &btn_red && __time32(0) - btn->time_event > 10)  SCB->AIRCR = 0x05FA0004;
        else
        if (btn == &btn_blue && __time32(0) - btn->time_event > 2)  SwitchRelayOn();
        else
        return 0;
    }
    btn->time_event = 0;
    return 1;
}

void PowerCallback(uint8_t pin);
uint8_t PowerLine_curr = 0;

uint8_t CheckPowerLine(void)
{
    return PowerLine_curr;
}

uint8_t CheckPowerLine___(void)
{
//    return 1;
#ifdef CHECK_POWERLINE_FAKE
  return 1;
#else
//  return GPIO_PinInGet( EXT_PWR.port, EXT_PWR.pin );
//
  static uint8_t first = 1;
  if (first) {
      first = 0;
      gpio_INI_EXT_PWR(PowerCallback);
      gpio_SET_EXT_PWR;
  }
//  return GPIO_PinInGet( EXT_PWR.port, EXT_PWR.pin );
  return gpio_GET_EXT_PWR;
#endif
}

void Dispatcher_PowerLine()
{
    static uint32_t timer = 100;
    uint8_t power_state = CheckPowerLine___();
    if (PowerLine_curr != power_state && timer >= 5) {
        PowerLine_curr = power_state;
        mQueue_PutMsg( PWR_ONLINE_EVENT, PowerLine_curr ); 
    }
    if (++timer >= 100) timer = 100;
}

void PowerCallback(uint8_t pin)
{
    Dispatcher_PowerLine();
}

//#ifndef VER_2_3
void ADEFreq0Callback(uint8_t pin)
{
//  if(GPIO_PinInGet( ADE_FREQ0.port, ADE_FREQ0.pin ))
  if(gpio_GET_ADE_FREQ0)
  {
      gpio_INI_IMPLED;
//#ifdef VER_2_2
//    GPIO_PinModeSet( IMPLED.port, IMPLED.pin, gpioModePushPull, 1 );
//#else
//    GPIO_PinModeSet( IMPLED.port, IMPLED.pin, gpioModePushPull, 0 );
//#endif
  }
  else
  {
      gpio_RES_IMPLED;
//#ifdef VER_2_2
//    GPIO_PinModeSet( IMPLED.port, IMPLED.pin, gpioModePushPull, 0 );
//#else
//    GPIO_PinModeSet( IMPLED.port, IMPLED.pin, gpioModePushPull, 1 );
//#endif
  }
}
//#endif

//#ifndef VER_2_3
void ADEFreq1Callback(uint8_t pin)
{
//  if(GPIO_PinInGet( ADE_FREQ1.port, ADE_FREQ1.pin ))
  if(gpio_GET_ADE_FREQ1)
  {
      gpio_INI_IMPOUT;
//#ifdef VER_2_2
//    GPIO_PinModeSet( IMPOUT.port, IMPOUT.pin, gpioModePushPull, 1 );
//#else    
//    GPIO_PinModeSet( IMPOUT.port, IMPOUT.pin, gpioModePushPull, 0 );
//#endif
  }
  else
  {
      gpio_RES_IMPOUT;
//#ifdef VER_2_2
//    GPIO_PinModeSet( IMPOUT.port, IMPOUT.pin, gpioModePushPull, 0 );
//#else 
//    GPIO_PinModeSet( IMPOUT.port, IMPOUT.pin, gpioModePushPull, 1 );
//#endif
  }
}
//#endif


void Triggers_Init(void)
{
	CMU_ClockEnable( cmuClock_GPIO, true );
	GPIOINT_Init();

	// Button 1
    gpio_INI_BUTTON1(RedButtonCallback);
    gpio_SET_BUTTON1;
//	GPIO_PinModeSet( BUTTON1.port, BUTTON1.pin, gpioModeInputPullFilter, 1 );
//	GPIOINT_CallbackRegister( BUTTON1.pin, RedButtonCallback );
//	GPIO_IntConfig( BUTTON1.port, BUTTON1.pin, true, true, true );

	// Button 2
    gpio_INI_BUTTON2(BlueButtonCallback);
    gpio_SET_BUTTON2;
//	GPIO_PinModeSet( BUTTON2.port, BUTTON2.pin, gpioModeInputPullFilter, 1 );
//	GPIOINT_CallbackRegister( BUTTON2.pin, BlueButtonCallback );
//	GPIO_IntConfig( BUTTON2.port, BUTTON2.pin, true, true, true );

	// Power line sensor

	// IR case open switch trigger

	// Magnetic field trigger

	// Supply for triggers

        // CF1,CF2
        //GPIO_PinModeSet(IMPOUT.port, IMPOUT.pin, gpioModePushPull, 0);
        
//        #ifndef VER_2_3
        gpio_INI_ADE_FREQ0(ADEFreq0Callback);
        gpio_SET_ADE_FREQ0;
//        GPIO_PinModeSet( ADE_FREQ0.port, ADE_FREQ0.pin, gpioModeInputPullFilter, 0 );
//	GPIOINT_CallbackRegister( ADE_FREQ0.pin, ADEFreq0Callback );
//	GPIO_IntConfig( ADE_FREQ0.port, ADE_FREQ0.pin, true, true, true );
   
        gpio_INI_ADE_FREQ1(ADEFreq1Callback);
        gpio_SET_ADE_FREQ1;
//        GPIO_PinModeSet( ADE_FREQ1.port, ADE_FREQ1.pin, gpioModeInputPullFilter, 0 );
//	GPIOINT_CallbackRegister( ADE_FREQ1.pin, ADEFreq1Callback );
//	GPIO_IntConfig( ADE_FREQ1.port, ADE_FREQ1.pin, true, true, true );       
//        #endif
        
//        GPIO_PinModeSet( EXT_PWR.port, EXT_PWR.pin, gpioModeInputPullFilter, 0 );

//        gpio_INI_EXT_PWR(PowerCallback);
//        gpio_SET_EXT_PWR;
        
//        GPIO_PinModeSet( EXT_PWR.port, EXT_PWR.pin, gpioModeInputPullFilter, 0 );
//#ifndef VER_2_3
//        GPIOINT_CallbackRegister( EXT_PWR.pin, PowerCallback );
//	GPIO_IntConfig( EXT_PWR.port, EXT_PWR.pin, true, true, true );
//#endif
}

void Triggers_Deinit()
{

}