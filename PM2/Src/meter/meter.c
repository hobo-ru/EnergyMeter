#include "meter.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

//#define METER_EMULATE_DATA

#ifdef METER_EMULATE_DATA
#include <math.h>
#else
#include "defines_HW.h"
#endif

METER_DATA _meter_data;
METER _meter;
METER * meter = &_meter;

void METER_emulate();

void METER_init()
{
    memset(meter, 0, sizeof(METER));
    meter->data = &_meter_data;
//    meter->update = METER_upd;
    METER_ARCH_DATA_SETTINGS * ads;
    ads = &meter->data->aDataSet[0];
    ads->timeout = 60 * 60 * 24; // сутки
    ads->start_at_time_alignment = METER_TIME_ALIGMENT_day;
    ads->count_param = ARCH_DATA_PARAM_COUT_MAX;
    uint8_t p = 0;
    uint8_t k = 0;
    for (k = 1; k <= 4; ++k) {
        ads->param_id[p].MDT = MDT_P_p;
        ads->param_id[p].phase = 0;
        ads->param_id[p].tariff = k;
        p++;
        ads->param_id[p].MDT = MDT_P_n;
        ads->param_id[p].phase = 0;
        ads->param_id[p].tariff = k;
        p++;
        ads->param_id[p].MDT = MDT_Q_p;
        ads->param_id[p].phase = 0;
        ads->param_id[p].tariff = k;
        p++;
        ads->param_id[p].MDT = MDT_Q_n;
        ads->param_id[p].phase = 0;
        ads->param_id[p].tariff = k;
        p++;
    }
    
    ads = &meter->data->aDataSet[1];
    p = 0;
    ads->timeout = 60 * 30; // 30мин срезы
    ads->start_at_time_alignment = METER_TIME_ALIGMENT_hour;
    ads->count_param = ARCH_DATA_PROFILE_PARAM_COUT_MAX;
    for (k = 0; k < 1; ++k) {
        ads->param_id[p].MDT = MDT_P_p;
        ads->param_id[p].phase = k;
        ads->param_id[p].tariff = 1;
        p++;
        ads->param_id[p].MDT = MDT_P_p;
        ads->param_id[p].phase = k;
        ads->param_id[p].tariff = 2;
        p++;
        ads->param_id[p].MDT = MDT_Q_p;
        ads->param_id[p].phase = k;
        ads->param_id[p].tariff = 0;
        p++;
        ads->param_id[p].MDT = MDT_Q_n;
        ads->param_id[p].phase = k;
        ads->param_id[p].tariff = 0;
        p++;
    }
    
    METER_QUALITY_SETTINGS * mqs = &meter->data->mQualitySet_F;
    mqs->nominal = 50.0;
    mqs->time_integral = 10;
    mqs->max = mqs->nominal + 0.2;
    mqs->min = mqs->nominal - 0.2;
    mqs = &meter->data->mQualitySet_U;
    mqs->nominal = 220.0;
    mqs->time_integral = 10 * 60;
    mqs->max = mqs->nominal * 1.1;
    mqs->min = mqs->nominal * 0.9;
}

void METER_upd_Quality_F(float new_val, uint32_t timestamp, METER_QUALITY_SETTINGS * mqs, METER_QUALITY * mq
                         , METER_EVENT me_max_start, METER_EVENT me_max_stop
                         , METER_EVENT me_min_start, METER_EVENT me_min_stop
                         , float scale)
{
    mq->value_to_aver += new_val;
    if (++mq->count_to_aver < mqs->time_integral) {
        return;
    }
    mq->value_d = mq->value_to_aver / mq->count_to_aver;
    mq->value_to_aver = 0;
    mq->count_to_aver = 0;
    
    if (mq->value_d > mqs->max && !mq->flag) { // выход за допустимый максимум
        mq->flag = 1;
        meter->event_arch(me_max_start, timestamp, 0);
        mq->value_limit = mq->value_d;
    } else
    if (mq->value_d > mqs->max && mq->flag == 1) {
        if (mq->value_d > mq->value_limit) {
            mq->value_limit = mq->value_d;
        }
    } else
    if (mq->value_d < mqs->max && mq->flag == 1) {
        mq->flag = 0;
        uint32_t val = (uint16_t)(mq->value_limit * scale);
        meter->event_arch(me_max_stop, timestamp, val&0xFFFF);
    } 
        
    if (mq->value_d < mqs->min && !mq->flag) { // выход за допустимый минимум
        mq->flag = 2;
        meter->event_arch(me_min_start, timestamp, 0);
        mq->value_limit = mq->value_d;
    } else
    if (mq->value_d < mqs->min && mq->flag == 2) {
        if (mq->value_d < mq->value_limit) {
            mq->value_limit = mq->value_d;
        }
    } else
    if (mq->value_d > mqs->min && mq->flag == 2) {
        mq->flag = 0;
        uint32_t val = (uint16_t)(mq->value_limit * scale);
        meter->event_arch(me_min_stop, timestamp, val&0xFFFF);
    }        
}

void METER_upd_Quality_U(float new_val, uint32_t timestamp, METER_QUALITY_SETTINGS * mqs, METER_QUALITY * mq
                         , METER_EVENT me_max_start, METER_EVENT me_max_stop
                         , METER_EVENT me_min_start, METER_EVENT me_min_stop
                         , float scale)
{
    mq->value_to_aver += new_val;
    if (++mq->count_to_aver >= mqs->time_integral) {
        mq->value_d = mq->value_to_aver / mq->count_to_aver;
        mq->value_to_aver = 0;
        mq->count_to_aver = 0;
    }
    
    if (new_val > mqs->max && !mq->flag) { // выход за допустимый максимум
        mq->flag = 1;
        meter->event_arch(me_max_start, timestamp, 0);
        mq->value_limit = new_val;
    } else
    if (new_val > mqs->max && mq->flag == 1) {
        if (new_val > mq->value_limit) {
            mq->value_limit = new_val;
        }
    } else
    if (new_val < mqs->max && mq->flag == 1) {
        mq->flag = 0;
        uint32_t val = (uint16_t)(mq->value_limit * scale);
        meter->event_arch(me_max_stop, timestamp, val&0xFFFF);
    } 
        
    if (new_val < mqs->min && !mq->flag) { // выход за допустимый минимум
        mq->flag = 2;
        meter->event_arch(me_min_start, timestamp, 0);
        mq->value_limit = new_val;
    } else
    if (new_val < mqs->min && mq->flag == 2) {
        if (new_val < mq->value_limit) {
            mq->value_limit = new_val;
        }
    } else
    if (new_val > mqs->min && mq->flag == 2) {
        mq->flag = 0;
        uint32_t val = (uint16_t)(mq->value_limit * scale);
        meter->event_arch(me_min_stop, timestamp, val&0xFFFF);
    }        
}

void METER_upd(uint32_t timestamp)
{
    //METER_emulate();
    
    uint8_t tariff = meter->tariff_conv(timestamp);
    if (tariff >= TARIFF_MAX) return;
    if (!tariff) tariff++;
    
    METER_NEW_DATA *nData = &meter->data->curr;
    // =============== интеграторы ===============
    uint8_t phase;
    METER_INTEGRATORS * itgr;
    METER_INTEGRATORS * itgr_sum;
    for (phase = 0; phase < PHASE_MAX; ++phase) {
        itgr = &meter->data->itgr[tariff][phase];
        itgr_sum = &meter->data->itgr[0][phase];
        if (nData->phase[phase].P >= 0) {
            itgr->P_p += abs(nData->phase[phase].P);
            itgr_sum->P_p += abs(nData->phase[phase].P);
            if (nData->phase[phase].Q >= 0) {
                itgr->Q1 += abs(nData->phase[phase].Q);
                itgr_sum->Q1 += abs(nData->phase[phase].Q);
            } else {
                itgr->Q4 += abs(nData->phase[phase].Q);
                itgr_sum->Q4 += abs(nData->phase[phase].Q);
            }
        } else {
            itgr->P_n += abs(nData->phase[phase].P);
            itgr_sum->P_n += abs(nData->phase[phase].P);
            if (nData->phase[phase].Q >= 0) {
                itgr->Q2 += abs(nData->phase[phase].Q);
                itgr_sum->Q2 += abs(nData->phase[phase].Q);
            } else {
                itgr->Q3 += abs(nData->phase[phase].Q);
                itgr_sum->Q3 += abs(nData->phase[phase].Q);
            }
        }
        if (nData->phase[phase].Q >= 0) {
            itgr->Q_p += abs(nData->phase[phase].Q);
            itgr_sum->Q_p += abs(nData->phase[phase].Q);
        } else {
            itgr->Q_n += abs(nData->phase[phase].Q);
            itgr_sum->Q_n += abs(nData->phase[phase].Q);
        }
        itgr->S = (uint64_t)sqrt((itgr->P_p + itgr->P_n) * (itgr->P_p + itgr->P_n) + (itgr->Q_p + itgr->Q_n) * (itgr->Q_p + itgr->Q_n));//abs(nData->phase[phase].S);
        itgr_sum->S = (uint64_t)sqrt((itgr_sum->P_p + itgr_sum->P_n) * (itgr_sum->P_p + itgr_sum->P_n) + (itgr_sum->Q_p + itgr_sum->Q_n) * (itgr_sum->Q_p + itgr_sum->Q_n));//abs(nData->phase[phase].S);
//        itgr->S += abs(nData->phase[phase].S);
//        itgr_sum->S += abs(nData->phase[phase].S);
    }
    
    // =============== архивы значений ===============
    uint8_t arch_index, arch_shift;
    struct tm tm_new;
    struct tm * tm_old;
    tm_old = localtime((time_t*)&timestamp);
    memmove(&tm_new, tm_old, sizeof(tm_new));
    for (arch_index = 0; arch_index < ARCH_DATA_MAX; ++arch_index) {
        METER_ARCH_DATA_SETTINGS * ads = &meter->data->aDataSet[arch_index];
        METER_ARCH_DATA * ad = &meter->data->aData[arch_index];
        if (!ads->timeout) {
            switch (ads->start_at_time_alignment) {
            case METER_TIME_ALIGMENT_min:  { ads->timeout = 60; break; }
            case METER_TIME_ALIGMENT_hour: { ads->timeout = 60 * 60; break; }
            case METER_TIME_ALIGMENT_day:  { ads->timeout = 60 * 60 * 24; break; }
            default: break;
            }
            if (!ads->timeout) break;
        }
        if (!ad->timestamp) {
            switch (ads->start_at_time_alignment) {
            case METER_TIME_ALIGMENT_min:  {
                tm_old->tm_sec = 0;
                ad->timestamp = mktime(tm_old);
                while (ad->timestamp <= timestamp - ads->timeout) {
                    ad->timestamp += ads->timeout;
                }
                break;
            }
            case METER_TIME_ALIGMENT_hour: {
                tm_old->tm_sec = 0;
                tm_old->tm_min = 0;
                ad->timestamp = mktime(tm_old);
                while (ad->timestamp <= timestamp - ads->timeout) {
                    ad->timestamp += ads->timeout;
                }
                break;
            }
            case METER_TIME_ALIGMENT_day:  {
                tm_old->tm_sec = 0;
                tm_old->tm_min = 0;
                tm_old->tm_hour = 0;
                ad->timestamp = mktime(tm_old);
                while (ad->timestamp <= timestamp - ads->timeout) {
                    ad->timestamp += ads->timeout;
                }
                break;
            }
            default: {ad->timestamp = timestamp; break;}
            }
        }
        uint32_t ts = ad->timestamp;
        tm_old = localtime((time_t*)&ts);
        uint8_t flag_need_save = 0;
        if (ad->timestamp > timestamp) {
            flag_need_save = 1;
        } else
        if ((timestamp - ad->timestamp) >= ads->timeout) {
            flag_need_save = 1;
        }
        switch (ads->start_at_time_alignment) {
        case METER_TIME_ALIGMENT_min:  { if (tm_old->tm_min  != tm_new.tm_min ) flag_need_save = 1; break; }
        case METER_TIME_ALIGMENT_hour: { if (tm_old->tm_hour != tm_new.tm_hour) flag_need_save = 1; break; }
        case METER_TIME_ALIGMENT_day:  { if (tm_old->tm_mday != tm_new.tm_mday) flag_need_save = 1; break; }
        default: break;
        }
        if (flag_need_save) {
            ad->timestamp = timestamp;
            if (meter->event_arch_data) {
                meter->event_arch_data(arch_index); // save
            }
            for (arch_shift = 0; arch_shift < ads->count_param; ++arch_shift) {
                ad->value[arch_shift].f32 = 0;
            }
        }
        for (arch_shift = 0; arch_shift < ads->count_param; ++arch_shift) {
            uint8_t tariff_arch = ads->param_id[arch_shift].tariff;
            uint8_t phase_arch  = ads->param_id[arch_shift].phase;
            itgr = &meter->data->itgr[tariff_arch][phase_arch];
            switch(ads->param_id[arch_shift].MDT) {
            case MDT_P_p: { 
                ad->value[arch_shift].f32 = meter->data->itgr[tariff_arch][phase_arch].P_p * meter->K;
                break;
            }
            case MDT_P_p_d: { 
                if (tariff != tariff_arch && tariff_arch) break;
                if (nData->phase[phase_arch].P < 0) break; 
                ad->value[arch_shift].f32 += abs(nData->phase[phase_arch].P) * meter->K;
                break;
            }
            case MDT_P_n: {
                ad->value[arch_shift].f32 = meter->data->itgr[tariff_arch][phase_arch].P_n * meter->K;
                break;
            }
            case MDT_P_n_d: { 
                if (tariff != tariff_arch && tariff_arch) break;
                if (nData->phase[phase_arch].P > 0) break; 
                ad->value[arch_shift].f32 += abs(nData->phase[phase_arch].P) * meter->K;
                break;
            }
            case MDT_Q_p: { 
                ad->value[arch_shift].f32 = meter->data->itgr[tariff_arch][phase_arch].Q_p * meter->K;
                break;
            }
            case MDT_Q_p_d: { 
                if (tariff != tariff_arch && tariff_arch) break;
                if (nData->phase[phase_arch].Q < 0) break; 
                ad->value[arch_shift].f32 += abs(nData->phase[phase_arch].Q) * meter->K;
                break;
            }
            case MDT_Q_n: {
                ad->value[arch_shift].f32 = meter->data->itgr[tariff_arch][phase_arch].Q_n * meter->K;
                break;
            }
            case MDT_Q_n_d: { 
                if (tariff != tariff_arch && tariff_arch) break;
                if (nData->phase[phase_arch].Q > 0) break; 
                ad->value[arch_shift].f32 += abs(nData->phase[phase_arch].Q) * meter->K;
                break;
            }
            case MDT_S: {
                ad->value[arch_shift].f32 = meter->data->itgr[tariff_arch][phase_arch].S * meter->K;
                break;
            }
            case MDT_S_d: { 
                if (tariff != tariff_arch && tariff_arch) break;
                ad->value[arch_shift].f32 += abs(nData->phase[phase_arch].S) * meter->K;
                break;
            }
            case MDT_Q1: {
                ad->value[arch_shift].f32 = meter->data->itgr[tariff_arch][phase_arch].Q1 * meter->K;
                break;
            }
            case MDT_Q2: {
                ad->value[arch_shift].f32 = meter->data->itgr[tariff_arch][phase_arch].Q2 * meter->K;
                break;
            }
            case MDT_Q3: {
                ad->value[arch_shift].f32 = meter->data->itgr[tariff_arch][phase_arch].Q3 * meter->K;
                break;
            }
            case MDT_Q4: {
                ad->value[arch_shift].f32 = meter->data->itgr[tariff_arch][phase_arch].Q4 * meter->K;
                break;
            }
            case MDT_U: {
                ad->value[arch_shift].f32 = nData->phase[phase_arch].U;
                break;
            }
            case MDT_Ulin: {
                ad->value[arch_shift].f32 = nData->phase[phase_arch].Ulin;
                break;
            }
            case MDT_I: {
                ad->value[arch_shift].f32 = nData->phase[phase_arch].I;
                break;
            }
            case MDT_F: {
                ad->value[arch_shift].f32 = nData->phase[phase_arch].F;
                break;
            }
            case MDT_cosFi: {
                ad->value[arch_shift].f32 = nData->phase[phase_arch].cosFi;
                break;
            }
            default: break;
            }
        }
    }
    
    // =============== показатели качества ===============
    METER_upd_Quality_F(nData->phase[0].F, timestamp, &meter->data->mQualitySet_F, &meter->data->mQuality_F
                        , ME_max_F_start, ME_max_F_stop, ME_min_F_start, ME_min_F_stop, 100.0);
    METER_upd_Quality_U(nData->phase[1].U, timestamp, &meter->data->mQualitySet_U, &meter->data->mQuality_U[1]
                        , ME_max_U1_start, ME_max_U1_stop, ME_min_U1_start, ME_min_U1_stop, 1.0);
    METER_upd_Quality_U(nData->phase[2].U, timestamp, &meter->data->mQualitySet_U, &meter->data->mQuality_U[2]
                        , ME_max_U2_start, ME_max_U2_stop, ME_min_U2_start, ME_min_U2_stop, 1.0);
    METER_upd_Quality_U(nData->phase[3].U, timestamp, &meter->data->mQualitySet_U, &meter->data->mQuality_U[3]
                        , ME_max_U3_start, ME_max_U3_stop, ME_min_U3_start, ME_min_U3_stop, 1.0);
    
    meter->data->timestamp = timestamp;
}

void METER_emulate()
{
#ifdef METER_EMULATE_DATA
    static uint32_t    prev_P[PHASE_MAX] = {0,0,0,0};
    static uint32_t    prev_Q[PHASE_MAX] = {0,0,0,0};
    static uint32_t    prev_S[PHASE_MAX] = {0,0,0,0};

    static METER_NEW_DATA curr;
    static uint8_t init_flag_local = 0;
    METER_NEW_DATA * dNew = &curr;
    uint8_t phase;
    float tmp1f;
    float tmp2f;
    if (!init_flag_local) {
        init_flag_local = 1;
        memset(&curr, 0, sizeof(METER_NEW_DATA));
        dNew->phase[0].U = 0;
        dNew->phase[1].U = 150;
        dNew->phase[2].U = 200;
        dNew->phase[3].U = 250;
    }
    for (phase = 0; phase < PHASE_MAX; ++phase) {
        dNew->phase[phase].P = 100*2000 - dNew->phase[phase].P;
        dNew->phase[phase].Q = 100*2000 - dNew->phase[phase].Q;
        tmp1f = dNew->phase[phase].P;
        tmp2f = dNew->phase[phase].Q;
        tmp1f *= tmp1f;
        tmp2f *= tmp2f;
        tmp1f += tmp2f;
        tmp1f = sqrt(tmp1f);
//        dNew->phase[phase].S = (uint32_t)sqrt(dNew->phase[phase].P * dNew->phase[phase].P + dNew->phase[phase].Q * dNew->phase[phase].Q);
        dNew->phase[phase].S = (uint32_t)tmp1f;
        if (dNew->phase[phase].U < 190 || dNew->phase[phase].U > 270) {
            dNew->phase[phase].U = 190;
        }
        dNew->phase[phase].U += 1;
        dNew->phase[phase].I = 3;
        if (dNew->phase[phase].F < 40 || dNew->phase[phase].F > 60) {
            dNew->phase[phase].F = 40;
        }
        dNew->phase[phase].F += 0.1;
        
//        if (!dNew->phase[phase].P) {
//            dNew->phase[phase].cosFi = 1.0;
//            continue;
//        }

        if(dNew->phase[phase].P || !prev_P[phase]) dNew->to_display[phase].Pi = (float)dNew->phase[phase].P * 1800 / (2000.0);  
        prev_P[phase] = dNew->phase[phase].P;
        if(dNew->phase[phase].Q || !prev_Q[phase]) dNew->to_display[phase].Qi = (float)dNew->phase[phase].Q * 1800 / (2000.0);  
        prev_Q[phase] = dNew->phase[phase].Q; 
        if(dNew->phase[phase].S || !prev_S[phase]) dNew->to_display[phase].Si = (float)dNew->phase[phase].S * 1800 / (2000.0);  
        prev_S[phase] = dNew->phase[phase].S;
        
        if (dNew->to_display[phase].Pi) {
            tmp1f = dNew->to_display[phase].Pi;
            tmp2f = dNew->to_display[phase].Si;
            tmp2f *= tmp2f;
            tmp2f += tmp1f * tmp1f;
            tmp2f = sqrt(tmp2f);
            tmp1f /= tmp2f;
        } else {
            tmp1f = 1.0;
        }
        dNew->phase[phase].cosFi = tmp1f;
        //dNew->phase[phase].cosFi = dNew->phase[phase].P / sqrt(dNew->phase[phase].P * dNew->phase[phase].P + dNew->phase[phase].Q * dNew->phase[phase].Q);
    }

    memmove(&meter->data->curr, &curr, sizeof(METER_NEW_DATA));
#endif
}
