#ifndef _MODEM_UART_H_
#define _MODEM_UART_H_

#include <stdbool.h>
#include <stdint.h>
//#include <stdio.h>

#define SLIP_START  0xDD
#define SLIP_END    0xDE
#define SLIP_ESC    0xDF

#define SLIP_PAYLOAD_SIZE (128)
//#define SLIP_PAYLOAD_SIZE (256)

typedef uint8_t (*SLIP_BUF_GET_RX)(void);
typedef void (*SLIP_BUF_PUT_TX)(uint8_t);

typedef struct {
    SLIP_BUF_GET_RX grx_isEpmty;
    SLIP_BUF_GET_RX grx;
    uint8_t mode;
    uint8_t len;
    uint8_t cmd;
    uint8_t dti_interface;
    uint8_t payload[SLIP_PAYLOAD_SIZE];
} SLIP_BUF;

extern SLIP_BUF modem_slip_rxbuf;
extern SLIP_BUF opto_slip_rxbuf;

uint8_t slip_CRC8(uint8_t * data, uint8_t len);

void ModemUART_Init(void); 
void ModemUART_Deinit(void);

void ModemUART_Clear(void);
uint8_t ModemUART_GetRx(void);
uint8_t ModemUART_RxIsEmpty(void);
void ModemUART_PutTx(uint8_t data);

void OptoUART_Init(void); 
void OptoUART_Deinit(void);

void OptoUART_Clear(void);
uint8_t OptoUART_GetRx(void);
uint8_t OptoUART_RxIsEmpty(void);
void OptoUART_PutTx(uint8_t data);

//typedef void (*ModemUART_Dispatcher_cb)(uint8_t cmd, uint8_t *data, uint8_t len, uint8_t dti_interface);
typedef void (*ModemUART_Dispatcher_cb)(SLIP_BUF *slip);
void ModemUART_SLIP_Dispatcher(SLIP_BUF *sb, ModemUART_Dispatcher_cb md_cb);

void ModemUART_SLIP_Send_F(SLIP_BUF_PUT_TX ptx, uint8_t cmd, uint8_t *payload, uint8_t len);

#endif
