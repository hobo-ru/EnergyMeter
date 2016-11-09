#ifndef _LEUART_H_
#define _LEUART_H_


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>



extern void LeUART_Init( void );
extern void LeUART_Deinit( void );


extern void LeUART_StartTransmitt( void );
extern void LeUART_StopTransmitt( void );


extern size_t LeUART_WriteTxBuffer( const uint8_t *data, size_t lenght );
extern size_t LeUART_SendCmd( uint8_t cmd, const uint8_t *data, size_t lenght );

extern void LeUART_SendManually( void );

extern void LeUART_TxDisable( void );
extern void LeUART_TxEnable( void );

extern void LeUART_SendCalibr(uint32_t ade_watt, uint32_t ade_amp, uint32_t ade_volt, uint32_t merc_imp);

#endif
