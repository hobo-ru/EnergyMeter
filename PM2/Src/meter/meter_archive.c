#include "meter_archive.h"

#include <string.h>
#include "td_rtc.h"

#include "meter.h"

typedef struct {
    uint32_t timestamp;
    uint32_t index;
} ARCH_record_info;

uint8_t * ARCH_load_rec(ARCH_control * acontrol, uint8_t *buffer, uint32_t index);

void ARCH_upd(ARCH_control * acontrol)
{
    ARCH_record_info r_h;
    ARCH_record_info r_l;
    ARCH_record_info r_cur;
    uint8_t buf[256]; // должно хватить)
    if (acontrol->settings.end_mem < acontrol->settings.start_mem) { while(1); }// assert
    if (sizeof(buf) < acontrol->settings.rec_size) { while(1); }// assert
    acontrol->index_head = -1;
//    acontrol->index_tail = -1;
    //return;
    
    // === head search Newton ===
    r_h.index = (acontrol->settings.end_mem - acontrol->settings.start_mem) / acontrol->settings.rec_size - 1;
    if (!ARCH_load_rec(acontrol, buf, r_h.index)) {
        r_h.timestamp = 0xFFFFFFFF;
    } else {
        memmove(&r_h.timestamp, buf, sizeof(uint32_t));
    }
    
    r_l.index = 0;
    if (!ARCH_load_rec(acontrol, buf, r_l.index)) {
        r_l.timestamp = 0xFFFFFFFF;
    } else {
        memmove(&r_l.timestamp, buf, sizeof(uint32_t));
    }
    r_cur.index = 0;
    
    if (r_l.timestamp == 0xFFFFFFFF) {
        acontrol->index_head = r_h.index;
        acontrol->index_send = acontrol->index_head;
//        acontrol->index_tail = r_h.index;
        return;
    }
    
    while (r_cur.index != (r_h.index + r_l.index) / 2) {
        r_cur.index = (r_h.index + r_l.index ) / 2;
        if (!ARCH_load_rec(acontrol, buf, r_cur.index)) {
            r_cur.timestamp = 0xFFFFFFFF;
        } else {
            memmove(&r_cur.timestamp, buf, sizeof(uint32_t));
        }
        
        if (r_cur.timestamp == 0xFFFFFFFF) {
            r_h.index = r_cur.index;
            r_h.timestamp = r_cur.timestamp;
        } else
        if (r_l.timestamp < r_cur.timestamp) {
            r_l.index = r_cur.index;
            r_l.timestamp = r_cur.timestamp;
        } else {
            r_h.index = r_cur.index;
            r_h.timestamp = r_cur.timestamp;
        }
    }
    acontrol->index_head = r_cur.index;
    if (!ARCH_load_rec(acontrol, buf, acontrol->index_head)) {
        acontrol->index_head = 0;
    }
    acontrol->index_send = acontrol->index_head;
    
//    // === tail search Newton ===
//    r_h.index = (acontrol->settings.end_mem - acontrol->settings.start_mem) / acontrol->settings.rec_size - 1;
//    if (!arch->load(acontrol, buf, r_h.index)) {
//        r_h.timestamp = 0xFFFFFFFF;
//    } else {
//        memmove(&r_h.timestamp, buf, sizeof(uint32_t));
//    }
//    
//    if (r_h.timestamp != 0xFFFFFFFF) {
//        r_l.index = r_cur.index;
//    } else {
//        r_h.index = r_cur.index;
//        r_h.timestamp = r_cur.timestamp;
//        r_l.index = 0;
//    }
//    if (!arch->load(acontrol, buf, r_l.index)) {
//        r_l.timestamp = 0xFFFFFFFF;
//    } else {
//        memmove(&r_l.timestamp, buf, sizeof(uint32_t));
//    }
//    while (r_cur.index != (r_h.index + r_l.index) / 2) {
//        r_cur.index = (r_h.index + r_l.index ) / 2;
//        if (!arch->load(acontrol, buf, r_cur.index)) {
//            r_cur.timestamp = 0xFFFFFFFF;
//        } else {
//            memmove(&r_cur.timestamp, buf, sizeof(uint32_t));
//        }
//        
//        if (r_cur.timestamp == 0xFFFFFFFF) {
//            r_l.index = r_cur.index;
//            r_l.timestamp = r_cur.timestamp;
//        } else
//        if (r_h.timestamp > r_cur.timestamp) {
//            r_h.index = r_cur.index;
//            r_h.timestamp = r_cur.timestamp;
//        } else {
//            r_l.index = r_cur.index;
//            r_l.timestamp = r_cur.timestamp;
//        }
//    }
//    if (r_cur.timestamp == 0xFFFFFFFF) {
//        if (r_h.timestamp == 0xFFFFFFFF) {
//            r_cur.index = r_l.index;
//            r_cur.timestamp = r_l.timestamp;
//        } else {
//            r_cur.index = r_h.index;
//            r_cur.timestamp = r_h.timestamp;
//        }
//    }
//    acontrol->index_tail = r_cur.index;
}

void ARCH_upd_METER_INTEGRATORS_and_RTC(ARCH_control * acontrol)
{ 
    uint8_t buf[256];
    uint8_t i, j;
    METER_ARCH_DATA_SETTINGS * mads;
    METER_ARCH_DATA * mad;
    METER_INTEGRATORS * itgr;
    for (j = 0; j < ARCH_DATA_MAX; ++j) { // обновляем итераторы из последних записей архивов
    //j = 0; {
        if (ARCH_load_rec(&acontrol[j], buf, acontrol[j].index_head)) {
            mads = &meter->data->aDataSet[j];
            mad = (METER_ARCH_DATA *)buf;
            if( *((time_t*)buf) > __time32(0) ) { // восстанавливаем время
                TD_RTC_SetOffsetTime( *((time_t*)buf) );
            }
            for (i = 0; i < mads->count_param; ++i) {
                itgr = &meter->data->itgr[mads->param_id[i].tariff][mads->param_id[i].phase];
                switch (mads->param_id[i].MDT) {
                case MDT_P_p: { itgr->P_p = (uint64_t)(mad->value[i].f32 * 2000.0); break; }
                case MDT_P_n: { itgr->P_n = (uint64_t)(mad->value[i].f32 * 2000.0); break; }
                case MDT_Q_p: { itgr->Q_p = (uint64_t)(mad->value[i].f32 * 2000.0); break; }
                case MDT_Q_n: { itgr->Q_n = (uint64_t)(mad->value[i].f32 * 2000.0); break; }
                case MDT_Q1:  { itgr->Q1  = (uint64_t)(mad->value[i].f32 * 2000.0); break; }
                case MDT_Q2:  { itgr->Q2  = (uint64_t)(mad->value[i].f32 * 2000.0); break; }
                case MDT_Q3:  { itgr->Q3  = (uint64_t)(mad->value[i].f32 * 2000.0); break; }
                case MDT_Q4:  { itgr->Q4  = (uint64_t)(mad->value[i].f32 * 2000.0); break; }
                case MDT_S:   { itgr->S   = (uint64_t)(mad->value[i].f32 * 2000.0); break; }
                }
            }
        }
    }
    for (j = 0; j < TARIFF_MAX; ++j) { // сумма внутри тарифа по фазам, если не записан итератор
        itgr = &meter->data->itgr[j][0];
        if (!itgr->P_p) { for (i = 1; i < PHASE_MAX; ++i) { itgr->P_p += meter->data->itgr[j][i].P_p;} }
        if (!itgr->P_n) { for (i = 1; i < PHASE_MAX; ++i) { itgr->P_n += meter->data->itgr[j][i].P_n;} }
        if (!itgr->Q_p) { for (i = 1; i < PHASE_MAX; ++i) { itgr->Q_p += meter->data->itgr[j][i].Q_p;} }
        if (!itgr->Q_n) { for (i = 1; i < PHASE_MAX; ++i) { itgr->Q_n += meter->data->itgr[j][i].Q_n;} }
        if (!itgr->S)   { for (i = 1; i < PHASE_MAX; ++i) { itgr->S   += meter->data->itgr[j][i].S;  } }
        if (!itgr->Q1)  { for (i = 1; i < PHASE_MAX; ++i) { itgr->Q1  += meter->data->itgr[j][i].Q1; } }
        if (!itgr->Q2)  { for (i = 1; i < PHASE_MAX; ++i) { itgr->Q2  += meter->data->itgr[j][i].Q2; } }
        if (!itgr->Q3)  { for (i = 1; i < PHASE_MAX; ++i) { itgr->Q3  += meter->data->itgr[j][i].Q3; } }
        if (!itgr->Q4)  { for (i = 1; i < PHASE_MAX; ++i) { itgr->Q4  += meter->data->itgr[j][i].Q4; } }
    }
    itgr = &meter->data->itgr[0][0]; // сумма по тарифам, если не записан итератор
    if (!itgr->P_p) { for (i = 1; i < TARIFF_MAX; ++i) { itgr->P_p += meter->data->itgr[i][0].P_p;} }
    if (!itgr->P_n) { for (i = 1; i < TARIFF_MAX; ++i) { itgr->P_n += meter->data->itgr[i][0].P_n;} }
    if (!itgr->Q_p) { for (i = 1; i < TARIFF_MAX; ++i) { itgr->Q_p += meter->data->itgr[i][0].Q_p;} }
    if (!itgr->Q_n) { for (i = 1; i < TARIFF_MAX; ++i) { itgr->Q_n += meter->data->itgr[i][0].Q_n;} }
    if (!itgr->S)   { for (i = 1; i < TARIFF_MAX; ++i) { itgr->S   += meter->data->itgr[i][0].S;  } }
    if (!itgr->Q1)  { for (i = 1; i < TARIFF_MAX; ++i) { itgr->Q1  += meter->data->itgr[i][0].Q1; } }
    if (!itgr->Q2)  { for (i = 1; i < TARIFF_MAX; ++i) { itgr->Q2  += meter->data->itgr[i][0].Q2; } }
    if (!itgr->Q3)  { for (i = 1; i < TARIFF_MAX; ++i) { itgr->Q3  += meter->data->itgr[i][0].Q3; } }
    if (!itgr->Q4)  { for (i = 1; i < TARIFF_MAX; ++i) { itgr->Q4  += meter->data->itgr[i][0].Q4; } }

    if (ARCH_load_rec(&acontrol[2], buf, acontrol[2].index_head)) { // восстанавливаем время
        if( *((time_t*)buf) > __time32(0) ) {
            TD_RTC_SetOffsetTime( *((time_t*)buf) );
        }
    }
}
