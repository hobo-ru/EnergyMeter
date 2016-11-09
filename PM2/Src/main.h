#ifndef MAIN_H
#define MAIN_H

#include "em_chip.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "em_usart.h"
#include <stdio.h>
#include <time.h>

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "croutine.h"
#include "event_groups.h"

#include "sleep.h"
#include "gpiointerrupt.h"

#include "defines_HW.h"
#include "modem.h"

#include "timeevents.h"
#include "triggersevents.h"

#include "td_rtc.h"

#include "defines_HW.h"
//typedef struct {
//	GPIO_Port_TypeDef port;
//	unsigned int pin;
//}gpio_t;
//
//
//// Pin aliases
//#define ADE_CS 			((gpio_t){gpioPortC, 8	})
//#define ADE_SCK 		((gpio_t){gpioPortC, 9	})
//#define ADE_SDI 		((gpio_t){gpioPortC, 10	})
//#define ADE_SDO 		((gpio_t){gpioPortC, 11	})
//#define ADE_FREQ0 		((gpio_t){gpioPortA, 0	})
//#define ADE_FREQ1 		((gpio_t){gpioPortB, 11	})
//#define ADE_IRQ 		((gpio_t){gpioPortF, 2	})
//#define ADE_RESET 		((gpio_t){gpioPortA, 10	})
//#ifdef VER_3_0
//#define LCD_CS          ((gpio_t){gpioPortE, 10    })
//#define LCD_SDO         ((gpio_t){gpioPortB, 11    })
//#else
//#define LCD_CS 			((gpio_t){gpioPortC, 0	})
//#define LCD_SDO 		((gpio_t){gpioPortC, 1	})
//#endif
//#define LCD_SDI 		((gpio_t){gpioPortC, 2	})
//#define LCD_SCK 		((gpio_t){gpioPortC, 3	})
//#define LCD_RES 		((gpio_t){gpioPortC, 4	})
//#define LCD_A0 			((gpio_t){gpioPortC, 13	})
//#define LCD_POWER 		((gpio_t){gpioPortC, 14	})
//#define LCD_BACKLIGHT 	        ((gpio_t){gpioPortF, 3	})
//#define FLASH_CS 		((gpio_t){gpioPortC, 15	})
//#ifdef VER_3_0
//#define FLASH_SI         ((gpio_t){gpioPortB, 11    })      // EEPROM INPUT
//#else
//#define FLASH_SI 		((gpio_t){gpioPortC, 1	})      // EEPROM INPUT
//#endif
//#define FLASH_SO 		((gpio_t){gpioPortC, 2	})      // EEEPROM OUTPUT
//#define FLASH_SCL 		((gpio_t){gpioPortC, 3	})
//#define TRIGGERS_SUP 	        ((gpio_t){gpioPortA, 8	})
//#define MAGNET_TRIGGER	        ((gpio_t){gpioPortA, 9	})
//#define IR_TRIGGER		((gpio_t){gpioPortE, 13	})
//#define RS485_TX 		((gpio_t){gpioPortB, 13	})
//#define RS485_RX 		((gpio_t){gpioPortB, 14	})
//#define BUTTON1 		((gpio_t){gpioPortD, 4	})
//#define BUTTON2 		((gpio_t){gpioPortD, 5	})
//#define RADIO_RX 		((gpio_t){gpioPortD, 6	})
//#define RADIO_TX 		((gpio_t){gpioPortD, 7	})
//#define RX485_TX_EN 	        ((gpio_t){gpioPortE, 11	})
//#define AXSEM_RESET 	((gpio_t){gpioPortE, 11	})
//#define IMPLED 			((gpio_t){gpioPortE, 10	})	// Pulse LED
//#define RELAYON 		((gpio_t){gpioPortF, 4	})	// Relay close pulse
//#define RELAYOFF 		((gpio_t){gpioPortF, 5	})	// Relay open pulse
//#define STRIJ_LOGO		((gpio_t){gpioPortA, 2	})	// Strij logo LED
//#define IMPOUT	 		((gpio_t){gpioPortA, 1	})	// Decoupled pulse output
//#define EXT_PWR			((gpio_t){gpioPortE, 12	})	// High when power line is under voltage
//#define AXEM_TX			((gpio_t){gpioPortD, 7	})
//#define AXEM_RX			((gpio_t){gpioPortD, 6	})
//
//#define S3S4_SUP		((gpio_t){gpioPortA, 8	})
//#define S3		        ((gpio_t){gpioPortE, 13	})
//#define S4		        ((gpio_t){gpioPortA, 9	})




#endif