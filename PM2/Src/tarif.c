#include "tarif.h"
#include "timeevents.h"
#include "archive.h"
#include "at25sf041.h"

#include "dti.h"

#include "tariff.h"
#include "meter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//TarifTitle_s *pTitle = NULL;             // описание тарифа
//Tarif_s *pTarif = NULL;                  // тарифная сетка времени

//uint64_t TarifBitMask[4];

//void Tarifs2bits()
//{
//    memset((void *)(&TarifBitMask[0]), 0, sizeof(TarifBitMask));
//  
//    uint8_t tariff;
//    for(uint8_t i = 0; i <= 48; i++) {
//        tariff = TARIFF_from_time(i * 60 * 30);
//        TarifBitMask[tariff - 1] = (((uint64_t)1) << i);  
//    }
////    for(uint8_t i = 0; i != Archive_GetTarifSettings()->TarifZoneTotal; i++)
////    {
////        for(uint32_t t = pTarif[i].time_zone.start; t <= pTarif[i].time_zone.end; t += 10*60)
////        {
////            TarifBitMask[((uint32_t)(pTarif[i].title)) - 0x10000001] |= (((uint64_t)1) << (t/1800));  
////        }
////    }
//    
//}

//void Bits2Tarifs()
//{
//    uint8_t trf[12];
//    
//    uint8_t tariff;
//    for(uint8_t i = 0; i <= 48; i++) {
//        for(tariff = 1; tariff <= 3; tariff++) {
//            if ((TarifBitMask[tariff - 0] >> i) & 1) {
//                trf[i/4] |= tariff << (i%4);
//            }
//        }
//    }
//    TARIFF_set(trf, sizeof(trf));
   //return;
//   bool a;
//   Archive_GetTarifSettings()->TarifZoneTotal = 0;
//   for(uint8_t i = 0; i <= 2; i++)
//    {
//
//        a = 0;
//        for(int j = 0; j != 49; j++)
//        {
//
//            if((a == 0) && (TarifBitMask[i]&((uint64_t)1 << j)))
//            {
//                pTarif[Archive_GetTarifSettings()->TarifZoneTotal].time_zone.start = j*60*30;
//                a = 1;
//            }
//            else if((a == 1) && !(TarifBitMask[i]&((uint64_t)1 << j)))
//            {
//                pTarif[Archive_GetTarifSettings()->TarifZoneTotal].time_zone.end = j*60*30 - 1;
//                a = 0;
//                pTarif[Archive_GetTarifSettings()->TarifZoneTotal].title = (RateZoneType_e)(i + 0x10000001);
//                if(++Archive_GetTarifSettings()->TarifZoneTotal >= MAX_TARIF_ZONE_COUNT) Archive_GetTarifSettings()->TarifZoneTotal = MAX_TARIF_ZONE_COUNT - 1;
//            }
//        }
//    }
//}


//void Tarif_Sort( void );



//const TarifTitle_s *Tarif_GetTitle(uint8_t title)
//{
//  if( title < Archive_GetTarifSettings()->TarifTitleTotal )    return &pTitle[title];
//  else return NULL;
//}
//const Tarif_s *Tarif_GetSchedule(uint8_t schedule)
//{
//  if( schedule < Archive_GetTarifSettings()->TarifZoneTotal )    return &pTarif[schedule];
//  else return NULL;  
//}


//void Tarif_Deinit(void)
//{
//  if( pTitle ) free( pTitle );
//  pTitle = NULL;
//  if( pTarif ) free( pTarif );
//  pTarif = NULL;
//}


//void Tarif_SaveTarif( void )
//{
//    Tarifs2bits();
//    //if( ( Archive_CalcCalibrateCrc() == Archive_GetCalibrateSettings()->crc) && Archive_GetCalibrateSettings()->EnGain && Archive_GetCalibrateSettings()->WattH ) {
//      Archive_SaveSettings(true, true);
//    //} else Archive_SaveSettings(false, true);
//}

//uint8_t packet[] = { 0, 0x4e, 0x94, 0x62, 0xAA, 0x6e, 0xff };
//void Tarif_SetDefault(void)
//{
//    Archive_SetTarifTitle(3);
//    pTitle = (TarifTitle_s*) calloc (MAX_DAILY_TARIF_COUNT , sizeof(TarifTitle_s));
//    if( !pTitle) while(1);
//
//    pTitle[0].type = NIGHT_ZONE_RATE;
//    snprintf ((char *)pTitle[0].name,      sizeof(pTitle[0].name), "T1");
//    pTitle[1].type = PEAK_ZONE_RATE;
//    snprintf ((char *)pTitle[1].name,      sizeof(pTitle[1].name), "T2");
//    pTitle[2].type = HALF_PEAK_ZONE_RATE;
//    snprintf ((char *)pTitle[2].name,      sizeof(pTitle[2].name), "T3");
//
//    Archive_SetTarifZone(6);
//    pTarif = (Tarif_s*) calloc (MAX_TARIF_ZONE_COUNT , sizeof(Tarif_s));
//    if( !pTarif) while(1);
//
//    pTarif[0].title = NIGHT_ZONE_RATE;             // Ночь
//    pTarif[0].time_zone.start = 0;                 // 00:00
//    pTarif[0].time_zone.end = (7*3600) - 1;        // 06:69
//
//    pTarif[1].title = PEAK_ZONE_RATE;              // Пик
//    pTarif[1].time_zone.start = 7*3600;            // 07:00
//    pTarif[1].time_zone.end = (10*3600) - 1;       // 09:59
//
//    pTarif[2].title = HALF_PEAK_ZONE_RATE;         // Полупик
//    pTarif[2].time_zone.start = 10*3600;           // 10:00
//    pTarif[2].time_zone.end = (17*3600) - 1;       // 16:59
//   
//    pTarif[3].title = PEAK_ZONE_RATE;              // Пик%
//    pTarif[3].time_zone.start = 17*3600;           // 17:00
//    pTarif[3].time_zone.end = (21*3600) - 1;       // 20:59
//   
//    pTarif[4].title = HALF_PEAK_ZONE_RATE;         // Полупик
//    pTarif[4].time_zone.start = 21*3600;           // 21:00
//    pTarif[4].time_zone.end = (23*3600) - 1;       // 22:59
//
//    pTarif[5].title = NIGHT_ZONE_RATE;             // Ночь
//    pTarif[5].time_zone.start = 23*3600;           // 23:00
//    pTarif[5].time_zone.end = (24*3600) - 1;       // 23:59
//    
//    DTI_regProperty("pTitle", STRUCT_TYPE, (uint32_t)pTitle, 0);
//    DTI_regProperty("pTarif", STRUCT_TYPE, (uint32_t)pTarif, 0);
//    DTI_regProperty("TSettings", STRUCT_TYPE, (uint32_t)Archive_GetTarifSettings(), 0);  
//}

//size_t Tarif_ReadTarifTitle( size_t offset, TarifTitle_s* TarifTitle )
//{
//  uint32_t ptr, i;
//  uint8_t buffer[TARIF_TITLE_SIZE];
//  if( !TarifTitle ) return 0;
//  FLASH_Init_SPI();
//
//  for( i = 0; i < Archive_GetTarifSettings()->TarifTitleTotal; i++ ) {
//    ptr = 0;
//    if( ArchiveGetDataRecord( offset , buffer, TARIF_TITLE_SIZE ) ) {
//      memcpy( &TarifTitle[i].type,  &buffer[ptr],  sizeof(RateZoneType_e) );
//      ptr += sizeof(RateZoneType_e);
//      memcpy( &TarifTitle[i].name,  &buffer[ptr],  TITLE_NAME_LEN * sizeof(uint8_t) );
//    } else break;
//    offset += TARIF_TITLE_SIZE;
//  }
//  //FLASH_Deinit_SPI();
//  return i;
//}

//size_t Tarif_ReadTarifZone( size_t offset, Tarif_s* TarifZone )
//{
//  uint32_t ptr, i;
//  uint8_t buffer[TARIF_ZONE_SIZE];
//  if( !TarifZone ) return 0;  
//  FLASH_Init_SPI();
//    for( i = 0; i < Archive_GetTarifSettings()->TarifZoneTotal; i++ ) {
//      ptr = 0;      
//      if( ArchiveGetDataRecord( offset, buffer, TARIF_ZONE_SIZE ) ) {
//        memcpy( &pTarif[i].title,  &buffer[ptr],  sizeof(pTarif[i].title) );
//        ptr += sizeof(pTarif[i].title);
//        memset( &pTarif[i].time_zone, 0, sizeof(pTarif[i].time_zone) );
//        memcpy( &pTarif[i].time_zone.start,  &buffer[ptr],  sizeof(pTarif[i].time_zone.start) );
//      }
//      offset += TARIF_ZONE_SIZE;      
//    }
//    //FLASH_Deinit_SPI();
//    return i;
//}

//void Tarif_FillTarifSchedule( void )
//{
//  int i;
//  Tarif_Sort();
//  for( i = 0; i < Archive_GetTarifSettings()->TarifZoneTotal; i++ ) {
//    if( i != (Archive_GetTarifSettings()->TarifZoneTotal-1) )
//      pTarif[i].time_zone.end = pTarif[i+1].time_zone.start - 1;
//    else pTarif[i].time_zone.end = (24*3600) - 1;
//  }
//}



//void Tarif_Init( void )
//{
//  size_t cnt;
//
//
//   if( ( Archive_CalcTarifCrc() == Archive_GetTarifSettings()->crc) && Archive_GetTarifSettings()->TarifTitleTotal && Archive_GetTarifSettings()->TarifZoneTotal ) {
//    FLASH_Init_SPI();
//
//    pTitle = (TarifTitle_s*) calloc (MAX_DAILY_TARIF_COUNT, sizeof(TarifTitle_s));
//    if( !pTitle) while(1);
//
//    cnt = Tarif_ReadTarifTitle(TARIF_TITLE_ADR , pTitle );
//    if( cnt != Archive_GetTarifSettings()->TarifTitleTotal ) {
//      Tarif_Deinit();
//      Tarif_SetDefault();
//      Tarif_SaveTarif();
//      return;
//    }
//
//    pTarif = (Tarif_s*) calloc (MAX_TARIF_ZONE_COUNT, sizeof(Tarif_s));
//    if( !pTarif ) while(1);      
//
//    cnt = Tarif_ReadTarifZone( TARIF_TITLE_ADR  + (cnt * TARIF_TITLE_SIZE), pTarif );
//    if( cnt != Archive_GetTarifSettings()->TarifZoneTotal ) {
//      Tarif_Deinit();
//      Tarif_SetDefault();
//      Tarif_SaveTarif();
//      return;
//    }
//
//    Tarif_FillTarifSchedule();
//    
//    Tarifs2bits();
//    
//    Bits2Tarifs();
//    
//    DTI_regProperty("pTitle", STRUCT_TYPE, (uint32_t)pTitle, 0);
//    DTI_regProperty("pTarif", STRUCT_TYPE, (uint32_t)pTarif, 0);
//    DTI_regProperty("TSettings", STRUCT_TYPE, (uint32_t)Archive_GetTarifSettings(), 0);  
//    
//    //FLASH_Deinit_SPI();    
//  } else {
//    Tarif_SetDefault();
//    Tarif_SaveTarif();
//  }
//}

//static int Tarif_StartCmp (const void* p1, const void* p2)
//{
//  if ( ((Tarif_s*)p1)->time_zone.start <  ((Tarif_s*)p2)->time_zone.start ) return -1;
//  if ( ((Tarif_s*)p1)->time_zone.start == ((Tarif_s*)p2)->time_zone.start ) return 0;
//  if ( ((Tarif_s*)p1)->time_zone.start >  ((Tarif_s*)p2)->time_zone.start ) return 1;
//  return 0;
//}

//void Tarif_Sort( void )
//{
//  qsort ( &pTarif[0], Archive_GetTarifSettings()->TarifZoneTotal,  sizeof(Tarif_s), &Tarif_StartCmp ); 
//}


//RateZoneType_e Tarif_GetAbsolute(time_t time)
//{
//  uint8_t tarif;
//  for( tarif = 0; tarif < Archive_GetTarifSettings()->TarifZoneTotal; tarif++ ) {
//    if( (time >= pTarif[tarif].time_zone.start ) && (time <= pTarif[tarif].time_zone.end) )
//      return pTarif[tarif].title;
//  }
//  return EMPTY_RATE;
//}

//RateZoneType_e Tarif_GetCurrent(time_t time)
//{
//  return Tarif_GetAbsolute( time - GetStartCurrenttDay(time) );
//}

//uint8_t Tarif_index(RateZoneType_e tarif)
//{
//    switch(tarif) {
//    case TOTAL_RATE         : return 0;
//    case NIGHT_ZONE_RATE    : return 1;
//    case PEAK_ZONE_RATE     : return 2;
//    case HALF_PEAK_ZONE_RATE: return 3;
//    default: break;
//    }
//    return -1;
//}

//uint8_t Tarif_timestamp(uint32_t time)
//{
//    return Tarif_index(Tarif_GetCurrent((time_t)time));
//}