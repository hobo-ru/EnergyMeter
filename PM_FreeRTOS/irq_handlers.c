#include "em_chip.h"
#include "bsp_trace.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "e_meter.h"


extern uint32_t INP1_CNT, INP2_CNT;

//__weak void GPIO_ODD_IRQHandler(void);
void GPIO_ODD_IRQHandler(void)
{
    	uint32_t flag;
        
        flag = GPIO_IntGet();// & TD_GPIO_ODD_MASK;
        if (flag&INP1_MASK)
          INP1_CNT++;
#ifdef PW_CALIBRATE
         if (flag&INP2_MASK)
          INP2_CNT++;
#endif
         GPIO_IntClear(flag);
}