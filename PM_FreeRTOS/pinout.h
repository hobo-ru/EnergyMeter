#ifndef PINOUT_H
#define PINOUT_H

#define Res_PORT                gpioPortC           /**< Res Data port */
#define Res_BIT                 13					/**< Res Data bit */
#define Res_MASK                (1 << Res_BIT)		/**< Res Data mask */

#define INP1_PORT			Res_PORT
#define INP1_BIT			Res_BIT
#define INP1_MASK			Res_MASK

#define RX_BIT                  5		
#define RX_MASK                 (1 << RX_BIT)		/**< RS232 RX (In) mask */

#define INP2_PORT			RX_PORT
#define INP2_BIT			RX_BIT
#define INP2_PIN			RX_PIN
#define INP2_MASK			RX_MASK

#define	SDA_PORT                gpioPortA           /**< I2C Data port */
#define	SDA_BIT                 0					/**< I2C Data bit */
#define	SDA_MASK                (1 << SDA_BIT)		/**< I2C Data mask */
#define	SDA_PIN                 1					/**< I2C Data pin */

#define BFL_PORT				SDA_PORT										// +3B_FL line (индикатор наличия напряжения внешнего электропитания, переключить "как вход" на время неактивного состояния LCD)
#define BFL_BIT					SDA_BIT
#define BFL_PIN					SDA_PIN
#define BFL_MASK				SDA_MASK

#define	SCL_PORT                gpioPortA           /**< I2C Clock port */
#define	SCL_BIT                 1					/**< I2C Clock bit */
#define	SCL_MASK                (1 << SCL_BIT)		/**< I2C Clock mask */
#define SCL_PIN                 2					/**< I2C Clock pin */

#define POW_PORT			SCL_PORT											// LCD_POW
#define POW_BIT				SCL_BIT
#define POW_MASK			SCL_MASK
#define EXT_PHASE_A_SENS_INIT	GPIO_PinModeSet (BFL_PORT, BFL_BIT, gpioModeInput, 1)

#define ROFF_PORT			USR1_PORT											// RELAY OFF
#define ROFF_BIT			USR1_BIT
#define ROFF_MASK			USR1_MASK
#define RELAY_OFF_SET		GPIO_PinOutSet (ROFF_PORT, ROFF_BIT)
#define RELAY_OFF_CLEAR		GPIO_PinOutClear (ROFF_PORT, ROFF_BIT)

#define	USR1_PORT               gpioPortC           /**< User GPIO 1 port */
#define	USR1_BIT                15					/**< User GPIO 1 bit */
#define	USR1_MASK               (1 << USR1_BIT)		/**< User GPIO 1 mask */
#define	USR1_PIN                24					/**< User GPIO 1 pin */
#define PE_DATA_PORT	TIM2_PORT
#define PE_DATA_PIN		TIM2_BIT
#define PE_CLK_PORT		IO1_PORT
#define PE_CLK_PIN		IO1_BIT
#define PE_STR_PORT		ADC1_PORT
#define PE_STR_PIN		ADC1_BIT

#define	ADC0_PORT               gpioPortC           /**< Analog to Digital Converter 0 port */
#define	ADC0_BIT                0					/**< Analog to Digital Converter 0 bit */
#define	ADC0_MASK               (1 << ADC0_BIT)		/**< Analog to Digital Converter 0 mask */
#define	ADC0_PIN                5					/**< Analog to Digital Converter 0 pin */

#define	ADC1_PORT               gpioPortC           /**< Analog to Digital Converter 1 port */
#define	ADC1_BIT                1					/**< Analog to Digital Converter 1 bit */
#define	ADC1_MASK               (1 << ADC1_BIT)		/**< Analog to Digital Converter 1 mask */
#define	ADC1_PIN                6					/**< Analog to Digital Converter 1 pin */


#define ST765R_GPIO_CS   		ADC0_BIT										// CS
#define ST765R_GPIO_CS_PORT   	ADC0_PORT
//#define ST765R_GPIO_RST 		SCL_BIT
//#define ST765R_GPIO_RST_PORT 	SCL_PORT
#define ST765R_GPIO_CLK  		IO1_BIT											// SCL (SCK)
#define ST765R_GPIO_CLK_PORT  	IO1_PORT
#define ST765R_GPIO_A0   		SDA_BIT											// A0
#define ST765R_GPIO_A0_PORT   	SDA_PORT
#define ST765R_GPIO_MOSI 		TIM2_BIT										// SDI (SDO)
#define ST765R_GPIO_MOSI_PORT 	TIM2_PORT

/**I/O definitions */
#define Res_PORT                gpioPortC           /**< Res Data port */
#define Res_BIT                 13					/**< Res Data bit */
#define Res_MASK                (1 << Res_BIT)		/**< Res Data mask */

#define	SDA_PORT                gpioPortA           /**< I2C Data port */
#define	SDA_BIT                 0					/**< I2C Data bit */
#define	SDA_MASK                (1 << SDA_BIT)		/**< I2C Data mask */
#define	SDA_PIN                 1					/**< I2C Data pin */

#define	SCL_PORT                gpioPortA           /**< I2C Clock port */
#define	SCL_BIT                 1					/**< I2C Clock bit */
#define	SCL_MASK                (1 << SCL_BIT)		/**< I2C Clock mask */
#define SCL_PIN                 2					/**< I2C Clock pin */

#define	NIRQ_RF_PORT            gpioPortA           /**< RF IRQ port */
#define	NIRQ_RF_BIT             2					/**< RF IRQ bit */
#define	NIRQ_RF_MASK            (1 << NIRQ_RF_BIT)	/**< RF IRQ mask */
#define NIRQ_RF_PIN             3					/**< RF IRQ pin */

/** @} (end addtogroup Port_A) */

/***************************************************************************//**
 * @addtogroup Port_B Port B
 * @brief TD1202 module port B
 * @{
 ******************************************************************************/

#define	LFXTAL_P_PORT           gpioPortB           /**< Positive 32768 Hz Crystal port */
#define	LFXTAL_P_BIT            7					/**< Positive 32768 Hz Crystal bit */
#define	LFXTAL_P_MASK           (1 << LFXTAL_P_BIT)	/**< Positive 32768 Hz Crystal mask */
#define	LFXTAL_P_PIN            7					/**< Positive 32768 Hz Crystal pin */

#define	LFXTAL_N_PORT           gpioPortB           /**< Negative 32768 Hz Crystal port */
#define	LFXTAL_N_BIT            8					/**< Negative 32768 Hz Crystal bit */
#define	LFXTAL_N_MASK           (1 << LFXTAL_N_BIT)	/**< Negative 32768 Hz Crystal mask */
#define	LFXTAL_N_PIN            8					/**< Negative 32768 Hz Crystal pin */

#define	DAC0_PORT               gpioPortB           /**< Digital to Analog Converter 0 port */
#define	DAC0_BIT                11					/**< Digital to Analog Converter 0 bit */
#define	DAC0_MASK               (1 << DAC0_BIT)		/**< Digital to Analog Converter 0 mask */
#define	DAC0_PIN                10					/**< Digital to Analog Converter 0 pin */

#define	USR0_PORT               gpioPortB           /**< User GPIO 0 port */
#define	USR0_BIT                13					/**< User GPIO 0 bit */
#define	USR0_MASK               (1 << USR0_BIT)		/**< User GPIO 0 mask */
#define	USR0_PIN                12					/**< User GPIO 0 pin */

#define	GP2_PORT                gpioPortB           /**< RF GPIO 2 port */
#define	GP2_BIT                 14					/**< RF GPIO 2 bit */
#define	GP2_MASK                (1 << GP2_BIT)		/**< RF GPIO 2 mask */
#define	GP2_PIN                 13					/**< RF GPIO 2 pin */

/** @} (end addtogroup Port_B) */

/***************************************************************************//**
 * @addtogroup Port_C Port C
 * @brief TD1202 module port C
 * @{
 ******************************************************************************/

#define	ADC0_PORT               gpioPortC           /**< Analog to Digital Converter 0 port */
#define	ADC0_BIT                0					/**< Analog to Digital Converter 0 bit */
#define	ADC0_MASK               (1 << ADC0_BIT)		/**< Analog to Digital Converter 0 mask */
#define	ADC0_PIN                5					/**< Analog to Digital Converter 0 pin */

#define	ADC1_PORT               gpioPortC           /**< Analog to Digital Converter 1 port */
#define	ADC1_BIT                1					/**< Analog to Digital Converter 1 bit */
#define	ADC1_MASK               (1 << ADC1_BIT)		/**< Analog to Digital Converter 1 mask */
#define	ADC1_PIN                6					/**< Analog to Digital Converter 1 pin */

#if (MODULE_REVISION == REVISION_VBC4)
#define	GP1_PORT                gpioPortC           /**< RF GPIO 1 port */
#define	GP1_BIT                 15					/**< RF GPIO 1 bit */
#define	GP1_MASK                (1 << GP1_BIT)		/**< RF GPIO 1 mask */
#define	GP1_PIN                 24					/**< RF GPIO 1 pin */
#else
#define	GP1_PORT                gpioPortC           /**< RF GPIO 1 port */
#define	GP1_BIT                 13					/**< RF GPIO 1 bit */
#define	GP1_MASK                (1 << GP1_BIT)		/**< RF GPIO 1 mask */
#define	GP1_PIN                 22					/**< RF GPIO 1 pin */
#endif

#define	IO1_PORT                gpioPortC           /**< IO 1 port */
#define	IO1_BIT                 14					/**< IO 1 bit */
#define	IO1_MASK                (1 << IO1_BIT)		/**< IO 1 mask */
#define	IO1_PIN                 23					/**< IO 1 pin */

#define	USR1_PORT               gpioPortC           /**< User GPIO 1 port */
#define	USR1_BIT                15					/**< User GPIO 1 bit */
#define	USR1_MASK               (1 << USR1_BIT)		/**< User GPIO 1 mask */
#define	USR1_PIN                24					/**< User GPIO 1 pin */

/** @} (end addtogroup Port_C) */

/***************************************************************************//**
 * @addtogroup Port_D Port D
 * @brief TD1202 module port D
 * @{
 ******************************************************************************/

#define TX_PORT                 gpioPortD           /**< RS232 TX (Out) port */
#define TX_BIT                  4					/**< RS232 TX (Out) bit */
#define TX_MASK                 (1 << TX_BIT)		/**< RS232 TX (Out) mask */
#define TX_PIN                  16					/**< RS232 TX (Out) pin */

#define RX_PORT                 gpioPortD           /**< RS232 RX (In) port */
#define RX_BIT                  5					/**< RS232 RX (In) bit */
#define RX_MASK                 (1 << RX_BIT)		/**< RS232 RX (In) mask */
#define RX_PIN                  17					/**< RS232 RX (In) pin */

#define TIM1_PORT               gpioPortD           /**< Timer 1 port */
#define TIM1_BIT                6					/**< Timer 1 bit */
#define TIM1_MASK               (1 << TIM1_BIT)		/**< Timer 1 mask */
#define TIM1_PIN                18					/**< Timer 1 pin */

#define TIM2_PORT               gpioPortD           /**< Timer 2 port */
#define TIM2_BIT                7					/**< Timer 2 bit */
#define TIM2_MASK               (1 << TIM2_BIT)		/**< Timer 2 mask */
#define TIM2_PIN                19					/**< Timer 2 pin */

/** @} (end addtogroup Port_D) */

/***************************************************************************//**
 * @addtogroup Port_E Port E
 * @brief TD1202 module port E
 * @{
 ******************************************************************************/

#define SDI_RF_PORT             gpioPortE           /**< RF SPI MOSI (Out) port */
#define SDI_RF_BIT              10					/**< RF SPI MOSI (Out) bit */
#define SDI_RF_MASK             (1 << SDI_RF_BIT)	/**< RF SPI MOSI (Out) mask */
#define SDI_RF_PIN              29					/**< RF SPI MOSI (Out) pin */

#define SDO_RF_PORT             gpioPortE           /**< RF SPI MISO (In) port */
#define SDO_RF_BIT              11					/**< RF SPI MISO (In) bit */
#define SDO_RF_MASK             (1 << SDO_RF_BIT)	/**< RF SPI MISO (In) mask */
#define SDO_RF_PIN              30					/**< RF SPI MISO (In) pin */

#define SCLK_RF_PORT            gpioPortE           /**< RF SPI Clock (Out) port */
#define SCLK_RF_BIT             12					/**< RF SPI Clock (Out) bit */
#define SCLK_RF_MASK            (1 << SCLK_RF_BIT)	/**< RF SPI Clock (Out) mask */
#define SCLK_RF_PIN             31					/**< RF SPI Clock (Out) pin */

#define NSEL_RF_PORT            gpioPortE           /**< RF Chip Select (Out) port */
#define NSEL_RF_BIT             13					/**< RF Chip Select (Out) bit */
#define NSEL_RF_MASK            (1 << NSEL_RF_BIT)	/**< RF Chip Select (Out) mask */
#define NSEL_RF_PIN             32					/**< RF Chip Select (Out) pin */

/** @} (end addtogroup Port_E) */

/***************************************************************************//**
 * @addtogroup Port_F Port F
 * @brief TD1202 module port_F
 * @{
 ******************************************************************************/

#define DB3_PORT                gpioPortF           /**< JTAG SWDCLK Clock (In) port */
#define DB3_BIT                 0					/**< JTAG SWDCLK Clock (In) bit */
#define DB3_MASK                (1 << DB3_BIT)		/**< JTAG SWDCLK Clock (In) mask */
#define DB3_PIN                 25					/**< JTAG SWDCLK Clock (In) pin */

#define DB2_PORT                gpioPortF           /**< JTAG SWDIO Data I/O port */
#define DB2_BIT                 1					/**< JTAG SWDIO Data I/O bit */
#define DB2_MASK                (1 << DB2_BIT)		/**< JTAG SWDIO Data I/O mask */
#define DB2_PIN                 26					/**< JTAG SWDIO Data I/O pin */

#if (MODULE_REVISION == REVISION_VBC4)
#define POWER_CRYSTAL_PORT      gpioPortB           /**< Power RF Crystal port */
#define POWER_CRYSTAL_BIT       11					/**< Power RF Crystal bit */
#define POWER_CRYSTAL_MASK      (1 << POWER_CRYSTAL_BIT)	/**< Power RF Crystal mask */
#define POWER_CRYSTAL_PIN       10					/**< Power RF Crystal pin */
#else
#define POWER_CRYSTAL_PORT      gpioPortF           /**< Power RF Crystal port */
#define POWER_CRYSTAL_BIT       2					/**< Power RF Crystal bit */
#define POWER_CRYSTAL_MASK      (1 << POWER_CRYSTAL_BIT)	/**< Power RF Crystal mask */
#define POWER_CRYSTAL_PIN       27					/**< Power RF Crystal pin */
#endif

/** @} (end addtogroup Port_F) */

/***************************************************************************//**
 * @addtogroup RF_Interface RF Interface
 * @brief TD1202 module RF chip interface
 * @{
 ******************************************************************************/

/** RF Chip interface */
#define DATA_RF_PORT            GP1_PORT			/**< RF Data port */
#define DATA_RF_BIT             GP1_BIT				/**< RF Data bit */
#define DATA_RF_MASK            GP1_MASK			/**< RF Data mask */
#define DATA_RF_PIN             GP1_PIN				/**< RF Data pin */

#define CLOCK_RF_PORT           GP2_PORT			/**< RF clock port */
#define CLOCK_RF_BIT            GP2_BIT				/**< RF clock bit */
#define CLOCK_RF_MASK           GP2_MASK			/**< RF clock mask */
#define CLOCK_RF_PIN            GP2_PIN				/**< RF clock pin */

#define IRQ0_RADIO              NIRQ_RF_MASK		/**< RF IRQ0 mask */
#define IRQ0_PORT               NIRQ_RF_PORT		/**< RF IRQ0 port */
#define IRQ0_BIT                NIRQ_RF_BIT			/**< RF IRQ0 bit */

#define IRQ1_RADIO              GP2_MASK			/**< RF IRQ1 mask */
#define IRQ1_PORT               GP2_PORT			/**< RF IRQ1 port */
#define IRQ1_BIT                GP2_BIT				/**< RF IRQ1 bit */
#endif