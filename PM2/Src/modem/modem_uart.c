#include "modem_uart.h"

#include "em_device.h"
#include "em_gpio.h"
#include "em_chip.h"
#include "em_usart.h"
#include "em_cmu.h"
//#include "main.h"
//#include "mqueue.h"

#include "defines_HW.h"
#include "triggersevents.h"
#include "systimer.h"
#include "dti.h"

#define MODEM_UART		USART1
#define MODEM_UART_TX_IRQN	USART1_TX_IRQn
#define MODEM_UART_RX_IRQN	USART1_RX_IRQn

#define MODEM_UART_CMU_CLOCK	cmuClock_USART1
#define MODEM_UART_RX_IRQ	USART1_RX_IRQHandler
#define MODEM_UART_TX_IRQ	USART1_TX_IRQHandler

#define OPTO_UART           USART0
#define OPTO_UART_TX_IRQN   USART0_TX_IRQn
#define OPTO_UART_RX_IRQN   USART0_RX_IRQn

#define OPTO_UART_CMU_CLOCK	cmuClock_USART0
#define OPTO_UART_RX_IRQ	USART0_RX_IRQHandler
#define OPTO_UART_TX_IRQ	USART0_TX_IRQHandler

//#define UART_BUFFER_SIZE (70)
#define UART_BUFFER_SIZE (140)

typedef struct {
    uint8_t buf[UART_BUFFER_SIZE];
    uint8_t wr;
    uint8_t rd;
    uint8_t len;
} UART_X_BUF;

UART_X_BUF mu_RX;
UART_X_BUF mu_TX;

UART_X_BUF ou_RX;
UART_X_BUF ou_TX;

enum {
    SLIP_MODE_START,
    SLIP_MODE_RECEIVING_PAYLOAD,
    SLIP_MODE_RECEIVING_CMD,
    SLIP_MODE_ESC,
};

SLIP_BUF modem_slip_rxbuf;
SLIP_BUF opto_slip_rxbuf;

#define use_crc (1)

//********************************************************
//* RX ***************************************************
//********************************************************

void ModemUART_Clear(void)
{
    mu_RX.rd = mu_RX.wr;
    mu_RX.len = 0;
}

uint8_t ModemUART_RxIsEmpty(void)
{
    return (mu_RX.wr == mu_RX.rd ? 1 : 0);
}

uint8_t ModemUART_GetRx(void)
{
	if (ModemUART_RxIsEmpty()) return 0;
    uint8_t data = mu_RX.buf[mu_RX.rd];
    if (++mu_RX.rd >= UART_BUFFER_SIZE) {
        mu_RX.rd = 0;
    }
    return data;
}

void ModemUART_PutTx(uint8_t data)
{
    mu_TX.buf[mu_TX.wr] = data;
    if (++mu_TX.wr >= UART_BUFFER_SIZE) {
        mu_TX.wr = 0;
    }
    mu_TX.len++;
    USART_IntEnable( MODEM_UART, USART_IEN_TXBL );
}

void ModemUART_Init( void )
{
	USART_InitAsync_TypeDef uartInit =
	{
		.enable       = usartDisable,   // Wait to enable the transmitter and receiver
		.refFreq      = 0,              // Setting refFreq to 0 will invoke the CMU_ClockFreqGet() function and measure the HFPER clock
		.baudrate     = 115200,          // Desired baud rate
		.oversampling = usartOVS16,     // Set oversampling value to x16
		.databits     = usartDatabits8, // 8 data bits
		.parity       = usartNoParity,  // No parity bits
		.stopbits     = usartStopbits1, // 1 stop bit
		.mvdis        = false,          // Use majority voting
		.prsRxEnable  = false,          // using PRS input
		.prsRxCh      = usartPrsRxCh0,  // Doesn't matter which channel we select
	};

    memset(&mu_RX, 0, sizeof(UART_X_BUF));
    memset(&mu_TX, 0, sizeof(UART_X_BUF));
    memset(&modem_slip_rxbuf, 0, sizeof(SLIP_BUF));
    modem_slip_rxbuf.dti_interface = AX_FAST_DL_INTERFACE;
    modem_slip_rxbuf.grx = ModemUART_GetRx;
    modem_slip_rxbuf.grx_isEpmty = ModemUART_RxIsEmpty;

	CMU_ClockEnable(MODEM_UART_CMU_CLOCK, true);
	CMU_ClockEnable (cmuClock_GPIO, true);

    gpio_INI_AXEM_TX;
    gpio_INI_AXEM_RX;

	USART_InitAsync(MODEM_UART, &uartInit);                                              // Apply configuration struct to USART1
	MODEM_UART->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC2; // Clear RX/TX buffers and shift regs, enable transmitter and receiver pins
	USART_IntClear(MODEM_UART, _USART_IF_MASK); // Clear any USART interrupt flags
    USART_IntEnable(MODEM_UART, USART_IEN_RXDATAV );

	NVIC_ClearPendingIRQ(MODEM_UART_RX_IRQN);   // Clear pending RX interrupt flag in NVIC
	NVIC_ClearPendingIRQ(MODEM_UART_TX_IRQN);   // Clear pending TX interrupt flag in NVIC

	NVIC_EnableIRQ(MODEM_UART_RX_IRQN);
	NVIC_EnableIRQ(MODEM_UART_TX_IRQN);
    
//	ModemUART_SetTransceiverMode(UART_NO_TRANSMITT_INTERRUPT);
        
	USART_Enable(MODEM_UART, usartEnable);     // Enable transmitter and receiver
}

void ModemUART_Deinit(void)
{
    gpio_OFF_AXEM_TX;
    gpio_OFF_AXEM_RX;

	USART_IntClear( MODEM_UART, _USART_IF_MASK ); // Clear any USART interrupt flags
	USART_Enable( MODEM_UART, usartDisable );     // Enable transmitter and receiver

	NVIC_ClearPendingIRQ( MODEM_UART_TX_IRQN );   // Clear pending RX interrupt flag in NVIC
	NVIC_ClearPendingIRQ( MODEM_UART_TX_IRQN );   // Clear pending TX interrupt flag in NVIC

	NVIC_DisableIRQ( MODEM_UART_TX_IRQN );
	NVIC_DisableIRQ( MODEM_UART_TX_IRQN );
	CMU_ClockEnable( MODEM_UART_CMU_CLOCK, false );
//	ReceiveCallback = NULL;
}

void OptoUART_Clear(void)
{
    ou_RX.rd = ou_RX.wr;
    ou_RX.len = 0;
}

uint8_t OptoUART_RxIsEmpty(void)
{
    return (ou_RX.wr == ou_RX.rd ? 1 : 0);
}

uint8_t OptoUART_GetRx(void)
{
	if (OptoUART_RxIsEmpty()) return 0;
    uint8_t data = ou_RX.buf[ou_RX.rd];
    if (++ou_RX.rd >= UART_BUFFER_SIZE) {
        ou_RX.rd = 0;
    }
    return data;
}

void OptoUART_PutTx(uint8_t data)
{
    ou_TX.buf[ou_TX.wr] = data;
    if (++ou_TX.wr >= UART_BUFFER_SIZE) {
        ou_TX.wr = 0;
    }
    ou_TX.len++;
#ifdef OPTO_PORT    
    USART_IntEnable( OPTO_UART, USART_IEN_TXBL );
#endif //OPTO_PORT
}

void OptoUART_Init( void )
{
#ifdef OPTO_PORT    
	USART_InitAsync_TypeDef uartInit =
	{
		.enable       = usartDisable,   // Wait to enable the transmitter and receiver
		.refFreq      = 0,              // Setting refFreq to 0 will invoke the CMU_ClockFreqGet() function and measure the HFPER clock
		.baudrate     = 9600,           // Desired baud rate
		.oversampling = usartOVS16,     // Set oversampling value to x16
		.databits     = usartDatabits8, // 8 data bits
		.parity       = usartNoParity,  // No parity bits
		.stopbits     = usartStopbits1, // 1 stop bit
		.mvdis        = false,          // Use majority voting
		.prsRxEnable  = false,          // using PRS input
		.prsRxCh      = usartPrsRxCh0,  // Doesn't matter which channel we select
	};
#endif //OPTO_PORT
    memset(&ou_RX, 0, sizeof(UART_X_BUF));
    memset(&ou_TX, 0, sizeof(UART_X_BUF));
    memset(&opto_slip_rxbuf, 0, sizeof(SLIP_BUF));
    opto_slip_rxbuf.dti_interface = OPTO_FAST_DL_INTERFACE;
    opto_slip_rxbuf.grx = OptoUART_GetRx;
    opto_slip_rxbuf.grx_isEpmty = OptoUART_RxIsEmpty;

#ifdef OPTO_PORT
	CMU_ClockEnable(OPTO_UART_CMU_CLOCK, true);
	CMU_ClockEnable (cmuClock_GPIO, true);

    gpio_INI_RSopto_TX;
    gpio_INI_RSopto_RX;

	USART_InitAsync(OPTO_UART, &uartInit);                                              // Apply configuration struct to USART1
	OPTO_UART->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC5; // Clear RX/TX buffers and shift regs, enable transmitter and receiver pins
	USART_IntClear(OPTO_UART, _USART_IF_MASK); // Clear any USART interrupt flags
    USART_IntEnable(OPTO_UART, USART_IEN_RXDATAV );

	NVIC_ClearPendingIRQ(OPTO_UART_RX_IRQN);   // Clear pending RX interrupt flag in NVIC
	NVIC_ClearPendingIRQ(OPTO_UART_TX_IRQN);   // Clear pending TX interrupt flag in NVIC

	NVIC_EnableIRQ(OPTO_UART_RX_IRQN);
	NVIC_EnableIRQ(OPTO_UART_TX_IRQN);
    
//	ModemUART_SetTransceiverMode(UART_NO_TRANSMITT_INTERRUPT);
        
	USART_Enable(OPTO_UART, usartEnable);     // Enable transmitter and receiver
#endif //OPTO_PORT
}

void OptoUART_Deinit(void)
{
#ifdef OPTO_PORT 
    gpio_OFF_RSopto_TX;
    gpio_OFF_RSopto_RX;

	USART_IntClear( OPTO_UART, _USART_IF_MASK ); // Clear any USART interrupt flags
	USART_Enable( OPTO_UART, usartDisable );     // Enable transmitter and receiver

	NVIC_ClearPendingIRQ( OPTO_UART_TX_IRQN );   // Clear pending RX interrupt flag in NVIC
	NVIC_ClearPendingIRQ( OPTO_UART_TX_IRQN );   // Clear pending TX interrupt flag in NVIC

	NVIC_DisableIRQ( OPTO_UART_TX_IRQN );
	NVIC_DisableIRQ( OPTO_UART_TX_IRQN );
	CMU_ClockEnable( OPTO_UART_CMU_CLOCK, false );
//	ReceiveCallback = NULL;
#endif //OPTO_PORT
}

void UART_RX_IRQ(UART_X_BUF * xbuf, USART_TypeDef * usart) 
{
    if (usart->STATUS & USART_STATUS_RXDATAV) {
        xbuf->buf[xbuf->wr] = usart->RXDATA;
        if (++xbuf->wr >= UART_BUFFER_SIZE) {
            xbuf->wr = 0;
        }
        xbuf->len++;
    }
}

void UART_TX_IRQ(UART_X_BUF * xbuf, USART_TypeDef * usart)
{
    if (usart->STATUS & USART_STATUS_TXBL) {       //USART_STATUS_TXBL
        if (xbuf->wr != xbuf->rd) {
            usart->TXDATA = xbuf->buf[xbuf->rd];// muart_txbuf[mUART_TX_tail++] );
            if (++xbuf->rd >= UART_BUFFER_SIZE) {
                xbuf->rd = 0;
            }
            xbuf->len--;
        } else {
           USART_IntDisable( usart, USART_IEN_TXBL ); 
        }
    }
}

void MODEM_UART_RX_IRQ(void)
{
    UART_RX_IRQ(&mu_RX, MODEM_UART);
//    if (MODEM_UART->STATUS & USART_STATUS_RXDATAV) {
//        mu_RX.buf[mu_RX.wr] = MODEM_UART->RXDATA;
//        if (++mu_RX.wr >= UART_BUFFER_SIZE) {
//            mu_RX.wr = 0;
//        }
//        mu_RX.len++;
//    }
}

void MODEM_UART_TX_IRQ(void)
{
    UART_TX_IRQ(&mu_TX, MODEM_UART);
//    if (MODEM_UART->STATUS & USART_STATUS_TXBL) {       //USART_STATUS_TXBL
//        if (mu_TX.wr != mu_TX.rd) {
//            MODEM_UART->TXDATA = mu_TX.buf[mu_TX.rd];// muart_txbuf[mUART_TX_tail++] );
//            if (++mu_TX.rd >= UART_BUFFER_SIZE) {
//                mu_TX.rd = 0;
//            }
//            mu_TX.len--;
//        } else {
//           USART_IntDisable( MODEM_UART, USART_IEN_TXBL ); 
//        }
//    }
}

void OPTO_UART_RX_IRQ(void)
{
    UART_RX_IRQ(&ou_RX, OPTO_UART);
}

void OPTO_UART_TX_IRQ(void)
{
    UART_TX_IRQ(&ou_TX, OPTO_UART);
}

static uint8_t slip_CRC8byte(uint8_t data)
{
   uint8_t crc = 0;
   if(data & 1)     crc ^= 0x5e;
   if(data & 2)     crc ^= 0xbc;
   if(data & 4)     crc ^= 0x61;
   if(data & 8)     crc ^= 0xc2;
   if(data & 0x10)  crc ^= 0x9d;
   if(data & 0x20)  crc ^= 0x23;
   if(data & 0x40)  crc ^= 0x46;
   if(data & 0x80)  crc ^= 0x8c;
   return crc;
}

uint8_t slip_CRC8(uint8_t * data, uint8_t len)
{
    uint8_t crc = 0;
    for (uint8_t i = 0; i < len; i++) {
        crc = slip_CRC8byte(data[i] ^ crc);
    }
    return crc;
}

void ModemUART_SLIP_Dispatcher(SLIP_BUF *sb, ModemUART_Dispatcher_cb md_cb)
{
    while (!sb->grx_isEpmty()) {
        uint8_t c = sb->grx();
        switch (sb->mode) {
        case  SLIP_MODE_START: {
            if (c == SLIP_START) {
                sb->len = 0;
                memset(sb->payload, 0, SLIP_PAYLOAD_SIZE);
                sb->mode = SLIP_MODE_RECEIVING_CMD;
            }
            break;
        }
        case SLIP_MODE_RECEIVING_PAYLOAD:
        case SLIP_MODE_RECEIVING_CMD: {
            if (c == SLIP_ESC) {
                sb->mode = SLIP_MODE_ESC;
                break;
            }
            if (c == SLIP_START) {
                sb->len = 0;
                memset(sb->payload, 0, SLIP_PAYLOAD_SIZE);
                sb->mode = SLIP_MODE_RECEIVING_CMD;
                break;
            }
            if (c == SLIP_END) {
                sb->mode = SLIP_MODE_START;
                if (use_crc) {
                    sb->len--;
                    if (slip_CRC8(&sb->payload[0], sb->len) != sb->payload[sb->len]) {
                        break;
                    }
                }
                if (md_cb) {
                    md_cb(sb);
//                    md_cb(sb->cmd, sb->payload, sb->len, sb->dti_interface);
                }
                break;
            }
            if (sb->mode == SLIP_MODE_RECEIVING_CMD) {
                sb->cmd = c;
                sb->mode = SLIP_MODE_RECEIVING_PAYLOAD;
                break;
            }
            if (sb->mode == SLIP_MODE_RECEIVING_PAYLOAD) {
                sb->payload[sb->len++] = c;
                break;
            }
            break;
        }
        case SLIP_MODE_ESC: {
            sb->payload[sb->len++] = c ^ 0xFF;
            sb->mode = SLIP_MODE_RECEIVING_PAYLOAD;
            break;
        }
        default: {
            sb->mode = SLIP_MODE_START;
            break;
        }
        }

        if (sb->len == SLIP_PAYLOAD_SIZE - 1) {
            sb->mode = SLIP_MODE_START;
        }
    }
}

void ModemUART_SLIP_Send_F(SLIP_BUF_PUT_TX ptx, uint8_t cmd, uint8_t *payload, uint8_t len)
{
    ptx(SLIP_START);
//    {uint32_t dly = 1000*1000; while(dly) dly--;}
    DelayMs(10);
    ptx(SLIP_START);
    ptx(cmd);
    uint16_t i;
    uint8_t c;
    for (i = 0; i <= len; i++) {
        if (i == len) {
            if (use_crc) {
                c = slip_CRC8(payload, len);
            } else {
                break;
            }
        } else {
            c = payload[i];
        }

        if ((c == SLIP_START) || (c == SLIP_END) || (c == SLIP_ESC)) {
            ptx(SLIP_ESC);
            ptx(c ^ 0xFF);
        } else {
            ptx(c);
        }
    }
    ptx(SLIP_END);
}
