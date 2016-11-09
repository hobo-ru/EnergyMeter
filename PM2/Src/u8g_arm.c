#include "u8g_arm.h"
#include "main.h"
#include "dti.h"

#ifndef F_CPU
#define F_CPU 14000000UL
#endif

void NOP (uint32_t i)
{
    for (i=i;i>0;i--)
        asm volatile("nop");
}

void u8g_Delay(uint16_t val)
{
  int t;
  for (t=0; t < val; t++)
  {
    NOP(F_CPU/100000UL);
  }
}
void u8g_MicroDelay(void)
{
  //NOP(F_CPU/10000000UL);
}

void u8g_10MicroDelay(void)
{
  //NOP(F_CPU/1000000UL);
}

#define _BV(x) (1<<x)

uint8_t u8g_com_sw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
    int16_t i,j;
    switch(msg)
    {
    case U8G_COM_MSG_STOP:
        gpio_OFF_LCD_CS;
        gpio_OFF_LCD_SDO;
        gpio_OFF_LCD_SCK;
        gpio_OFF_LCD_A0;
        gpio_OFF_LCD_POWER;
        gpio_OFF_LCD_RES;
        gpio_RES_LCD_POWER;
        break;
        
    case U8G_COM_MSG_INIT:
        CMU_ClockEnable (cmuClock_GPIO, true);
        gpio_INI_LCD_CS;
        gpio_INI_LCD_SDO;
        gpio_INI_LCD_SCK;
        gpio_INI_LCD_A0;
        gpio_INI_LCD_POWER;
        gpio_INI_LCD_RES;
        gpio_SET_LCD_POWER;      
        break;
        
    case U8G_COM_MSG_ADDRESS: 
        if (arg_val) {
            gpio_SET_LCD_A0;
        } else {
            gpio_RES_LCD_A0;
        }
        //        u8g_10MicroDelay();
        break;
        
    case U8G_COM_MSG_RESET:
        if (arg_val) {
            gpio_SET_LCD_RES;
        } else {
            gpio_RES_LCD_RES;
        }
        //        u8g_10MicroDelay();
        break;
        
    case U8G_COM_MSG_WRITE_BYTE:
        //simple: just write one byte
        //        spiwrite(arg_val);
        DTI_Enable = 0;
        gpio_SET_LCD_SCK;
        gpio_RES_LCD_CS;
        //NOP (1);
        for (i=7; i>=0; i--) {
            if (arg_val & _BV(i))
                gpio_SET_LCD_SDO;
            else
                gpio_RES_LCD_SDO;
            //NOP (1);
            gpio_RES_LCD_SCK;
            //NOP (1);
            gpio_SET_LCD_SCK;
            //NOP (1);
        }
        gpio_SET_LCD_CS;
        //NOP (1);
        DTI_Enable = 1;
        break;
        
    case U8G_COM_MSG_WRITE_SEQ:
    case U8G_COM_MSG_WRITE_SEQ_P:
        {
            uint8_t *ptr = arg_ptr;
            DTI_Enable = 0;
            gpio_SET_LCD_SCK;
            gpio_RES_LCD_CS;
            //NOP (1);
            for (j=0; j<arg_val; j++) {
                for (i=7; i>=0; i--) {
                    if (ptr[j] & _BV(i))
                        gpio_SET_LCD_SDO;
                    else
                        gpio_RES_LCD_SDO;
                    //NOP (1);
                    gpio_RES_LCD_SCK;
                    //NOP (1);
                    gpio_SET_LCD_SCK;
                    //NOP (1);
                }
            }
            gpio_SET_LCD_CS;
            //NOP (1);
        }
        DTI_Enable = 1;
        break;

//    case U8G_COM_MSG_RELEASE_SPI:
////        gpio_OFF_LCD_CS;
//        gpio_OFF_LCD_SDO;
//        gpio_OFF_LCD_SCK;
//        break;
//
//    case U8G_COM_MSG_RESTORE_SPI:
////        gpio_INI_LCD_CS;
//        gpio_INI_LCD_SDO;
//        gpio_INI_LCD_SCK;
//        break; 
    default: break;
    }
    return 1;
}
