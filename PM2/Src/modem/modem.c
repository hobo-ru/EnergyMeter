#include "modem.h"
#include "mqueue.h"
//#include "axem.h"
#include "electro_v5.h"
#include "scheduler.h"
#include "pwr_managment.h"
#include "modem_uart.h"
//#include "event.h"
#include "dti.h"
#include "tariff.h"
#include <string.h>
#include "main.h"
#include "archive.h"
#include "systimer.h"
#include "em_wdog.h"

#define REPEAT_CMD_BUFFER_SIZE          16
#define REPEAT_PAYLOAD_BUFFER_SIZE      32
#define MODEM_SEND_PROTOCOL             AXEM_SEND_BY_C_CMD

uint8_t DL_Mode = SLIP_CRX;
uint8_t AXSEM_ACK_timeout = 0;
uint8_t AXSEM_packets_to_send = 0;

modem_states_e ModemState = MODEM_READY;

uint64_t TarifBitMask[4];

typedef union {
	time_t time;
	uint8_t buff[sizeof(time_t)];
}electro_time_u;

typedef union {
	uint16_t limit;
	uint8_t buff[sizeof(uint16_t)];
}electro_limit_u;

void AXSEM_Reset(void)
{
    AXSEM_ACK_timeout = 0;
    gpio_INI_AXSEM_RESET;
    ModemUART_Clear();
    {uint32_t dly = 5*1000; while(dly) dly--;}
    //Delay100Ms(5);
    gpio_SET_AXSEM_RESET;
    {uint32_t dly = 5*1000; while(dly) dly--;}
    //Delay100Ms(5);
}

void AXSEM_Off(void)
{
    AXSEM_ACK_timeout = 0;
    gpio_INI_AXSEM_RESET;
    ModemUART_Clear();
    {uint32_t dly = 1000; while(dly) dly--;}
    //Delay100Ms(5);
    gpio_SET_AXSEM_RESET;
//    {uint32_t dly = 5*1000; while(dly) dly--;}
    //Delay100Ms(5);
}

extern uint8_t test_send_sleep;
void Modem_SetDLPacket(SLIP_BUF * slip)
//void Modem_SetDLPacket(uint8_t cmd, uint8_t *data, uint8_t len, uint8_t dti_interface)
{
//    uint8_t cmd; 
//    uint8_t *data; 
    uint8_t len; 
//    uint8_t dti_interface;
//    cmd = slip->cmd;
//    data = slip->payload;
    len = slip->len;
//    dti_interface = slip->dti_interface;
    
    uint8_t  *packet = slip->payload;
//    size_t lenght = len;
	electro_time_u        electro_time;
	electro_limit_u       electro_limit;
	int i;
    static uint8_t tarif_pack = 0;
    //ModemState = MODEM_READY;
    switch (slip->cmd) {
    case AXEM_CMD_ECHO: {
        mQueue_PutMsg(STRIJ_LED_SHORT_ON_EVENT, 0);
        break;
    }
    case AXEM_CMD_SEND_GET_SERIAL: {
        if (slip->len != 4) break;
        Settings.ProtC_ID = (slip->payload[0] << 24) + (slip->payload[1] << 16) + (slip->payload[2] << 8) + slip->payload[3];
        break;
    }
    case AXEM_CMD_SEND_NBFi: {
        if (DL_Mode == SLIP_FASTDL) {
            ModemState = MODEM_READY;
        } else {
            ModemState = MODEM_WAIT_ACK;
        }
        AXSEM_ACK_timeout = 0;
        //mQueue_PutMsg(STRIJ_LED_SHORT_ON_EVENT, 0);
        break;
    }
    case AXEM_CMD_SET_MODE: {
        if (len < 1) break;
        uint8_t dl_mode = slip->payload[0];
//        DL_Mode = data[0];
//        if (!CheckPowerLine() && DL_Mode != SLIP_DRX) {
//            uint8_t DL_Mode_tmp = SLIP_DRX;
//            ModemUART_SLIP_Send(AXEM_CMD_SET_MODE, &DL_Mode_tmp, 1);
//        }
        if (dl_mode == SLIP_DRX) dl_mode = SLIP_CRX;
        if (dl_mode != DL_Mode) {
            ModemUART_SLIP_Send(AXEM_CMD_SET_MODE, &DL_Mode, 1);
        }
        ModemState = MODEM_READY;
        AXSEM_ACK_timeout = 0;
        if (CheckPowerLine()) {
            ModemUART_SLIP_Send(AXEM_CMD_GET_PACKETS_TO_SEND, 0, 0);
        } else {
            AXSEM_packets_to_send = 0;
        }
        break;
    }
    case AXEM_CMD_DL_PACKET: {
        uint8_t cmd_DL = *packet++; len--;
        if (DL_Mode == SLIP_FASTDL) {
            Schedule_PutUniqe( RESET_FAST_DL, __time32(0) + 180);
            ModemState = MODEM_WAIT_ACK;
        } else {
            ModemState = MODEM_READY;
        }
        AXSEM_ACK_timeout = 0;
        switch (cmd_DL) {
        case EL_V5_DTI_echo: {
            ModemUART_SLIP_Send_F(ModemUART_PutTx, AXEM_CMD_SEND_NBFi, slip->payload, slip->len);
            break;
        }
        case EL_V5_DTI: {
            if (!PwrManager_GetStatus()) break;
            //DTI_set_Interface_no485();
//            DTI_Poll_cb(cmd, data, len, AX_FAST_DL_INTERFACE);
            DTI_Poll_cb(slip);
            break;
        }
        case EL_V5_SET_RADIO_COMMUNICATION: {
            uint8_t radio_header = *packet++;
            if (radio_header == 1) {
                mQueue_PutMsg( SET_FAST_DL, 0 );
            } else
            if (radio_header == 0) {
                mQueue_PutMsg( RESET_FAST_DL, 0 );
            }
            break;
        }
        case EL_V5_SET_TARIFF_EXT: {
            uint8_t part_index = *packet++; len--;
            V5_TARIFF_HEADER * vth = (V5_TARIFF_HEADER *)packet; packet+=2; len-= 2;
            
            break;
        }
        case EL_V5_SET_PROFILE: {
            if (slip->len != sizeof(V5_SET_PROFILE)) break;
            V5_SET_PROFILE *profile = (V5_SET_PROFILE *)slip->payload;
            METER_ARCH_DATA_SETTINGS * mads = &meter->data->aDataSet[1];
            mads->timeout = ((((uint16_t)profile->timeout_hi) << 4) + profile->timeout_lo) * 60;
            if (!profile->alignment) {
                mads->start_at_time_alignment = 0;
            } else {
                mads->start_at_time_alignment = profile->alignment + 1;
            }
            need_send_profile = profile->need_send;
            for (int i = 0; i < 4; ++i) {
                mads->param_id[i].tariff = profile->pi[i].tariff;
                mads->param_id[i].phase = profile->pi[i].obis_C / 20;
                switch (profile->pi[i].obis_C % 20) {
                case 1: mads->param_id[i].MDT = MDT_P_p;    break;
                case 2: mads->param_id[i].MDT = MDT_P_n;    break;
                case 3: mads->param_id[i].MDT = MDT_Q_p;    break;
                case 4: mads->param_id[i].MDT = MDT_Q_n;    break;
                case 9: mads->param_id[i].MDT = MDT_S;      break;
                case 5: mads->param_id[i].MDT = MDT_Q1;     break;
                case 6: mads->param_id[i].MDT = MDT_Q2;     break;
                case 7: mads->param_id[i].MDT = MDT_Q3;     break;
                case 8: mads->param_id[i].MDT = MDT_Q4;     break;
                case 12:mads->param_id[i].MDT = MDT_U;      break;
                case 11:mads->param_id[i].MDT = MDT_I;      break;
                case 14:mads->param_id[i].MDT = MDT_F;      break;
                case 13:mads->param_id[i].MDT = MDT_cosFi;  break;
                default:mads->param_id[i].MDT = 0;          break;
                }
            }
            ARCH_emit_event(ME_set_arch_data2, 0);
            mQueue_PutMsg( SAVE_SETTINGS, 0 );
            break;
        }
        case EL_V5_SET_TARIF: {
            mQueue_PutMsg(STRIJ_LED_SHORT_ON_EVENT, 0);
            uint8_t t_num = packet[0] >> 6;
            tarif_pack |= 1 << t_num;
            TarifBitMask[t_num] = 0;
            for(uint8_t i = 0; i != 6; i++) TarifBitMask[t_num] += (((uint64_t)packet[6 - i]) << (i*8));
            if(tarif_pack == 7) {
                tarif_pack = 0;
                if((TarifBitMask[0]^TarifBitMask[1]^TarifBitMask[2]) == 0xffffffffffff) {
                    uint8_t trf[12];
                    memset(trf, 0, 12);
                    uint8_t tariff;
                    for(uint8_t i = 0; i <= 48; i++) {
                        for(tariff = 0; tariff < 4; tariff++) {
                            if (((TarifBitMask[tariff] >> i) & 1) == 1) {
                                trf[i/4] |= (tariff) << (i%4) * 2;
                                break;
                            }
                        }
                    }
                    TARIFF_set(trf, sizeof(trf));
                    mQueue_PutMsg( STORE_NEW_TARIF, 0 );
                }
            }
            //mQueue_PutMsg( SEND_TARIFS_EVENT, 0 );
            break;
        }
        case EL_V5_SET_TIME: {
            mQueue_PutMsg(STRIJ_LED_SHORT_ON_EVENT, 0);
            i = sizeof(time_t);
            while( i ) {
                electro_time.buff[i-1] = *packet;
                i--;
                packet++;
            }
            mQueue_PutMsg( TIME_SYNC_EVENT, electro_time.time );
            break;
        }
        case EL_V5_SET_LIMIT: {
            mQueue_PutMsg(STRIJ_LED_SHORT_ON_EVENT, 0);
            i = sizeof(uint16_t);
            while( i) {
                electro_limit.buff[i-1] =  *packet;
                i--;
                packet++;
            }
            Settings.Power_limit = electro_limit.limit;
            mQueue_PutMsg( SAVE_SETTINGS, 0);
            mQueue_PutMsg( SET_PWR_LIMIT_EVENT, electro_limit.limit );
            break;
        }
        default: break;
        }
    }
    case AXEM_CMD_GET_PACKETS_TO_SEND: {
//        if (test_send_sleep == 2) test_send_sleep = 0;
        if (len < 1) break;
        AXSEM_packets_to_send = packet[0];
        break;
    }
    default: break;
    }
}

bool Modem_IsSending( void )
{
	return (ModemState == MODEM_SEND_PAYLOAD || AXSEM_packets_to_send) ? true : false;
}

bool Modem_IsWaitingAck( void )
{
	return (ModemState == MODEM_WAIT_ACK || AXSEM_ACK_timeout >= 3) ? true : false;
}

//bool Event_Send( ArchiveEvent_e event, time_t time )
//{
//    uint8_t Buffer[8];
////    ModemState = MODEM_SEND_PAYLOAD;
//    Electro_MakeEvent(event, time, Buffer, sizeof(Buffer));
//    //ModemUART_SLIP_Send(AXEM_CMD_SEND_NBFi, Buffer, sizeof(Buffer));
//    return true;
//}

bool ModemUART_SLIP_Send(uint8_t cmd, uint8_t *payload, uint8_t len)
{
    if (!CheckPowerLine() && cmd != AXEM_CMD_SET_MODE) {
        return false;
    }
    if (DL_Mode == SLIP_FASTDL && cmd == AXEM_CMD_SEND_NBFi) {
        return false;
    }
    if (!PwrManager_GetStatus() && cmd == AXEM_CMD_SEND_NBFi) {
        return false;
    }
    if (cmd == AXEM_CMD_SEND_NBFi) {
        ModemState = MODEM_SEND_PAYLOAD;
    } else {
//        ModemState = MODEM_WAIT_ACK;
    }
//    if (cmd == AXEM_CMD_SET_MODE && len == 1) { // test
//        return false;
//    }
    ModemUART_SLIP_Send_F(ModemUART_PutTx, cmd, payload, len);
    return true;
}
