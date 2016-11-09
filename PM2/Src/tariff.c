#include "tariff.h"

#include <string.h>
#include "dti.h"

//TARIFF_TABLE tariff_table;
uint8_t tariff_curr;

TARIFF_TABLE_EXT ttable;

void TARIFF_init()
{
//    int i;
//    for (i = 0; i < TARIFF_TABLE_ITEMS; ++i) {
//        tariff_table.h2[i].hh0 = 0;
//        tariff_table.h2[i].hh1 = 0;
//        tariff_table.h2[i].hh2 = 0;
//        tariff_table.h2[i].hh3 = 0;
//    }
    static uint8_t flag_reg = 0;
    if (!flag_reg) {
        flag_reg = 1;
        DTI_regProperty("pTariff", STRUCT_TYPE, (uint32_t)&ttable, 0);
    }
    memset(&ttable, 0, sizeof(TARIFF_TABLE_EXT));
}

uint8_t TARIFF_set(void *data, uint8_t data_len)
{
    if (sizeof(TARIFF_2hour) * TARIFF_TABLE_ITEMS != data_len) return sizeof(TARIFF_2hour) * TARIFF_TABLE_ITEMS;
    memmove(&ttable.tariff_month[0].workday, data, sizeof(TARIFF_2hour) * TARIFF_TABLE_ITEMS);
    return 0;
}

uint8_t TARIFF_from_zone(uint8_t z)
{
    uint8_t tl = z;
    uint8_t hh = tl % 4;
    tl /= 4;
    if (tl >= TARIFF_TABLE_ITEMS) {
        tl = TARIFF_TABLE_ITEMS  - 1;
    }
    switch (hh) {
    case 0: return ttable.tariff_month[0].workday[tl].hh0 + 1;
    case 1: return ttable.tariff_month[0].workday[tl].hh1 + 1;
    case 2: return ttable.tariff_month[0].workday[tl].hh2 + 1;
    case 3: return ttable.tariff_month[0].workday[tl].hh3 + 1;
    default : break;
    }
    return 0;
}

uint8_t TARIFF_from_time(uint32_t t)
{
    uint32_t tl = t / (60 * 30);
    tl = tl % TARIFF_ZONE_MAX;
    return TARIFF_from_zone(tl);
}

uint8_t TARIFF_is_use(uint8_t tariff)
{
    uint8_t z;
    for (z = 0; z < TARIFF_ZONE_MAX; ++z) {
        if (TARIFF_from_zone(z) == tariff) {
            return 1;
        }
    }
    return 0;
}

void TARIFF_set_curr_time(uint32_t t)
{
    tariff_curr = TARIFF_from_time(t);
}

uint8_t TARIFF_get_curr()
{
    return tariff_curr;
}
