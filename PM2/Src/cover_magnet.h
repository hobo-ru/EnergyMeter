#ifndef _COVER_MAGNET_H_
#define _COVER_MAGNET_H_


#include <stdbool.h>
#include <stdint.h>


void CoverMagnet_Init(void);
void CoverMagnet_Deinit(void);


extern void Magnet_Init(void);
extern void Magnet_Deinit(void);
extern bool Magnet_IsCloser(void);


extern void Cover_Init(void);
extern void Cover_Deinit(void);
extern bool Cover_IsOpen(void);




#endif
