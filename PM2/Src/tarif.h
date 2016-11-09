#ifndef TARIF_H
#define TARIF_H



#include <stdbool.h>
#include <stdint.h>
#include <time.h>



//typedef enum {
//  EMPTY_RATE            = 0,
//  NIGHT_ZONE_RATE       = 0x10000001,   //T1
//  PEAK_ZONE_RATE        = 0x10000002,   //T2  
//  HALF_PEAK_ZONE_RATE   = 0x10000003,   //T3    
//  TOTAL_RATE            = 0x10000004,   //Total
//}RateZoneType_e;
//
//#define TITLE_NAME_LEN  8
//typedef struct {
//  RateZoneType_e    type;
//  uint8_t       name[TITLE_NAME_LEN];
//}TarifTitle_s;
//#define TARIF_TITLE_SIZE        ( sizeof(RateZoneType_e) + ( TITLE_NAME_LEN * sizeof(uint8_t) ) + sizeof(uint8_t) )
//
//
//typedef struct {
//  time_t	start;	// Time of start, two elements in case one tarif happens twice a day
//  time_t	end;	// End time
//}TimeZone_s;
//
//
//#define TARIF_ZONE_SIZE         ( sizeof(RateZoneType_e) + sizeof(time_t) + sizeof(uint8_t) )
//
//typedef struct {
//  RateZoneType_e    title;
//  TimeZone_s    time_zone;
//}Tarif_s;


//extern TarifTitle_s *pTitle;             // описание тарифа
//extern Tarif_s *pTarif;                  // тарифная сетка времени
//extern uint64_t TarifBitMask[];

//extern void Tarif_Init( void );
//extern void Tarif_Deinit( void );

//extern RateZoneType_e Tarif_GetCurrent( time_t time );
//extern RateZoneType_e Tarif_GetAbsolute( time_t time );
//extern const TarifTitle_s *Tarif_GetTitle( uint8_t title );
//extern const Tarif_s *Tarif_GetSchedule( uint8_t schedule );

//void Tarif_SaveTarif( void );

//extern void Tarif_SetDefault( void );
//extern void Tarif_FillTarifSchedule( void );
//extern void Tarif_StartNewTarif( void );

//extern void Tarif_SetNewTarif( uint8_t tarif, uint8_t hour, bool hhour );
//extern void Tarif_StoreNewTarif( void );

//void Tarifs2bits();
//void Bits2Tarifs();

//uint8_t Tarif_index(RateZoneType_e tarif);
//uint8_t Tarif_timestamp(uint32_t time);

#endif
