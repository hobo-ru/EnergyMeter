#ifndef DEFINES_VER_110_H
#define DEFINES_VER_110_H

#include "em_chip.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_usart.h"

//#define MODEM_LOW_POWER_MODE_value (5)
//#define MODEM_NORMAL_MODE_value (5)
#define DD_ADE7953
#define OPTO_PORT

typedef struct {
	GPIO_Port_TypeDef port;
	unsigned int pin;
}gpio_t;

// Pin aliases
#define ADE_CS 			((gpio_t){gpioPortC, 8	})
#define ADE_SCK 		((gpio_t){gpioPortC, 9	})
#define ADE_SDI 		((gpio_t){gpioPortC, 10	})
#define ADE_SDO 		((gpio_t){gpioPortC, 11	})
#define ADE_RESET 		((gpio_t){gpioPortA, 10	})
#define LCD_CS 			//((gpio_t){gpioPortC, 0	})
#define LCD_SDO 		//((gpio_t){gpioPortC, 1	})
#define LCD_SCK 		//((gpio_t){gpioPortC, 3	})
#define LCD_RES 		//((gpio_t){gpioPortC, 4	})
#define LCD_A0 			//((gpio_t){gpioPortC, 13	})
#define LCD_POWER 		//((gpio_t){gpioPortC, 14	})
#define LCD_BACKLIGHT 	//((gpio_t){gpioPortF, 3	})
#define FLASH_CS 		((gpio_t){gpioPortC, 15	})
#define FLASH_SI 		((gpio_t){gpioPortB, 11	})      // EEPROM INPUT
#define FLASH_SO 		((gpio_t){gpioPortC, 2	})      // EEEPROM OUTPUT
#define FLASH_SCL 		((gpio_t){gpioPortC, 3	})
#define FLASH_POW       ((gpio_t){gpioPortA, 0	})
#define RSopto_TX       ((gpio_t){gpioPortC, 0	})
#define RSopto_RX       ((gpio_t){gpioPortC, 1	})
#define RS485_TX 		((gpio_t){gpioPortB, 13	})
#define RS485_RX 		((gpio_t){gpioPortB, 14	})
#define AXEM_TX			((gpio_t){gpioPortD, 7	})
#define AXEM_RX			((gpio_t){gpioPortD, 6	})
#define AXSEM_RESET 	((gpio_t){gpioPortE, 11	})
#define BUTTON1 		((gpio_t){gpioPortD, 4	})
#define BUTTON2 		((gpio_t){gpioPortD, 5	})
#define RELAYON 		((gpio_t){gpioPortF, 4	})	// Relay close pulse
#define RELAYOFF 		((gpio_t){gpioPortF, 5	})	// Relay open pulse
#define STRIJ_LOGO		((gpio_t){gpioPortF, 3	})	// Strij logo LED
#define EXT_PWR			((gpio_t){gpioPortE, 12	})	// High when power line is under voltage

// ADE
#define gpio_INI_ADE_CS         GPIO_PinModeSet(ADE_CS.port, ADE_CS.pin, gpioModePushPull,      1)   
#define gpio_RES_ADE_CS         GPIO_PinModeSet(ADE_CS.port, ADE_CS.pin, gpioModePushPull,      0)   
#define gpio_OFF_ADE_CS         GPIO_PinModeSet(ADE_CS.port, ADE_CS.pin, gpioModeDisabled,      0)  

#define gpio_INI_ADE_SCK        GPIO_PinModeSet(ADE_SCK.port, ADE_SCK.pin, gpioModePushPull,    1)   
#define gpio_SET_ADE_SCK        GPIO_PinOutSet(ADE_SCK.port, ADE_SCK.pin)  
#define gpio_RES_ADE_SCK        GPIO_PinOutClear(ADE_SCK.port, ADE_SCK.pin)  
#define gpio_OFF_ADE_SCK        GPIO_PinModeSet(ADE_SCK.port, ADE_SCK.pin, gpioModeDisabled,    0) 

#define gpio_INI_ADE_SDI        GPIO_PinModeSet(ADE_SDI.port, ADE_SDI.pin, gpioModeInput, 		0)   
#define gpio_OFF_ADE_SDI        GPIO_PinModeSet(ADE_SDI.port, ADE_SDI.pin, gpioModeDisabled,    0) 
#define gpio_GET_ADE_SDI        GPIO_PinInGet(ADE_SDI.port, ADE_SDI.pin)

#define gpio_INI_ADE_SDO        GPIO_PinModeSet(ADE_SDO.port, ADE_SDO.pin, gpioModePushPull,    1)   
#define gpio_SET_ADE_SDO        GPIO_PinOutSet(ADE_SDO.port, ADE_SDO.pin)  
#define gpio_RES_ADE_SDO        GPIO_PinOutClear(ADE_SDO.port, ADE_SDO.pin)  
#define gpio_OFF_ADE_SDO        GPIO_PinModeSet(ADE_SDO.port, ADE_SDO.pin, gpioModeDisabled,    0) 

#define gpio_INI_ADE_FREQ0(a)   //GPIO_PinModeSet(ADE_FREQ0.port, ADE_FREQ0.pin, gpioModeInputPullFilter, 0); GPIOINT_CallbackRegister(ADE_FREQ0.pin, a)
#define gpio_SET_ADE_FREQ0      //GPIO_IntConfig(ADE_FREQ0.port, ADE_FREQ0.pin, true, true, true)
#define gpio_OFF_ADE_FREQ0      //GPIO_IntConfig(ADE_FREQ0.port, ADE_FREQ0.pin, true, true, false)
#define gpio_GET_ADE_FREQ0      0 //GPIO_PinInGet(ADE_FREQ0.port, ADE_FREQ0.pin)

#define gpio_INI_ADE_FREQ1(a)   //GPIO_PinModeSet(ADE_FREQ1.port, ADE_FREQ1.pin, gpioModeInputPullFilter, 0); GPIOINT_CallbackRegister(ADE_FREQ1.pin, a)
#define gpio_SET_ADE_FREQ1      //GPIO_IntConfig(ADE_FREQ1.port, ADE_FREQ1.pin, true, true, true)
#define gpio_OFF_ADE_FREQ1      //GPIO_IntConfig(ADE_FREQ1.port, ADE_FREQ1.pin, true, true, false)
#define gpio_GET_ADE_FREQ1      0 //GPIO_PinInGet(ADE_FREQ1.port, ADE_FREQ1.pin)

#define gpio_INI_ADE_IRQ        //GPIO_PinModeSet(ADE_IRQ.port, ADE_IRQ.pin, gpioModeInput, 		0)   
#define gpio_OFF_ADE_IRQ        //GPIO_PinModeSet(ADE_IRQ.port, ADE_IRQ.pin, gpioModeDisabled,    0) 

#define gpio_INI_ADE_RES        //GPIO_PinModeSet(ADE_IRQ.port, ADE_IRQ.pin, gpioModeInput, 		0)   
#define gpio_OFF_ADE_RES        GPIO_PinModeSet(ADE_RESET.port, ADE_RESET.pin, gpioModeDisabled,    0) 

// LCD
#define gpio_INI_LCD_CS         //GPIO_PinModeSet(LCD_CS.port, LCD_CS.pin, gpioModePushPull, 1)
#define gpio_SET_LCD_CS         //GPIO_PinOutSet(LCD_CS.port, LCD_CS.pin)  
#define gpio_RES_LCD_CS         //GPIO_PinOutClear(LCD_CS.port, LCD_CS.pin)  
#define gpio_OFF_LCD_CS         //GPIO_PinModeSet(LCD_CS.port, LCD_CS.pin, gpioModeDisabled, 0) 

#define gpio_INI_LCD_SDO        //GPIO_PinModeSet(LCD_SDO.port, LCD_SDO.pin, gpioModePushPull, 0)
#define gpio_SET_LCD_SDO        //GPIO_PinOutSet(LCD_SDO.port, LCD_SDO.pin)  
#define gpio_RES_LCD_SDO        //GPIO_PinOutClear(LCD_SDO.port, LCD_SDO.pin)  
#define gpio_OFF_LCD_SDO        //GPIO_PinModeSet(LCD_SDO.port, LCD_SDO.pin, gpioModeDisabled, 0) 

#define gpio_INI_LCD_SCK        //GPIO_PinModeSet(LCD_SCK.port, LCD_SCK.pin, gpioModePushPull, 1)
#define gpio_SET_LCD_SCK        //GPIO_PinOutSet(LCD_SCK.port, LCD_SCK.pin)  
#define gpio_RES_LCD_SCK        //GPIO_PinOutClear(LCD_SCK.port, LCD_SCK.pin)  
#define gpio_OFF_LCD_SCK        //GPIO_PinModeSet(LCD_SCK.port, LCD_SCK.pin, gpioModeDisabled, 0) 

#define gpio_INI_LCD_RES        //GPIO_PinModeSet(LCD_RES.port, LCD_RES.pin, gpioModePushPull, 1)
#define gpio_SET_LCD_RES        //GPIO_PinOutSet(LCD_RES.port, LCD_RES.pin)  
#define gpio_RES_LCD_RES        //GPIO_PinOutClear(LCD_RES.port, LCD_RES.pin)  
#define gpio_OFF_LCD_RES        //GPIO_PinModeSet(LCD_RES.port, LCD_RES.pin, gpioModeDisabled, 0) 

#define gpio_INI_LCD_A0         //GPIO_PinModeSet(LCD_A0.port, LCD_A0.pin, gpioModePushPull, 0)
#define gpio_SET_LCD_A0         //GPIO_PinOutSet(LCD_A0.port, LCD_A0.pin)  
#define gpio_RES_LCD_A0         //GPIO_PinOutClear(LCD_A0.port, LCD_A0.pin)  
#define gpio_OFF_LCD_A0         //GPIO_PinModeSet(LCD_A0.port, LCD_A0.pin, gpioModeDisabled, 0) 

#define gpio_INI_LCD_POWER      //GPIO_PinModeSet(LCD_POWER.port, LCD_POWER.pin, gpioModePushPull, 1)
#define gpio_SET_LCD_POWER      //GPIO_PinOutSet(LCD_POWER.port, LCD_POWER.pin)  
#define gpio_RES_LCD_POWER      //GPIO_PinOutClear(LCD_POWER.port, LCD_POWER.pin)  
#define gpio_OFF_LCD_POWER      //GPIO_PinModeSet(LCD_POWER.port, LCD_POWER.pin, gpioModeDisabled, 0) 

#define gpio_INI_LCD_BACKLIGHT  //GPIO_PinModeSet(LCD_BACKLIGHT.port, LCD_BACKLIGHT.pin, gpioModePushPull,    0)   
#define gpio_OFF_LCD_BACKLIGHT  //GPIO_PinModeSet(LCD_BACKLIGHT.port, LCD_BACKLIGHT.pin, gpioModeDisabled,    0) 

// FLASH
#define gpio_INI_FLASH_CS       GPIO_PinModeSet(FLASH_CS.port, FLASH_CS.pin, gpioModePushPull, 1)
#define gpio_SET_FLASH_CS       GPIO_PinOutSet(FLASH_CS.port, FLASH_CS.pin)  
#define gpio_RES_FLASH_CS       GPIO_PinOutClear(FLASH_CS.port, FLASH_CS.pin)  
#define gpio_OFF_FLASH_CS       GPIO_PinModeSet(FLASH_CS.port, FLASH_CS.pin, gpioModeDisabled, 0) 

#define gpio_INI_FLASH_SI       GPIO_PinModeSet(FLASH_SI.port, FLASH_SI.pin, gpioModePushPull, 1)
#define gpio_SET_FLASH_SI       GPIO_PinOutSet(FLASH_SI.port, FLASH_SI.pin)  
#define gpio_RES_FLASH_SI       GPIO_PinOutClear(FLASH_SI.port, FLASH_SI.pin)  
#define gpio_OFF_FLASH_SI       GPIO_PinModeSet(FLASH_SI.port, FLASH_SI.pin, gpioModeDisabled, 0) 

#define gpio_INI_FLASH_SO       GPIO_PinModeSet(FLASH_SO.port, FLASH_SO.pin, gpioModeInput,    1)
#define gpio_OFF_FLASH_SO       GPIO_PinModeSet(FLASH_SO.port, FLASH_SO.pin, gpioModeDisabled, 0)
#define gpio_GET_FLASH_SO       GPIO_PinInGet(FLASH_SO.port, FLASH_SO.pin)

#define gpio_INI_FLASH_SCL      GPIO_PinModeSet(FLASH_SCL.port, FLASH_SCL.pin, gpioModePushPull, 1)
#define gpio_SET_FLASH_SCL      GPIO_PinOutSet(FLASH_SCL.port, FLASH_SCL.pin)  
#define gpio_RES_FLASH_SCL      GPIO_PinOutClear(FLASH_SCL.port, FLASH_SCL.pin)  
#define gpio_OFF_FLASH_SCL      GPIO_PinModeSet(FLASH_SCL.port, FLASH_SCL.pin, gpioModeDisabled, 0) 

#define gpio_INI_FLASH_POW      GPIO_PinModeSet(FLASH_POW.port, FLASH_POW.pin, gpioModePushPull, 1)
#define gpio_OFF_FLASH_POW      GPIO_PinModeSet(FLASH_POW.port, FLASH_POW.pin, gpioModeDisabled, 0) 

// RS
#define gpio_INI_RS485_TX       GPIO_PinModeSet(RS485_TX.port, RS485_TX.pin, gpioModePushPull,  1)
#define gpio_OFF_RS485_TX       GPIO_PinModeSet(RS485_TX.port, RS485_TX.pin, gpioModePushPull,  0)

#define gpio_INI_RS485_RX       GPIO_PinModeSet(RS485_RX.port, RS485_RX.pin, gpioModeInputPull, 1)
#define gpio_OFF_RS485_RX       GPIO_PinModeSet(RS485_RX.port, RS485_RX.pin, gpioModeDisabled,  0)

#define gpio_INI_RS485_TX_EN
#define gpio_OFF_RS485_TX_EN

#define gpio_INI_AXEM_TX        GPIO_PinModeSet(AXEM_TX.port, AXEM_TX.pin, gpioModePushPull,  1)
#define gpio_OFF_AXEM_TX        GPIO_PinModeSet(AXEM_TX.port, AXEM_TX.pin, gpioModeDisabled,  0)

#define gpio_INI_AXEM_RX        GPIO_PinModeSet(AXEM_RX.port, AXEM_RX.pin, gpioModeInput,     0)
#define gpio_OFF_AXEM_RX        GPIO_PinModeSet(AXEM_RX.port, AXEM_RX.pin, gpioModeDisabled,  0)

#define gpio_INI_AXSEM_RESET    GPIO_PinModeSet(AXSEM_RESET.port, AXSEM_RESET.pin, gpioModePushPull, 0)
#define gpio_SET_AXSEM_RESET    GPIO_PinModeSet(AXSEM_RESET.port, AXSEM_RESET.pin, gpioModePushPull, 1)
#define gpio_OFF_AXSEM_RESET    GPIO_PinModeSet(AXSEM_RESET.port, AXSEM_RESET.pin, gpioModeInputPull,1)

#define gpio_INI_RSopto_TX      GPIO_PinModeSet(RSopto_TX.port, RSopto_TX.pin, gpioModePushPull,  1)
#define gpio_OFF_RSopto_TX      GPIO_PinModeSet(RSopto_TX.port, RSopto_TX.pin, gpioModePushPull,  0)

#define gpio_INI_RSopto_RX      GPIO_PinModeSet(RSopto_RX.port, RSopto_RX.pin, gpioModeInputPull, 1)
#define gpio_OFF_RSopto_RX      GPIO_PinModeSet(RSopto_RX.port, RSopto_RX.pin, gpioModeDisabled,  0)

// BTN
#define gpio_INI_BUTTON1(a)     GPIO_PinModeSet(BUTTON1.port, BUTTON1.pin, gpioModeInputPullFilter, 1); GPIOINT_CallbackRegister(BUTTON1.pin, a)
#define gpio_SET_BUTTON1        GPIO_IntConfig(BUTTON1.port, BUTTON1.pin, true, true, true)
#define gpio_OFF_BUTTON1        GPIO_IntConfig(BUTTON1.port, BUTTON1.pin, true, true, false)
#define gpio_GET_BUTTON1        GPIO_PinInGet(BUTTON1.port, BUTTON1.pin)

#define gpio_INI_BUTTON2(a)     GPIO_PinModeSet(BUTTON2.port, BUTTON2.pin, gpioModeInputPullFilter, 1); GPIOINT_CallbackRegister(BUTTON2.pin, a)
#define gpio_SET_BUTTON2        GPIO_IntConfig(BUTTON2.port, BUTTON2.pin, true, true, true)
#define gpio_OFF_BUTTON2        GPIO_IntConfig(BUTTON2.port, BUTTON2.pin, true, true, false)
#define gpio_GET_BUTTON2        GPIO_PinInGet(BUTTON2.port, BUTTON2.pin)

// LED, PIN_OUT
#define gpio_INI_IMPLED         //GPIO_PinModeSet(IMPLED.port, IMPLED.pin, gpioModePushPull,  1)
#define gpio_RES_IMPLED         //GPIO_PinModeSet(IMPLED.port, IMPLED.pin, gpioModePushPull,  0)
#define gpio_OFF_IMPLED         //GPIO_PinModeSet(IMPLED.port, IMPLED.pin, gpioModeDisabled,  0)

#define gpio_INI_IMPOUT         //GPIO_PinModeSet(IMPOUT.port, IMPOUT.pin, gpioModePushPull,  1)
#define gpio_RES_IMPOUT         //GPIO_PinModeSet(IMPOUT.port, IMPOUT.pin, gpioModePushPull,  0)
#define gpio_OFF_IMPOUT         //GPIO_PinModeSet(IMPOUT.port, IMPOUT.pin, gpioModeDisabled,  0)

#define gpio_INI_RELAYON        GPIO_PinModeSet(RELAYOFF.port, RELAYOFF.pin, gpioModePushPull, 1)
#define gpio_OFF_RELAYON        GPIO_PinModeSet(RELAYOFF.port, RELAYOFF.pin, gpioModeWiredAndPullUp, 0)

#define gpio_INI_RELAYOFF       GPIO_PinModeSet(RELAYON.port, RELAYON.pin, gpioModePushPull, 1)
#define gpio_OFF_RELAYOFF       GPIO_PinModeSet(RELAYON.port, RELAYON.pin, gpioModeWiredAndPullUp, 0)

#define gpio_INI_STRIJ_LOGO     GPIO_PinModeSet(STRIJ_LOGO.port, STRIJ_LOGO.pin, gpioModePushPull,  1)
#define gpio_OFF_STRIJ_LOGO     GPIO_PinModeSet(STRIJ_LOGO.port, STRIJ_LOGO.pin, gpioModeDisabled,  0)

#define gpio_INI_EXT_PWR(a)     GPIO_PinModeSet(EXT_PWR.port, EXT_PWR.pin, gpioModeInput, 0)//; GPIOINT_CallbackRegister(EXT_PWR.pin, a)
#define gpio_SET_EXT_PWR        //GPIO_IntConfig(EXT_PWR.port, EXT_PWR.pin, true, true, true) //GPIO_PinModeSet(EXT_PWR.port, EXT_PWR.pin, gpioModeInputPullFilter, 0)
//#define gpio_INI_EXT_PWR(a)     GPIO_PinModeSet(EXT_PWR.port, EXT_PWR.pin, gpioModeInputPull, 0); GPIOINT_CallbackRegister(EXT_PWR.pin, a)
//#define gpio_INI_EXT_PWR(a)     GPIO_PinModeSet(EXT_PWR.port, EXT_PWR.pin, gpioModeInputPullFilter, 0); GPIOINT_CallbackRegister(EXT_PWR.pin, a)
//#define gpio_SET_EXT_PWR        GPIO_IntConfig(EXT_PWR.port, EXT_PWR.pin, true, true, true) //GPIO_PinModeSet(EXT_PWR.port, EXT_PWR.pin, gpioModeInputPullFilter, 0)
#define gpio_OFF_EXT_PWR        GPIO_IntConfig(EXT_PWR.port, EXT_PWR.pin, true, true, false)
#define gpio_GET_EXT_PWR        GPIO_PinInGet(EXT_PWR.port, EXT_PWR.pin)

#define gpio_INI_S3S4_SUP       //GPIO_PinModeSet(S3S4_SUP.port, S3S4_SUP.pin, gpioModePushPull,  1)
#define gpio_OFF_S3S4_SUP       //GPIO_PinModeSet(S3S4_SUP.port, S3S4_SUP.pin, gpioModeDisabled,  0)

#define gpio_INI_S4             //GPIO_PinModeSet(S4.port, S4.pin, gpioModeWiredAndPullUpFilter, 1)
#define gpio_OFF_S4             //GPIO_PinModeSet(S4.port, S4.pin, gpioModeDisabled,  0)
#define gpio_GET_S4             1 //GPIO_PinInGet(S4.port, S4.pin)

#define gpio_INI_S_OPTO_COVER_SUP   //GPIO_PinModeSet(S_OPTO_COVER_SUP.port, S_OPTO_COVER_SUP.pin, gpioModePushPull,  1)
#define gpio_OFF_S_OPTO_COVER_SUP   //GPIO_PinModeSet(S_OPTO_COVER_SUP.port, S_OPTO_COVER_SUP.pin, gpioModeDisabled,  0)

#define gpio_INI_S_HALL_SUP         //gpio_INI_S_OPTO_COVER_SUP
#define gpio_OFF_S_HALL_SUP         //gpio_OFF_S_OPTO_COVER_SUP

#define gpio_INI_S_OPTO_BODY_SUP    //GPIO_PinModeSet(S_OPTO_BODY_SUP.port, S_OPTO_BODY_SUP.pin, gpioModePushPull,  1)
#define gpio_OFF_S_OPTO_BODY_SUP    //GPIO_PinModeSet(S_OPTO_BODY_SUP.port, S_OPTO_BODY_SUP.pin, gpioModeDisabled,  0)

#define gpio_INI_S_OPTO_COVER       //GPIO_PinModeSet(S_OPTO_COVER.port, S_OPTO_COVER.pin, gpioModeInput,    0)
#define gpio_OFF_S_OPTO_COVER       //GPIO_PinModeSet(S_OPTO_COVER.port, S_OPTO_COVER.pin, gpioModeDisabled, 0)
#define gpio_GET_S_OPTO_COVER       //GPIO_PinInGet(S_OPTO_COVER.port, S_OPTO_COVER.pin)

#define gpio_INI_S_HALL             //GPIO_PinModeSet(S_HALL.port, S_HALL.pin, gpioModeInputPull,1)
#define gpio_OFF_S_HALL             //GPIO_PinModeSet(S_HALL.port, S_HALL.pin, gpioModeDisabled, 0)
#define gpio_GET_S_HALL             1//GPIO_PinInGet(S_HALL.port, S_HALL.pin)

#define gpio_INI_S_OPTO_BODY        //gpio_INI_S_OPTO_COVER
#define gpio_OFF_S_OPTO_BODY        //gpio_OFF_S_OPTO_COVER
#define gpio_GET_S_OPTO_BODY        //gpio_GET_S_OPTO_COVER

#define gpio_OFF_SWIO               //GPIO_PinModeSet(SWIO.port, SWIO.pin, gpioModeDisabled, 0)

#endif
