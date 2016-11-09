#ifndef __DTI_H
#define __DTI_H

#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"

#include "mqueue.h"
#include "scheduler.h"
//#include "event.h"
#include "modem_uart.h"

typedef struct 
{
  uint8_t name[16];
  uint16_t hardware_ID;
  uint8_t  hardware_rev;
  uint8_t  software_rev;
  uint8_t  software_subrev;
  uint16_t packet_size;
  uint32_t RAM_RO_startadd;
  uint32_t RAM_RW_startadd;
  uint32_t EE_RO_startadd;
  uint32_t EE_RW_startadd;
  uint32_t End_Of_Mem;
} DTI_INFO; 


#define  UINT8_TYPE             0
#define  INT8_TYPE              1
#define  UINT16_TYPE            2
#define  INT16_TYPE             3
#define  UINT32_TYPE            4
#define  INT32_TYPE             5
#define  UINT8_MAS_TYPE         6
#define  INT8_MAS_TYPE          7
#define  UINT16_MAS_TYPE        8
#define  INT16_MAS_TYPE         9
#define  UINT32_MAS_TYPE        10
#define  INT32_MAS_TYPE         11
#define  STRUCT_TYPE            12
#define  FUNCTION_TYPE          13  
#define  TIME_TYPE              14  
#define  FLOAT_TYPE             15
#define  FLOAT_MAS_TYPE         16



#define  SEGMENT_RAM_RO         0
#define  SEGMENT_RAM_RW         (1<<6)
#define  SEGMENT_EE_RO          (2<<6)
#define  SEGMENT_EE_RW          (3<<6)

typedef struct 
{
  uint8_t name[10];
  uint16_t id;
  uint8_t type;
  uint32_t address;
  uint16_t parent_id;
}  DTI_Property;

//#pragma pack(push, 1)
//typedef struct {
//  uint32_t address;
//  uint16_t id;
//  uint16_t parent_id;
//  uint8_t type;
//  uint8_t name[11];
//}  DTI_Property; // 20 bytes
//#pragma pack(pop)

#define LEUART_INTERFACE        0
#define AX_FAST_DL_INTERFACE    1
#define OPTO_FAST_DL_INTERFACE  2

#define DTI_MAX_PROPERTIES      30

#define DTIRxBufLength 128
#define DTITxBufLength 128

#define ACCESS_TIMEOUT  (100*60*10)     //10 min

extern DTI_Property* DTI_PropList[DTI_MAX_PROPERTIES];
extern uint8_t DTI_PropPtr;

extern volatile bool DTI_Enable;
extern const DTI_INFO DTI_info;
extern uint16_t access_timer;
void DTI_Init();
//uint16_t DTI_GetC(void);
void DTI_RxCommFix(char c);
//void DTI_RxCommFix_DL(char c);
//int16_t DTI_GetComm(uint8_t* buf,uint16_t maxnum);
//uint8_t DTI_GetChar(uint8_t* Mas);
//uint8_t DTI_CalcLRC(uint8_t* Mas, uint8_t Num);
//uint8_t HexAsciiToInt(uint8_t a, uint8_t b);
//void SendHexChar(uint8_t c);
//uint8_t crc_summ(uint8_t d);
void DTI_Poll();
void DTI_Timer_Dispatcher();
void DTI_Poll_cb(SLIP_BUF * slip);
//void DTI_Poll_cb(uint8_t cmd, uint8_t *data, uint8_t len, uint8_t dti_interface);

bool DTI_regProperty(const uint8_t *name, uint8_t type, uint32_t add, uint16_t parent);
bool DTI_getProperty_byName(uint8_t *name_str, DTI_Property *p);
bool DTI_getProperty_byID(uint16_t id, DTI_Property *p);
void DTI_runEvent(uint32_t event);
//void DTI_set_Interface_no485();

#endif // __DTI_H