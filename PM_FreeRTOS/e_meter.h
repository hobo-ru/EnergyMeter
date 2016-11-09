//e_meter.h
#ifndef E_METER_H
#define E_METER_H
#include "pinout.h"
#include "time.h"
void taskRadioSend (void *pParameters);
void taskDisplay (void *);
void taskTimeMonitor (void *);
void PowerMonitor ();
void taskPowerCount (void *);
void taskPowerCalc (void *);
uint16_t GetTarifFromTime(time_t);
void INPirqSetup (void);
void INPirqEnable (bool);
void DelayNOP (uint32_t);
void ext_write (uint8_t);
uint8_t ext_ctrl (uint8_t, uint8_t);
void ext_init (void);
#define _BV(x) (1<<x)
//#define PW_CALIBRATE

enum port_extender_bits
{
	PE_RESET_ALL = 0x00,
	PE_LOGO_LED = 0x01,					// QP7
	PE_RS485_TX_EN = 0x02,				// QP6
	PE_PULSE_OUT = 0x04,				// QP5
	PE_LCD_RES = 0x10,					// QP3
	PE_RELAY_ON = 0x20,					// QP2
	PE_BACKLIGHT = 0x40,				// QP1
	PE_RED_LED = 0x80,					// QP0
};

enum pe_status { PE_OFF = 0, PE_ON = 1, PE_GET = 2,};
#define IN1201                  1201

/** Module P/N in string format */
#define	MODULE_PN               "IN1201"

/** Module revision index */
#define REVISION_D              4	///< Revision D
#define REVISION_E              5	///< Revision E
#define REVISION_F              6	///< Revision F
#define REVISION_VBC4           7	///< VBC4

#ifndef MODULE_REVISION

/** Module reveision */
#define MODULE_REVISION         REVISION_F

#define GATEWAY_DEBUG				///< Default build configuration
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ICCARM__)

/** Macro for packing structure depending on the used compiler */
#define __PACKED                __packed

/** Macro for aligning structure depending on the used compiler */
#define __ALIGN(i)              __align(i)

/** Macro for RAM function declaration depending on the used compiler */
#define __RAMFUNCTION           __ramfunc

#elif defined(__GNUC__)

#define __PACKED                __attribute__((packed))

#define __ALIGN(i)              __attribute__((aligned(i)))

#define __RAMFUNCTION           __attribute__((section(".data.ram")))

#endif
#endif
#endif