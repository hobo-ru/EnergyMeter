#include "sensors_external.h"
#include <em_cmu.h>
#include <em_adc.h>
#include <em_gpio.h>
#include "main.h"
#include "archive.h"
//#include "event.h"
#include "u8g.h"

void SENSORS_EXT_INIT_adc()
{
    CMU_ClockEnable(cmuClock_GPIO, true);
    CMU_ClockEnable(cmuClock_ADC0, true);

    ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
    ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

    // Init common settings for both single conversion and scan mode
    init.timebase = ADC_TimebaseCalc(0);
    // Might as well finish conversion as quickly as possibly since polling for completion.
    // Set ADC clock to 7 MHz, use default HFPERCLK
    init.prescale = ADC_PrescaleCalc(7000000, 0);

    // WARMUPMODE must be set to Normal according to ref manual before entering EM2. In this example, the warmup time is not a big problem due to relatively infrequent polling. Leave at default NORMAL,
    ADC_Init(ADC0, &init);

    // Init for single conversion use, measure VDD/3 with 1.25 reference.
    singleInit.reference  = adcRef1V25;
    singleInit.input      = adcSingleInpCh1; // PE13
    singleInit.resolution = adcRes12Bit;

    // The datasheet specifies a minimum aquisition time when sampling vdd/3 32 cycles should be safe for all ADC clock frequencies
    singleInit.acqTime = adcAcqTime32;
    ADC_InitSingle(ADC0, &singleInit);

    ADC_Start(ADC0, adcStartSingle);  
}

void SENSORS_EXT_DEINIT_adc()
{
    CMU_ClockEnable(cmuClock_ADC0, false); 
}

uint32_t SENSORS_EXT_GET_adc()
{
    while (ADC0->STATUS & ADC_STATUS_SINGLEACT);
    return ADC_DataSingleGet(ADC0);
}

uint32_t SENSORS_EXT_Dispatcher_index = 0;
SENSORS_EXT_Current sensors_ext_curr;

void SENSORS_EXT_init()
{
    memset(&sensors_ext_curr, 0, sizeof(sensors_ext_curr));
}

void SENSORS_EXT_Dispatcher()
{
#ifdef DD_ADE7953
    sensors_ext_curr.adc_body = 4095;
    sensors_ext_curr.adc_cover = 4095;
    sensors_ext_curr.hall = 1;
#else    
    gpio_OFF_S_OPTO_BODY_SUP;
    gpio_INI_S_OPTO_COVER;
    gpio_INI_S_HALL;
    gpio_INI_S_OPTO_BODY;
    gpio_INI_S_OPTO_COVER_SUP;
    //gpio_INI_S_HALL_SUP;
    
    u8g_Delay(5);
    SENSORS_EXT_INIT_adc();
    u8g_Delay(5);
    
    sensors_ext_curr.adc_body = SENSORS_EXT_GET_adc(); // перепутано
    sensors_ext_curr.hall = gpio_GET_S_HALL;
    SENSORS_EXT_DEINIT_adc();
    gpio_OFF_S_OPTO_COVER_SUP;
    gpio_OFF_S_HALL;
    gpio_INI_S_OPTO_BODY_SUP;

    u8g_Delay(5);
    SENSORS_EXT_INIT_adc();
    u8g_Delay(5);
    
    gpio_OFF_S_OPTO_BODY_SUP;
    sensors_ext_curr.adc_cover = SENSORS_EXT_GET_adc(); // перепутано
    SENSORS_EXT_DEINIT_adc();
    gpio_OFF_S_OPTO_COVER;
#endif    

    if (sensors_ext_curr.adc_body < 1000 && sensors_ext_curr.state_body != 1) {
        if (sensors_ext_curr.state_body) {
            ARCH_emit_event(ME_btn_body_up, 0);
        }
        sensors_ext_curr.state_body = 1;
    }
    if (sensors_ext_curr.adc_body > 1200 && sensors_ext_curr.state_body != 2) {
        if (sensors_ext_curr.state_body) {
            ARCH_emit_event(ME_btn_body_down, 0);
        }
        sensors_ext_curr.state_body = 2;
    }
    if (sensors_ext_curr.adc_cover < 400 && sensors_ext_curr.state_cover != 1) {
        if (sensors_ext_curr.state_cover) {
            ARCH_emit_event(ME_btn_cover_up, 0);
        }
        sensors_ext_curr.state_cover = 1;
    }
    if (sensors_ext_curr.adc_cover > 500 && sensors_ext_curr.state_cover != 2) {
        if (sensors_ext_curr.state_cover) {
            ARCH_emit_event(ME_btn_cover_down, 0);
        }
        sensors_ext_curr.state_cover = 2;
    }
    if (sensors_ext_curr.hall == 0 && sensors_ext_curr.state_hall != 1) {
        if (sensors_ext_curr.state_hall) {
            ARCH_emit_event(ME_over_magnetic_field_start, 0);
        }
        sensors_ext_curr.state_hall = 1;
    }
    if (sensors_ext_curr.hall == 1 && sensors_ext_curr.state_hall != 2) {
        if (sensors_ext_curr.state_hall) {
            ARCH_emit_event(ME_over_magnetic_field_stop, 0);
        }
        sensors_ext_curr.state_hall = 2;
    }
    
/*    switch (SENSORS_EXT_Dispatcher_index++) {
    case 0: {
        gpio_INI_S_OPTO_COVER;
        gpio_INI_S_HALL;
        gpio_INI_S_OPTO_BODY;

        gpio_INI_S_OPTO_COVER_SUP;
        //gpio_INI_S_HALL_SUP;
        gpio_OFF_S_OPTO_BODY_SUP;
        SENSORS_EXT_INIT_adc();
        break;
    }
    case 1: {
        sensors_ext_curr.adc_cover = SENSORS_EXT_GET_adc();
        sensors_ext_curr.hall = gpio_GET_S_HALL;
//        SENSORS_EXT_DEINIT_adc();
        gpio_OFF_S_OPTO_COVER_SUP;
        gpio_INI_S_OPTO_BODY_SUP;
        break;
    }
    case 2: {
        SENSORS_EXT_INIT_adc();
        break;
    }
    case 3: {
        sensors_ext_curr.adc_body = SENSORS_EXT_GET_adc();
        SENSORS_EXT_DEINIT_adc();
        break;
    }
    default: {
        SENSORS_EXT_Dispatcher_index = 0;
        break;
    }
    }*/
}
