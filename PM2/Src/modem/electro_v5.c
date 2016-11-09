#include "electro_v5.h"
#include "modem.h"


#include "td_measure.h"

#include "archive.h"
//#include "adders.h"
//#include "tarif.h"
#include <stdlib.h>
#include <math.h>
#include "meter.h"
#include "tariff.h"
#include "archive.h"

size_t Electro_MakeEvent( uint8_t event, time_t time,  uint8_t *Buffer, size_t BufSize )
{
  if(BufSize < 8) return 0;
  
  Buffer[0] = EL_V5_EVENTS;

  Buffer[1] = (time>>24)&0xFF;
  Buffer[2] = (time>>16)&0xFF;
  Buffer[3] = (time>>8)&0xFF;
  Buffer[4] = (time>>0)&0xFF;
  Buffer[5] = event;
  Buffer[6] = TD_MEASURE_VoltageTemperature (false);
  Buffer[7] = TD_MEASURE_Temperature();  

  return BufSize;
}

//size_t Electro_MakeMessagesByDelta( uint8_t Delta, time_t Time, uint8_t *Buffer, size_t BufferSize )
//{
//  uint8_t delta;
//  int i;
//  if( BufferSize < 8 ) return 0;
//     
//  memset( Buffer, 0, BufferSize );
//  Time /= (60*60*24);
//  switch( Delta ) {
//    case EL_V5_DELTA_1_PH_TOTAL:
//      Buffer[0] = EL_V5_DELTA_1_PH_TOTAL;
//        Buffer[1] = ( Time>>8 )&0xFF;
//        Buffer[2] = ( Time>>0 )&0xFF;
//        for( i = 0; i < 24; i++ ) {
//          delta = prevDayDelta_Get(i);
//          if( i < 8 ) {
//            Buffer[3] |= ((delta&8)>>3)     << (7-i);              //старшие биты от почасовой разбивки потреблени€
//            Buffer[6] |= ((delta&4)>>2)     << (7-i);              //первый блок третьих битов от почасовой разбивки потреблени€
//          } else if( i < 16 ) {
//            Buffer[4] |= ((delta&8)>>3)     << (7-(i-8));          //старшие биты от почасовой разбивки потреблени€
//            Buffer[7] |= ((delta&4)>>2)     << (7-(i-8));          //первый блок третьих битов от почасовой разбивки потреблени€
//          } else {
//            Buffer[5] |= ((i&8)>>3)         << (7-(i-16));         //старшие биты от почасовой разбивки потреблени€
//          }
//        }
//      break;
//  
//    case EL_V5_DELTA_2_PH_TOTAL:
//      Buffer[0] = EL_V5_DELTA_2_PH_TOTAL;
//      for( i = 0; i < 24; i++ ) {
//        delta = prevDayDelta_Get(i);
//        if( i < 8 ) {
//          Buffer[2] |= ((delta&2) >> 1)   << ( 7-i );              //вторые  биты от почасовой разбивки потреблени€
//          Buffer[5] |= (delta&1)          << ( 7-i );              //младшие биты от почасовой разбивки потреблени€
//        } else if( i < 16 ) {
//          Buffer[3] |= ((delta&2) >> 1)   << ( 7-(i-8) );          //вторые  биты от почасовой разбивки потреблени€
//          Buffer[6] |= (delta&1)          << ( 7-(i-8) );          //младшие биты от почасовой разбивки потреблени€
//        } else {
//          Buffer[1] |= ((delta&4) >> 2)   << ( 7-(i-16) );         //второй блок третьих битов от почасовой разбивки потреблени€
//          Buffer[4] |= ((delta&2) >> 1)   << ( 7-(i-16) );         //вторые  биты от почасовой разбивки потреблени€
//          Buffer[7] |= (delta&1)          << ( 7-(i-16) );         //младшие биты от почасовой разбивки потреблени€
//        }
//        }
//      break;
//  }
//  return BufferSize;
//}

//size_t Electro_MakeMessagesByTarif( uint8_t El_V5, time_t Time, float Active_P, float Active_N, uint8_t *Buffer, size_t BufferSize )
//{
//  uint32_t Positive, Negative; 
//
//  if( BufferSize < 8 ) return 0;
//
//  Time /= (60*60*24);
//
//  Positive = (((uint32_t)Active_P) % 0x3FFFFFFF ) << 2;
//  Negative = ((uint32_t)fabs( Active_N )) % 0x3FF;
//
//  Buffer[0] = El_V5;
//  Buffer[1] = ( Time>>8 )&0xFF;
//  Buffer[2] = ( Time>>0 )&0xFF;
//  Buffer[3] = ( Positive>>24 )&0xFF;
//  Buffer[4] = ( Positive>>16 )&0xFF;
//  Buffer[5] = ( Positive>>8 )&0xFF;
//  Buffer[6] = ( Positive>>0 )&0xFC;
//  Buffer[6] |= ((Negative>>8 )&0x3);
//  Buffer[7] = Negative&0xFF;
//
//  return BufferSize;
//}

void METER_ARCH_PARAM_CFG_to_PARAM_IDENTIFIER(PARAM_IDENTIFIER * pi, METER_ARCH_PARAM_CFG * mapc)
{
    pi->timestamp_flag = 0;
    pi->obis_C = 0;
    pi->tariff = mapc->tariff;
    pi->obis_D = 8;
    switch (mapc->MDT) {
    case MDT_P_p:
    case MDT_P_p_d: { pi->obis_C = 1 + 20 * mapc->phase; break; } // ?Li Active power+ (QI+QIV)
    case MDT_P_n:
    case MDT_P_n_d: { pi->obis_C = 2 + 20 * mapc->phase; break; } // ?Li Active powerЦ (QII+QIII)
    case MDT_Q_p:
    case MDT_Q_p_d: { pi->obis_C = 3 + 20 * mapc->phase; break; } // ?Li Reactive power+ (QI+QII)
    case MDT_Q_n:
    case MDT_Q_n_d: { pi->obis_C = 4 + 20 * mapc->phase; break; } // ?Li Reactive powerЦ (QIII+QIV)
    case MDT_S:
    case MDT_S_d:   { pi->obis_C = 9 + 20 * mapc->phase; break; } // ?Li Apparent power+ (QI+QIV)
    case MDT_Q1:    { pi->obis_C = 5 + 20 * mapc->phase; break; } // ?Li Reactive power QI
    case MDT_Q2:    { pi->obis_C = 6 + 20 * mapc->phase; break; } // ?Li Reactive power QII
    case MDT_Q3:    { pi->obis_C = 7 + 20 * mapc->phase; break; } // ?Li Reactive power QIII
    case MDT_Q4:    { pi->obis_C = 8 + 20 * mapc->phase; break; } // ?Li Reactive power QIV
    case MDT_U:     { pi->obis_C = 12+ 20 * mapc->phase; break; } // Voltage : any phase
    case MDT_Ulin:  { pi->obis_C = 12+ 20 * mapc->phase; break; } // Voltage : any phase
    case MDT_I:     { pi->obis_C = 11+ 20 * mapc->phase; break; } // Current : any phase
    case MDT_F:     { pi->obis_C = 14+ 20 * mapc->phase; break; } // Supply frequency
    case MDT_cosFi: { pi->obis_C = 13+ 20 * mapc->phase; break; } // ?Li Power factor
    default:        { pi->obis_C = 0; break; }                   // error
    }
}

uint8_t Electro_MakeNbfiArch(uint8_t *buf, uint8_t EL_V5, METER_ARCH_DATA_SETTINGS * mads, METER_ARCH_DATA * mad)
{
    uint8_t buf_index = 0;
    uint8_t index;
    uint32_t val_u;
    buf[buf_index++] = EL_V5;
    for (index = 0; index < mads->count_param; index++) {
        METER_ARCH_PARAM_CFG mapc = mads->param_id[index];
        PARAM_IDENTIFIER pi;
        METER_ARCH_PARAM_CFG_to_PARAM_IDENTIFIER(&pi, &mapc);
        pi.timestamp_flag = !index ? 1 : 0;
        uint8_t * ppi = (uint8_t*)&pi;
        buf[buf_index++] = ppi[0];
        buf[buf_index++] = ppi[1];
//        buf[buf_index++] = ((*((uint8_t*)&pi)) >> 8) & 0xFF;
//        buf[buf_index++] = ((*((uint8_t*)&pi)) >> 0) & 0xFF;
        if (pi.timestamp_flag) {
            buf[buf_index++] = (mad->timestamp >> 24) & 0xFF;
            buf[buf_index++] = (mad->timestamp >> 16) & 0xFF;
            buf[buf_index++] = (mad->timestamp >> 8 ) & 0xFF;
            buf[buf_index++] = (mad->timestamp >> 0 ) & 0xFF;
        }
        val_u = (uint32_t)mad->value[index].f32;
        buf[buf_index++] = (val_u >> 24) & 0xFF;
        buf[buf_index++] = (val_u >> 16) & 0xFF;
        buf[buf_index++] = (val_u >> 8 ) & 0xFF;
        buf[buf_index++] = (val_u >> 0 ) & 0xFF;
    }
    return buf_index;
}

uint8_t Electro_MakeNbfiCurr(uint8_t *buf, uint8_t EL_V5, uint32_t timestamp)
{
    METER_ARCH_DATA_SETTINGS mads;
    METER_ARCH_DATA mad;
    METER_ARCH_DATA_SETTINGS *ads = &mads;
    METER_INTEGRATORS * itgr;
    ads->timeout = 0;
    ads->start_at_time_alignment = METER_TIME_ALIGMENT_day;
    ads->count_param = ARCH_DATA_PARAM_COUT_MAX;
    uint8_t p = 0;
    uint8_t k = 0;
    mad.timestamp = timestamp;
    for (k = 1; k <= 4; ++k) {
        itgr = &meter->data->itgr[k][0];
        
        ads->param_id[p].MDT = MDT_P_p;
        ads->param_id[p].phase = 0;
        ads->param_id[p].tariff = k;
        mad.value[p].f32 = itgr->P_p * meter->K;
        p++;
        ads->param_id[p].MDT = MDT_P_n;
        ads->param_id[p].phase = 0;
        ads->param_id[p].tariff = k;
        mad.value[p].f32 = itgr->P_n * meter->K;
        p++;
        ads->param_id[p].MDT = MDT_Q_p;
        ads->param_id[p].phase = 0;
        ads->param_id[p].tariff = k;
        mad.value[p].f32 = itgr->Q_p * meter->K;
        p++;
        ads->param_id[p].MDT = MDT_Q_n;
        ads->param_id[p].phase = 0;
        ads->param_id[p].tariff = k;
        mad.value[p].f32 = itgr->Q_n * meter->K;
        p++;
    }
    return Electro_MakeNbfiArch(buf, EL_V5, ads, &mad);
}

uint8_t Electro_MakeNbfiEvent(uint8_t *buf, uint8_t EL_V5, METER_ARCH_EVENT * mae, uint8_t shift)
{
    uint8_t buf_index = shift;
    if (!shift) {
        buf[buf_index++] = EL_V5;
    }
    buf[buf_index++] = (mae->timestamp >> 24) & 0xFF;
    buf[buf_index++] = (mae->timestamp >> 16) & 0xFF;
    buf[buf_index++] = (mae->timestamp >> 8 ) & 0xFF;
    buf[buf_index++] = (mae->timestamp >> 0 ) & 0xFF;
    buf[buf_index++] = (mae->event          ) & 0xFF;
    buf[buf_index++] = (mae->value     >> 8 ) & 0xFF;
    buf[buf_index++] = (mae->value     >> 0 ) & 0xFF;
    return buf_index;
}

//uint8_t Electro_MakeDayArch(uint8_t *buf, uint32_t index_record)
//{
//    uint8_t buf_arh[256];
//    if (!ARCH_load_data(0, index_record, (uint8_t*)buf_arh)) return 0;
//    METER_ARCH_DATA_SETTINGS * mads = &meter->data->aDataSet[0];
//    return Electro_MakeNbfiArch(buf, EL_V5_NBFI_DAILY, mads, (METER_ARCH_DATA *)buf_arh);
//}
//
//uint8_t Electro_MakeProfileArch(uint8_t *buf, uint32_t index_record)
//{
//    uint8_t buf_arh[256];
//    if (!ARCH_load_data(1, index_record, (uint8_t*)buf_arh)) return 0;
//    METER_ARCH_DATA_SETTINGS * mads = &meter->data->aDataSet[1];
//    return Electro_MakeNbfiArch(buf, EL_V5_NBFI_POWER_PROFILE, mads, (METER_ARCH_DATA *)buf_arh);
//}

//static uint8_t DayRegularCnt = 0;
//size_t Electro_MakeDayRegular( uint8_t *Buffer, size_t BufferLenght )
//{
//  size_t result;
//  
//  uint8_t i;
//  uint8_t buf[256];
//  uint8_t tariff;
//  float P_p;
//  float P_n;
//  if (!ARCH_load_data(0, arch_settings[0].index_head, (uint8_t*)buf)) return 0;
//  METER_ARCH_DATA_SETTINGS * mads = &meter->data->aDataSet[0];
//  METER_ARCH_DATA * mad = (METER_ARCH_DATA *)buf;
//
//  switch( DayRegularCnt ) {
//    case 0: {
//        tariff = 1;
//        for (i = 0; i < mads->count_param; ++i) {
//            if (mads->param_id[i].tariff != tariff || mads->param_id[i].phase) continue;
//            switch (mads->param_id[i].MDT) {
//            case MDT_P_p: { P_p = mad->value[i].f32; break; }
//            case MDT_P_n: { P_n = mad->value[i].f32; break; }
//            default: break;
//            }
//        }
//        result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T1, mad->timestamp, P_p, P_n, Buffer, BufferLenght );
//        break;
//    }
////      if( Archive_GetByTarif( NIGHT_ZONE_RATE ) ) {
////        result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T1, Archive_GetDaily()->time, Archive_GetByTarif(NIGHT_ZONE_RATE)->A_p, Archive_GetByTarif(NIGHT_ZONE_RATE)->A_n, Buffer, BufferLenght );
////        break;
////      } else Electro_SetNextRegularDay();
//      
//    case 1: {
//        tariff = 2;
//        for (i = 0; i < mads->count_param; ++i) {
//            if (mads->param_id[i].tariff != tariff || mads->param_id[i].phase) continue;
//            switch (mads->param_id[i].MDT) {
//            case MDT_P_p: { P_p = mad->value[i].f32; break; }
//            case MDT_P_n: { P_n = mad->value[i].f32; break; }
//            default: break;
//            }
//        }
//        result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T2, mad->timestamp, P_p, P_n, Buffer, BufferLenght );
//        break;
//    }
////      if( Archive_GetByTarif( PEAK_ZONE_RATE ) ) {
////        result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T2, Archive_GetDaily()->time, Archive_GetByTarif(PEAK_ZONE_RATE)->A_p, Archive_GetByTarif(PEAK_ZONE_RATE)->A_n, Buffer, BufferLenght );
////        break;
////      } else Electro_SetNextRegularDay();
//      
//    case 2: {
//        tariff = 3;
//        for (i = 0; i < mads->count_param; ++i) {
//            if (mads->param_id[i].tariff != tariff || mads->param_id[i].phase) continue;
//            switch (mads->param_id[i].MDT) {
//            case MDT_P_p: { P_p = mad->value[i].f32; break; }
//            case MDT_P_n: { P_n = mad->value[i].f32; break; }
//            default: break;
//            }
//        }
//        result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T3, mad->timestamp, P_p, P_n, Buffer, BufferLenght );
//        break;
//    }
////      if( Archive_GetByTarif( HALF_PEAK_ZONE_RATE ) ) {
////        result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T3, Archive_GetDaily()->time, Archive_GetByTarif(HALF_PEAK_ZONE_RATE)->A_p, Archive_GetByTarif(HALF_PEAK_ZONE_RATE)->A_n, Buffer, BufferLenght );
////        break;
////      } else Electro_SetNextRegularDay();
//
//    case 3:     result = Electro_MakeMessagesByDelta( EL_V5_DELTA_1_PH_TOTAL, mad->timestamp, Buffer, BufferLenght ); break;
//    case 4:     result = Electro_MakeMessagesByDelta( EL_V5_DELTA_2_PH_TOTAL, mad->timestamp, Buffer, BufferLenght ); break;
//    
//    default:    DayRegularCnt = 0; return 0;
//  }
//  return result;
//}
//void Electro_SetNextRegularDay( void )
//{
//  DayRegularCnt++;
//}

//TODO delete after debug
//static uint8_t CurrentAddersCnt = 0;
//uint8_t Electro_GetCurrentAddersCnt( void )       {     return CurrentAddersCnt;        }
//size_t Electro_MakeCurrentAdders( uint8_t *Buffer, size_t BufferLenght )
//{
//  size_t result;
//  uint8_t tariff;
//
//  switch( CurrentAddersCnt ) {
//    case 0:     //night
//        tariff = 1;
//        result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T1, __time32(0)
//                                    , ((float)(meter->data->itgr[tariff][1].P_p)) * meter->K
//                                    , ((float)(meter->data->itgr[tariff][1].P_n)) * meter->K
//                                    , Buffer, BufferLenght );
//        break;
////      if( Adders_GetByTarif( NIGHT_ZONE_RATE ) ) {
////        result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T1, __time32(0), Adders_GetByTarif(NIGHT_ZONE_RATE)->Active_P, Adders_GetByTarif(NIGHT_ZONE_RATE)->Active_N, Buffer, BufferLenght );
////        break;
////      } else Electro_SetNextAdders();
//      
//    case 1:     //peak
//        tariff = 2;
//        result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T2, __time32(0)
//                                    , ((float)(meter->data->itgr[tariff][1].P_p)) * meter->K
//                                    , ((float)(meter->data->itgr[tariff][1].P_n)) * meter->K
//                                    , Buffer, BufferLenght );
//        break;
////      if( Adders_GetByTarif( PEAK_ZONE_RATE ) ) {
////        result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T2, __time32(0), Adders_GetByTarif(PEAK_ZONE_RATE)->Active_P, Adders_GetByTarif(PEAK_ZONE_RATE)->Active_N, Buffer, BufferLenght );
////        break;
////      } else Electro_SetNextAdders();
//
//    case 2:     //half peak
//        tariff = 3;
//        result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T3, __time32(0)
//                                    , ((float)(meter->data->itgr[tariff][1].P_p)) * meter->K
//                                    , ((float)(meter->data->itgr[tariff][1].P_n)) * meter->K
//                                    , Buffer, BufferLenght );
//        break;
////      if( Adders_GetByTarif( HALF_PEAK_ZONE_RATE ) ) {
////      result = Electro_MakeMessagesByTarif( EL_V5_ACT_PN_TOTAL_T3, __time32(0), Adders_GetByTarif(HALF_PEAK_ZONE_RATE)->Active_P, Adders_GetByTarif(HALF_PEAK_ZONE_RATE)->Active_N, Buffer, BufferLenght );
////      break;
////      }else Electro_SetNextAdders();
//
//    //todo
//    case 3:     result = Electro_MakeMessagesByDelta( EL_V5_DELTA_1_PH_TOTAL, __time32(0), Buffer, BufferLenght ); break;
//    case 4:     result = Electro_MakeMessagesByDelta( EL_V5_DELTA_2_PH_TOTAL, __time32(0), Buffer, BufferLenght ); break;
//
//    default:    CurrentAddersCnt = 0; return 0;
//  }
//  return result;
//}
//
//void Electro_SetNextAdders( void )
//{
//  CurrentAddersCnt++;
//}

void Electro_MakeTarifs(uint8_t *buf, uint8_t tariff)
{
    uint64_t tmp_mask = 0;
    for(uint8_t i = 0; i <= 48; i++) {
        if (TARIFF_from_zone(i) == tariff) {
            tmp_mask |= (((uint64_t)1) << i);  
        }
    }
    buf[0] = EL_V5_SEND_TARIF;
    buf[1] = (tariff - 1) << 6;
    for(int8_t i = 7; i != 1; i--) {
        buf[i] = ((uint8_t *)&tmp_mask)[7-i];
    }
}

void Electro_MakeProfilePower(uint8_t *buf, uint8_t index)
{
    uint32_t Time = __time32(0);
    Time /= 60;
    float val_f = meter->data->aData[1].value[index].f32;
    int32_t val_i = (int)val_f;
    val_i <<= 2;
    METER_ARCH_PARAM_CFG mapc = meter->data->aDataSet[1].param_id[index];
    buf[0] = EL_V5_POWER_PROFILE;
    buf[1] = ( Time>>8 )&0xFF;
    buf[2] = ( Time>>0 )&0xFF;
    buf[3] = ( val_i>>24 )&0xFF;
    buf[4] = ( val_i>>16 )&0xFF;
    buf[5] = ( val_i>>8 )&0xFF;
    buf[6] = ( val_i>>0 )&0xFF;
    buf[6] |= ((mapc.tariff >> 1) & 0x03);
    buf[7] =  ((mapc.tariff >> 0) & 0x01) << 7;
    uint8_t OBIS = 0;
    switch (mapc.MDT) {
    case MDT_P_p:
    case MDT_P_p_d: { OBIS = 1 + 20 * mapc.phase; break; } // ?Li Active power+ (QI+QIV)
    case MDT_P_n:
    case MDT_P_n_d: { OBIS = 2 + 20 * mapc.phase; break; } // ?Li Active powerЦ (QII+QIII)
    case MDT_Q_p:
    case MDT_Q_p_d: { OBIS = 3 + 20 * mapc.phase; break; } // ?Li Reactive power+ (QI+QII)
    case MDT_Q_n:
    case MDT_Q_n_d: { OBIS = 4 + 20 * mapc.phase; break; } // ?Li Reactive powerЦ (QIII+QIV)
    case MDT_S:
    case MDT_S_d:   { OBIS = 9 + 20 * mapc.phase; break; } // ?Li Apparent power+ (QI+QIV)
    case MDT_Q1:    { OBIS = 5 + 20 * mapc.phase; break; } // ?Li Reactive power QI
    case MDT_Q2:    { OBIS = 6 + 20 * mapc.phase; break; } // ?Li Reactive power QII
    case MDT_Q3:    { OBIS = 7 + 20 * mapc.phase; break; } // ?Li Reactive power QIII
    case MDT_Q4:    { OBIS = 8 + 20 * mapc.phase; break; } // ?Li Reactive power QIV
    case MDT_U:     { OBIS = 12+ 20 * mapc.phase; break; } // Voltage : any phase
    case MDT_Ulin:  { OBIS = 12+ 20 * mapc.phase; break; } // Voltage : any phase
    case MDT_I:     { OBIS = 11+ 20 * mapc.phase; break; } // Current : any phase
    case MDT_F:     { OBIS = 14+ 20 * mapc.phase; break; } // Supply frequency
    case MDT_cosFi: { OBIS = 13+ 20 * mapc.phase; break; } // ?Li Power factor
    }
    buf[7] |= OBIS & 0x7F;
}

uint8_t Electro_MakeProfileSettings(uint8_t *buf)
{
    V5_SET_PROFILE *profile = (V5_SET_PROFILE *)buf;
    METER_ARCH_DATA_SETTINGS * mads = &meter->data->aDataSet[1];
    memset(profile, 0, sizeof(V5_SET_PROFILE));
    profile->EL_V5 = EL_V5_SET_PROFILE;
    profile->timeout_hi = (mads->timeout / 60) >> 4;
    profile->timeout_lo = (mads->timeout / 60) & 0xFF;
    switch (mads->start_at_time_alignment) {
    case 0: profile->alignment = 0; break;
    case 2: profile->alignment = 1; break;
    case 3: profile->alignment = 2; break;
    default: profile->alignment = 0; break;
    }
    profile->need_send = need_send_profile;
    for (int i = 0; i < 4; ++i) {
        METER_ARCH_PARAM_CFG_to_PARAM_IDENTIFIER(&profile->pi[i], &mads->param_id[i]);
    }
    return sizeof(V5_SET_PROFILE);
}