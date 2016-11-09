#ifndef _LEUART_H_
#define _LEUART_H_


#define RxBufLength 32
#define TxBufLength 128

extern uint8_t RxBuf[RxBufLength];
extern uint8_t TxBuf[TxBufLength];
extern uint8_t RxPtr;
extern uint8_t ReadPtr;
extern uint8_t TxPtr;
extern uint8_t WritePtr;

typedef void    (*LeUART_CALLBACK)(char);

extern volatile LeUART_CALLBACK LeUART_RxCallback;


void LeUART_Init( void );
void LeUART_PutChar(uint8_t c);
uint16_t LeUART_GetChar(void);
void LeUART_SetRxCallback(LeUART_CALLBACK cb);
void LeUART_DeInit( void );


#endif
