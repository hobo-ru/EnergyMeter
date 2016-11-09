#ifndef METER_H
#define METER_H

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TARIFF_MAX (1+4)
#define PHASE_MAX (1+3)
#define ARCH_DATA_MAX (2)
#define ARCH_DATA_PARAM_COUT_MAX (16)
#define ARCH_DATA_PROFILE_PARAM_COUT_MAX (4)

typedef enum {
    MDT_P_p             = 0x01,// �������� + ��������
    MDT_P_p_d           = 0x11,// �������� + ������
    
    MDT_P_n             = 0x02,// �������� - ��������
    MDT_P_n_d           = 0x12,// �������� - ������
    
    MDT_Q_p             = 0x03,// ���������� + ��������
    MDT_Q_p_d           = 0x13,// ���������� + ������
    
    MDT_Q_n             = 0x04,// ���������� - ��������
    MDT_Q_n_d           = 0x14,// ���������� - ������
    
    MDT_S               = 0x05,// ��������� ��������
    MDT_S_d             = 0x15,// ��������� ������
    
    MDT_Q1              = 0x25,// ���������� ���������� 1 ��������
    MDT_Q2              = 0x26,// ���������� ���������� 2 ��������
    MDT_Q3              = 0x27,// ���������� ���������� 3 ��������
    MDT_Q4              = 0x28,// ���������� ���������� 4 ��������
    
    MDT_U               = 0x33,// ���������� ���������
    MDT_Ulin            = 0x34,// ���������� ����� ������� 1-2 2-3 3-1
    MDT_I               = 0x35,// ���
    MDT_F               = 0x36,// �������
    MDT_cosFi           = 0x37,// cosFi
} METER_DATA_TYPE;

typedef enum {
    ME_update_fw        = 0x01, // ���������� ��������
    ME_back_fw          = 0x02, // ������� � ��������� ��������
    
    ME_update_rts       = 0x09, // ��������� ������� ��
    ME_update_rts_old   = 0x0A, // ������� ������� �
    
    ME_tariff_new       = 0x11, // ����� ��������� ����������
    ME_k_new            = 0x12, // ����� ������������ �������������
    ME_quality_set_new  = 0x13, // ����� �������� ���������� �������� ������������� �������
    ME_load_set_new     = 0x14, // ����� ������� ��������

    ME_btn_cover_up     = 0x21, // �������� ������� ������
    ME_btn_cover_down   = 0x22, // �������� ������� ������
    ME_btn_body_up      = 0x23, // �������� �������
    ME_btn_body_down    = 0x24, // �������� �������
    ME_over_magnetic_field_start = 0x25, // ������    ����������� ����������������� ���������� ����������� (������:) )
    ME_over_magnetic_field_stop  = 0x26, // ��������� ����������� ����������������� ���������� ����������� (������:) )

    ME_over_load        = 0x31, // ���������� �������� (���������� ���� - ������ ����)
    ME_under_load       = 0x32, // ����������� �������� ����� ���������� (��������� ���� - ��������� ����)

    ME_clear_values     = 0x39, // ����� ��������� (������������)
    ME_clear_arch_data1 = 0x3A, // ������� ������ ������ 1
    ME_clear_arch_data2 = 0x3B, // ������� ������ ������ 2
    ME_clear_arch_event = 0x3C, // ������� ������� �������
    
    ME_set_arch_data1   = 0x41, // ����� ��������� ��� ������� ������ 1
    ME_set_arch_data2   = 0x42, // ����� ��������� ������� ��������
    
    //�������� ����
    ME_max_U1_start     = 0x50, // ������     ���������� ��������� ���������� ���� 1
    ME_max_U1_stop      = 0x51, // ���������� ���������� ��������� ���������� ���� 1
    ME_min_U1_start     = 0x52, // ������     ���������� ��������  ���������� ���� 1
    ME_min_U1_stop      = 0x53, // ���������� ���������� ��������  ���������� ���� 1
    ME_max_U2_start     = 0x54, // ������     ���������� ��������� ���������� ���� 2
    ME_max_U2_stop      = 0x55, // ���������� ���������� ��������� ���������� ���� 2
    ME_min_U2_start     = 0x56, // ������     ���������� ��������  ���������� ���� 2
    ME_min_U2_stop      = 0x57, // ���������� ���������� ��������  ���������� ���� 2
    ME_max_U3_start     = 0x58, // ������     ���������� ��������� ���������� ���� 3
    ME_max_U3_stop      = 0x59, // ���������� ���������� ��������� ���������� ���� 3
    ME_min_U3_start     = 0x5A, // ������     ���������� ��������  ���������� ���� 3
    ME_min_U3_stop      = 0x5B, // ���������� ���������� ��������  ���������� ���� 3
    ME_max_F_start      = 0x5C, // ������     ���������� ��������� �������
    ME_max_F_stop       = 0x5D, // ���������� ���������� ��������� �������
    ME_min_F_start      = 0x5E, // ������     ���������� ��������  �������
    ME_min_F_stop       = 0x5F, // ���������� ���������� ��������  �������
    
    ME_power_on         = 0x71, // ��������� �������� (0xFFFF = ������������ ��������)
    ME_power_off        = 0x72, // ���������� ��������
    ME_power_up_f1      = 0x73, // ���������� ���� 1 ���������� ���������� ��� ������� ���� � ������������� �����
    ME_power_down_f1    = 0x74, // ���������  ���� 1 ���������� ���������� ��� ������� ���� � ������������� �����
    ME_power_up_f2      = 0x75, // ���������� ���� 2 ���������� ���������� ��� ������� ���� � ������������� �����
    ME_power_down_f2    = 0x76, // ���������  ���� 2 ���������� ���������� ��� ������� ���� � ������������� �����
    ME_power_up_f3      = 0x77, // ���������� ���� 3 ���������� ���������� ��� ������� ���� � ������������� �����
    ME_power_down_f3    = 0x78, // ���������  ���� 3 ���������� ���������� ��� ������� ���� � ������������� �����
    
    ME_direction_I1     = 0x79, // ����� ����������� ���� ���� 1
    ME_direction_I2     = 0x7A, // ����� ����������� ���� ���� 2
    ME_direction_I3     = 0x7B, // ����� ����������� ���� ���� 3
} METER_EVENT;

typedef enum {
    METER_TIME_ALIGMENT_not = 0,
    METER_TIME_ALIGMENT_min = 1,
    METER_TIME_ALIGMENT_hour= 2,
    METER_TIME_ALIGMENT_day = 3,
} METER_TIME_ALIGMENT;      // ��� �������� ������������ �������

#pragma pack(push, 1)
typedef struct {
    uint32_t timestamp;                         // ����� ������ ������� ������ ��� ������
    union {                                     // ������ ��� ������ � �����
//        uint32_t u32;
        float    f32;
    } value [ARCH_DATA_PARAM_COUT_MAX];         // ��������
    uint8_t  reserve[3];
    uint8_t  crc;                               // ����������� ����� ������ ��������� ����
} METER_ARCH_DATA;    // ����� � �������

typedef struct {
    uint32_t timestamp;                         // ����� ������ ������� ������ ��� ������
    union {                                     // ������ ��� ������ � �����
//        uint32_t u32;
        float    f32;
    } value [ARCH_DATA_PROFILE_PARAM_COUT_MAX]; // ��������
    uint8_t  reserve[3];
    uint8_t  crc;                               // ����������� ����� ������ ��������� ����
} METER_ARCH_PROFILE_DATA;    // ����� � ���������

typedef struct {
    uint32_t timestamp;                         // ����� �������
    uint8_t  event;                             // id �������
    uint16_t value;                             // �������� ��������� ������� 
    uint8_t  crc;                               // ����������� ����� ������ ��������� ����
} METER_ARCH_EVENT;   // ����� � ���������
#pragma pack(pop)

//#pragma pack(push, 1)

typedef struct {
    uint64_t P_p;   // A+ �������� ������� ������� �����������
    uint64_t P_n;   // A- �������� ������� ��������� �����������
    uint64_t Q_p;   // R+ ���������� ������� ������� �����������
    uint64_t Q_n;   // R- ���������� ������� ��������� �����������
    uint64_t S;     //    ������ �������
    
    uint64_t Q1;    // R1 ���������� ������� 1-�� ���������
    uint64_t Q2;    // R2 ���������� ������� 2-�� ���������
    uint64_t Q3;    // R3 ���������� ������� 3-�� ���������
    uint64_t Q4;    // R4 ���������� ������� 4-�� ���������
} METER_INTEGRATORS;// �����������

typedef struct {
    int32_t P;      // �������� �������
    int32_t Q;      // ���������� �������
    int32_t S;      // ������ �������
    float   U;      // ���������� ������ RMS
    float   Ulin;   // ���������� �������� RMS
    float   I;      // ��� RMS
    float   F;      // ������� ����
    float   cosFi;  // cosFi
} METER_INSTANTS;   // ������� ��������

typedef struct {
    uint8_t MDT;    // METER_DATA_TYPE
    uint8_t tariff; // �����, 0-�����
    uint8_t phase;  // ����, 0-�����
    uint8_t reserv;
} METER_ARCH_PARAM_CFG; // ��������� ��������� ������

typedef struct {
    uint32_t timeout;                   // ����� �� ������� ���������� ���������
    uint8_t  start_at_time_alignment;   // ���� ������������� ������������ �������
    uint8_t  count_param;               // ���-�� ����������
    METER_ARCH_PARAM_CFG param_id[ARCH_DATA_PARAM_COUT_MAX];
} METER_ARCH_DATA_SETTINGS;   // ��������� ������ � �������

typedef struct {
    float    nominal;       // ����������� �������� // = 50 Hz // 220 V
    uint32_t time_integral;// ����� ���������� ���
    float    max;           // ������� �������
    float    min;           // ������ �������
} METER_QUALITY_SETTINGS; // ��������� ������ � ���������

typedef struct {
    float    value_to_aver; // ����������� �������� (�����)
    uint32_t count_to_aver; // ���������� ����
    float    value_d;       // ����������
    uint32_t flag;          // ���� ������ 0=����� 1=���������� 2=������
    float    value_limit;   // ����������� ����������� �������� ���������
} METER_QUALITY;    // ������� �������� ��������

typedef struct {
    METER_INSTANTS phase[PHASE_MAX];
    struct {
        float Pi;  // ��������   �������� 
        float Qi;  // ���������� ��������
        float Si;  // ������     ��������
    } to_display[PHASE_MAX]; // ��� ����������� �� �������
} METER_NEW_DATA;

typedef struct {
    uint32_t timestamp;                                 // ����� ���������� ���������� ������
    METER_INTEGRATORS itgr[TARIFF_MAX][PHASE_MAX];      // ����������� �� ����� ������� + �������
    METER_NEW_DATA curr;                                // ������� �������� (��� ����������� �� ������)
    METER_ARCH_DATA_SETTINGS aDataSet[ARCH_DATA_MAX];   // ��������� ������� � �������
    METER_ARCH_DATA aData[ARCH_DATA_MAX];               // ����� � �������
    METER_QUALITY_SETTINGS mQualitySet_F;               // ��������� �������� // �������
    METER_QUALITY_SETTINGS mQualitySet_U;               // ��������� �������� // ���������� ������
    METER_QUALITY mQuality_F;                           // ����� � ��������� // �������
    METER_QUALITY mQuality_U[PHASE_MAX];                // ����� � ��������� // ���������� ������
} METER_DATA;

//#pragma pack(pop)

typedef void (*METER_update)(uint32_t timestamp);
//typedef float (*METER_i32_to_f32)(int32_t data);
typedef uint8_t (*METER_tariff_from_time)(uint32_t timestamp);
typedef void (*METER_event_data)(uint32_t data);
typedef void (*METER_event)(METER_EVENT me, uint32_t timestamp, uint16_t val);
typedef struct {
    //METER_update update;                // ������������� ��������� �� curr
    float K;
//    METER_i32_to_f32 conv_energ;        // ����������� �������
    METER_tariff_from_time tariff_conv; // �������� ����� �� �������
    METER_event_data event_arch_data;   // ���������� ������ � �����
    METER_event event_arch;             // ���������� ������ � ������ �������
    METER_DATA * data;
} METER;

extern METER * meter;

void METER_init();
void METER_upd(uint32_t timestamp);

#ifdef __cplusplus
}
#endif

#endif // METER_H
