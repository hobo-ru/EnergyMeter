#include "dti.h"
#include "archive.h"
#include "em_usart.h"
#include "leuart.h"
#include "at25sf041.h"
#include "modem.h"
#include "modem_uart.h"
#include "electro_v5.h"
#include "pwr_managment.h"

#define DTI_RAM_RO_ADD    0
#define DTI_RAM_RO_LEN    8192

#define DTI_RAM_RW_ADD    0x2000 // DTI_RAM_RO_ADD + DTI_RAM_RO_LEN
#define DTI_RAM_RW_LEN    336

#define DTI_EE_RO_ADD     0x2150 //DTI_RAM_RW_ADD+DTI_RAM_RW_LEN
#define DTI_EE_RO_LEN     65536

#define DTI_EE_RW_ADD     0x12150 //DTI_EE_RO_ADD+DTI_EE_RO_LEN
#define DTI_EE_RW_LEN     524288  //4 MBit EEPROM

//#define DTI_DEFAULT_ID  0

bool volatile DTI_Enable = 1;

//uint8_t DTI_Interface;

uint8_t DTIRxBuf[DTIRxBufLength];
uint8_t DTITxBuf[DTITxBufLength];
uint8_t DTIRxPtr = 0;
uint8_t DTIReadPtr = 0;
uint8_t DTITxPtr = 0;
uint8_t DTIWritePtr = 0;
uint16_t access_timer = 0;


DTI_Property* DTI_PropList[DTI_MAX_PROPERTIES];

uint8_t DTI_PropPtr = 0;

const DTI_INFO DTI_info = 
{
  "Amper100",                   //Device  name
  1,                            //Hardware ID
  120,                          //Hardware rev
  1,                            //Software rev
  01,                           //Software sub rev
  50,                           //Max read/write data len
  DTI_RAM_RO_ADD,               //Read only RAM start address
  DTI_RAM_RW_ADD,               //Read and write RAM start address
  DTI_EE_RO_ADD,                //Read only EEPROM start address(Programm memory)
  DTI_EE_RW_ADD,                //Read and write EEPROM start address(AT25SF041)
  DTI_EE_RW_ADD + DTI_EE_RW_LEN //End of memory
}; 

uint8_t DTI_RX_isEmpty(void)
{
    return (DTIRxPtr == DTIReadPtr ? 1 : 0);
}

uint8_t DTI_GetRX(void)
{
    return DTIRxBuf[DTIReadPtr++%DTIRxBufLength];
}

void DTI_RX_Clear(void)
{
    DTIReadPtr = DTIRxPtr;
}

SLIP_BUF slip_DTI_rx;

void DTI_Init()                 
{
    LeUART_SetRxCallback(&DTI_RxCommFix);
    
    DTI_regProperty("DTI_list", STRUCT_TYPE, (uint32_t)DTI_PropList, 0);
    DTI_regProperty("DTI_num", UINT8_TYPE , (uint32_t)&DTI_PropPtr, 0);
    DTI_regProperty("Event_run", FUNCTION_TYPE , (uint32_t)DTI_runEvent, 0);
    DTI_regProperty("Erase_4k", FUNCTION_TYPE , (uint32_t)FLASH_EraseBlock_4K, 0);
    DTI_regProperty("Erase_64k", FUNCTION_TYPE , (uint32_t)FLASH_EraseBlock_64K, 0);
    DTI_regProperty("Settings", STRUCT_TYPE , (uint32_t)&Settings, 0);
    
    memset(&slip_DTI_rx, 0, sizeof(SLIP_BUF));
    slip_DTI_rx.dti_interface = LEUART_INTERFACE;
    slip_DTI_rx.grx = DTI_GetRX;
    slip_DTI_rx.grx_isEpmty = DTI_RX_isEmpty;
}

void DTI_PutChar(uint8_t c)
{
  DTITxBuf[DTIWritePtr++%DTITxBufLength]=c;
}

void DTI_RxCommFix(char c)
{
  DTIRxBuf[DTIRxPtr++%DTIRxBufLength] = c;
//  DTI_Interface = LEUART_INTERFACE;
}

void DTI_Poll_cb(SLIP_BUF * slip)
//void DTI_Poll_cb(uint8_t cmd, uint8_t *data, uint8_t len, uint8_t dti_interface)
{
  uint16_t DTI_comm;
  uint32_t DTI_add;
  uint16_t DTI_len;
  uint8_t *DTI_ptr;
  uint8_t dev_add;
  
  uint8_t cmd; uint8_t *data; uint8_t len; uint8_t dti_interface;
  cmd = slip->cmd;
  data = slip->payload;
  len = slip->len;
  dti_interface = slip->dti_interface;
      
  DTI_Property p;

  if (slip_CRC8(data, len - 1) != data[len-1]) return;
  if (cmd != AXEM_CMD_DL_PACKET && cmd != AXEM_CMD_SEND_FAST_DL) return;
  cmd = *data++;
  if (cmd != EL_V5_DTI) return;
  
  uint8_t buf[128];
  int16_t i;
  uint8_t name[10]; //buffer for property name and  password
  
  dev_add = *data++;
  if((dev_add != Settings.DTI_address)&&(dev_add != 0xff))  return;
  DTI_comm = *data++;                                                  //get command #
  if(access_timer) access_timer = ACCESS_TIMEOUT;
  if(DTI_comm < 3)
  {
    DTI_add = data[0]+(data[1]<<8)+(data[2]<<16);   //get add
    DTI_len = data[3]+(data[4]<<8);                 //get len
  }
  DTIWritePtr = 0;
  DTITxPtr = 0;
  DTI_PutChar(EL_V5_DTI);
  DTI_PutChar(Settings.DTI_address);
  switch(DTI_comm)
  {
  case 0:
    DTI_ptr = (unsigned char*)(&DTI_info);
    DTI_PutChar(DTI_comm); 
    DTI_PutChar(DTI_len&0xff); 
    DTI_PutChar(DTI_len>>8);
    for(i=0;i!=DTI_len;i++) DTI_PutChar(DTI_ptr[i]);
    break;
  case 01:              //read  data
    if(DTI_add < DTI_RAM_RW_ADD) DTI_ptr = ((uint8_t*)DTI_add) + 0x20000000;                         //Read from RO RAM
    //else if(DTI_add < DTI_EE_RO_ADD)  DTI_ptr = &DTI_buf[DTI_add-DTI_RAM_RW_ADD];                    //read from RW RAM
    else if(DTI_add < DTI_EE_RW_ADD ) {break;}                                      //read from RO EEPROM  //not supported yet
    else if(DTI_add < DTI_EE_RW_ADD + DTI_EE_RW_LEN )                                                //read from RW EEPROM
    {
        FLASH_Init_SPI();
        FLASH_ReadBuffer(DTI_add - DTI_EE_RW_ADD , buf, DTI_len);
        DTI_ptr = buf;
    }        
    else {DTI_PutChar(0x83);  break;}                                         //OUT_OF_MEMORY
    DTI_PutChar(DTI_comm); 
    DTI_PutChar(DTI_len&0xff); 
    DTI_PutChar(DTI_len>>8);
    for(i=0;i!=DTI_len;i++) DTI_PutChar(DTI_ptr[i]);
    break;
  case 02:               //write data
    if(access_timer == 0)       
    {
      DTI_PutChar(0x86);      //access denied
      break;
    }
    if(DTI_add < DTI_RAM_RW_ADD) 
    {
        DTI_ptr = ((uint8_t*)DTI_add) + 0x20000000;
        for(i=0;i!=DTI_len;i++) DTI_ptr[i] = data[i+5];//DTI_GetChar(0);
    }                 
    else if(DTI_add < DTI_EE_RO_ADD)                                                    //Write to RW RAM
    {
    }
    else if(DTI_add < DTI_EE_RW_ADD ) {DTI_PutChar(0x82);  break;}            //Wtite protect
    else if(DTI_add < DTI_EE_RW_ADD + DTI_EE_RW_LEN )                                   //write to RW EEPROM
    {
      FLASH_Init_SPI();
      for(i=0;i!=DTI_len;i++) buf[i] = data[i+5];//DTI_GetChar(0);        
      FLASH_WriteBuffer(DTI_add - DTI_EE_RW_ADD , buf, DTI_len);  
    }        
    else {DTI_PutChar(0x83);  break;}                                         //OUT_OF_MEMORY
    DTI_PutChar(2); 
    break;
  case 03:              //get property by name
    for(int i = 0; i != 10 ; i++) name[i] = data[i];//DTI_GetChar(0);
    if(!DTI_getProperty_byName(name, &p)) {DTI_PutChar(0);break;}          //unknown property
    DTI_PutChar(DTI_comm); 
    if((p.type & 0xc0) == SEGMENT_RAM_RO)  p.address &= 0xfffffff;     
    DTI_PutChar(sizeof(DTI_Property)); 
    for(i=0;i!=sizeof(DTI_Property);i++) DTI_PutChar(((uint8_t *)&p)[i]);
    break;
  case 04:              //run func   
    if(access_timer == 0)       
    {
      DTI_PutChar(0x86);      //access denied
      break;
    }
    uint16_t id;
    void (*func)(uint32_t);
    uint32_t param;    
    for(int i = 0; i != 2; i++) ((uint8_t*)(&id))[i] = data[i];
    for(int i = 0; i != 4; i++) ((uint8_t*)&param)[i] = data[i+2];
    if(DTI_getProperty_byID(id, &p)) 
    {
      *((uint32_t*)&func) = p.address;
      func(param);
    }
    DTI_PutChar(4);
    break;
  case 05:       //log in
    for(int i = 0; i != 10 ; i++) name[i] = data[i];//DTI_GetChar(0);
    if(strcmp((char const *)(Settings.Password), (char const *)name))
    {
       DTI_PutChar(0x85);             //password not accepted
       access_timer = 0;
       break; 
    }  
    access_timer = ACCESS_TIMEOUT;
    DTI_PutChar(5);                   //password accepted
    break;
  default:
    DTI_PutChar(0x81);  
    break;
    
  }
  DTI_PutChar(slip_CRC8(DTITxBuf, DTIWritePtr));
  while (DTIWritePtr < 8) DTI_PutChar(0xFF);
  switch (dti_interface) {
  case LEUART_INTERFACE:        ModemUART_SLIP_Send_F(LeUART_PutChar,  AXEM_CMD_SEND_NBFi, DTITxBuf, DTIWritePtr); break;
  case AX_FAST_DL_INTERFACE:    ModemUART_SLIP_Send_F(ModemUART_PutTx, AXEM_CMD_SEND_NBFi, DTITxBuf, DTIWritePtr); break;
  case OPTO_FAST_DL_INTERFACE:  ModemUART_SLIP_Send_F(OptoUART_PutTx,  AXEM_CMD_SEND_NBFi, DTITxBuf, DTIWritePtr); break;
  }
}

void DTI_Poll()
{
  if (!DTI_Enable) {
      return;
  }
  if (!PwrManager_GetStatus()) {
//      ModemUART_Clear();
      DTI_RX_Clear();
      return;
  }
  ModemUART_SLIP_Dispatcher(&slip_DTI_rx, DTI_Poll_cb);
  ModemUART_SLIP_Dispatcher(&opto_slip_rxbuf, DTI_Poll_cb);
}

void DTI_Timer_Dispatcher()
{
    if (access_timer) access_timer--;
}

bool DTI_regProperty(const uint8_t *name, uint8_t type, uint32_t add, uint16_t parent)
{
    static uint16_t id_cntr = 1;
    DTI_Property *p = malloc(sizeof(DTI_Property));
    if(p == 0) return false;   
    for(int i = 0; i != 10; i++) p->name[i] = name[i];
    p->id = id_cntr++;
    p->address = add;
    p->parent_id = parent;
    p->type = type;
    DTI_PropList[DTI_PropPtr++%DTI_MAX_PROPERTIES] = p;
    return true;
}
 
bool DTI_getProperty_byName(uint8_t *name_str, DTI_Property *p)
{

  uint8_t i;
  for(i = 0; i!= DTI_PropPtr; i++)
  {
    if(!strcmp((char const *)(DTI_PropList[i]->name), (char const *)name_str) && !DTI_PropList[i]->parent_id) break;  
  }
  if(i == DTI_PropPtr) return false;

  memcpy(p, DTI_PropList[i], sizeof(DTI_Property));
  
  return true;
}

bool DTI_getProperty_byID(uint16_t id, DTI_Property *p)
{
  
  uint8_t i;
  
  for(i = 0; i!= DTI_PropPtr; i++)
  {
    if(DTI_PropList[i]->id == id) break;  
  }
  if(i == DTI_PropPtr) return false;

  memcpy(p, DTI_PropList[i], sizeof(DTI_Property));
  
  return true;
}

void DTI_runEvent(uint32_t event)
{
  mQueue_PutMsg( (mQueueType_e)event, 0 );
}

//void DTI_set_Interface_no485()
//{
//    DTI_Interface = AX_FAST_DL_INTERFACE;
//}
