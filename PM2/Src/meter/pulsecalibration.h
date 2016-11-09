#ifndef PULSECALIBRATION_H
#define PULSECALIBRATION_H

#include <stdint.h>
#include <stdbool.h>


// xWGAIN Pulse Calibration
//
//  Mercuri
//  |_|_|_|A+|_|
//   |_|_|_|A-|_|  
//
//
//  Amper Meter
//  |A+,p|_|A-|g|_|_|
//
//
//  Power DC
//  |p|g|
//

// xVARGAIN Pulse Calibration
//
//  Mercuri
//  |_|_|_|_|R+|
//   |_|_|_|_|R-|
//
//
//  Meter
//  |R+,p|_|R-|g|_|_|
//
//
//  Power DC
//  |p|g|
//


//
//  | 220V | 220_N_ |
//
//  Amper Meter
//  | 220V | Merc_220V | 220_N_ | Merc_N |
//
//  Mercuri
//  |  |  |  |  | Merc_220V | --ac-- | Merc_N | --n--  |
//
//  | --ac-- | --n--  |


extern void Calibration_Init( bool enable );
extern void Calibration_Deinit( void );

extern  uint32_t Calibration_GetPulse( void );
extern uint32_t Calibration_Calculate( uint32_t param );
extern bool Calibration_IsNeedCalibration( void );
extern void Calibration_StartNew( void );

extern  uint32_t Calibration_GetAccum( void );


#endif