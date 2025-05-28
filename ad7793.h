/*
 * ad7793.h
 *
 *  Created on: Aug 28, 2024
 *      Author: User
 */

#ifndef INC_AD7793_H_
#define INC_AD7793_H_

#include "main.h"

/*AD7793 Registers*/
#define AD7793_REG_COMM		0 /* Communications Register(WO, 8-bit) */
#define AD7793_REG_STAT	    0 /* Status Register	    (RO, 8-bit) */
#define AD7793_REG_MODE	    1 /* Mode Register	     	(RW, 16-bit */
#define AD7793_REG_CONF	    2 /* Configuration Register (RW, 16-bit)*/
#define AD7793_REG_DATA	    3 /* Data Register	     	(RO, 16-/24-bit) */
#define AD7793_REG_ID	    4 /* ID Register	     	(RO, 8-bit) */
#define AD7793_REG_IO	    5 /* IO Register	     	(RO, 8-bit) */
#define AD7793_REG_OFFSET   6 /* Offset Register	    (RW, 24-bit */
#define AD7793_REG_FULLSCAL	7 /* Full-Scale Register	(RW, 24-bit */

/* Communications Register Bit Designations (AD7793_REG_COMM) */
#define AD7793_COMM_WEN		(1 << 7) 			/* Write Enable */
#define AD7793_COMM_WRITE	(0 << 6) 			/* Write Operation */
#define AD7793_COMM_READ    (1 << 6) 			/* Read Operation */
#define AD7793_COMM_ADDR(x)	(((x) & 0x7) << 3)	/* Register Address */
#define AD7793_COMM_CREAD	(1 << 2) 			/* Continuous Read of Data Register */

/* Status Register Bit Designation masks(AD7793_REG_STAT) */
#define AD7793_STAT_RDY		(1 << 7) 					/* Ready */
#define AD7793_STAT_ERR		(1 << 6) 					/* Error (Overrange, Underrange) */
#define AD7793_STAT_CH3		(1 << 2) 					/* Channel 3 */
#define AD7793_STAT_CH2		(1 << 1) 					/* Channel 2 */
#define AD7793_STAT_CH1		(1 << 0) 					/* Channel 1 */

/* Mode Register Bit Designations (AD7793_REG_MODE) */
#define AD7793_MODE_SEL(x)		(((x) & 0x7) << 13)	/* Operation Mode Select */
#define AD7793_MODE_CLKSRC(x)	(((x) & 0x3) << 6) 	/* ADC Clock Source Select */
#define AD7793_MODE_RATE(x)		((x) & 0xF) 		/* Filter Update Rate Select */

/* Configuration Register Bit Designations (AD7793_REG_CONF) */
#define AD7793_CONF_VBIAS(x)  	(((x) & 0x3) << 14) 	/* Bias Voltage Generator Enable */
#define AD7793_CONF_BO_EN	  	(1 << 13) 				/* Burnout Current Enable */
#define AD7793_CONF_UNIPOLAR(x) (((x) & 0x1) << 12) 	/* Unipolar/Bipolar Enable */
#define AD7793_CONF_BOOST	  	(1 << 11) 				/* Boost Enable */
#define AD7793_CONF_GAIN(x)	  	(((x) & 0x7) << 8) 		/* Gain Select */
#define AD7793_CONF_REFSEL(x) 	(((x) & 0x1) << 7) 		/* INT/EXT Reference Select */
#define AD7793_CONF_BUF		  	(1 << 4) 				/* Buffered Mode Enable */
#define AD7793_CONF_CHAN(x)	  	((x) & 0x7) 			/* Channel select */

/* ID Register Bit Designations (AD7793_REG_ID) */
#define AD7793_ID			0xB
#define AD7793_ID_MASK		0xF

/* IO (Excitation Current Sources) Register Bit Designations (AD7793_REG_IO) */
#define AD7793_IEXCDIR(x)	(((x) & 0x3) << 2)
#define AD7793_IEXCEN(x)	(((x) & 0x3) << 0)

#define AD7793_TIMEOUT_SPI  	1000
#define AD7793_TIMEOUT_READ 	1000

#define N 24

static const uint16_t settle_time_ms[] = { 0, 4, 8, 16, 32, 40, 48, 60, 101, 120, 120, 160, 200, 240, 320, 480 }; /* Time for measurement to settle */

/**
 * Enum to set sample rate
 */
typedef enum {
	AD7793_RATE_470 = 1,
	AD7793_RATE_242,
	AD7793_RATE_123,
	AD7793_RATE_62,
	AD7793_RATE_50,
	AD7793_RATE_39,
	AD7793_RATE_33_2,
	AD7793_RATE_19_6, /**< 60 Hz only  */
	AD7793_RATE_16_7_50, /**< 50 Hz only  */
	AD7793_RATE_16_7_50_60, /**< 50 and 60 Hz */
	AD7793_RATE_12_5, /**< 50 and 60 Hz */
	AD7793_RATE_10, /**< 50 and 60 Hz */
	AD7793_RATE_8_33, /**< 50 and 60 Hz */
	AD7793_RATE_6_25, /**< 50 and 60 Hz */
	AD7793_RATE_4_17 /**< 50 and 60 Hz */
} AD7793_Rate;

/**
 * Enum to set functioning mode
 */
typedef enum {
	AD7793_MODE_CONT = 0, /**< AD7793_MODE_CONT */
	AD7793_MODE_SINGLE, /**< AD7793_MODE_SINGLE */
	AD7793_MODE_IDLE, /**< AD7793_MODE_IDLE */
	AD7793_MODE_PWRDN, /**< AD7793_MODE_PWRDN */
	AD7793_MODE_CAL_INT_ZERO, /**< AD7793_MODE_CAL_INT_ZERO */
	AD7793_MODE_CAL_INT_FULL, /**< AD7793_MODE_CAL_INT_FULL */
	AD7793_MODE_CAL_SYS_ZERO, /**< AD7793_MODE_CAL_SYS_ZERO */
	AD7793_MODE_CAL_SYS_FULL /**< AD7793_MODE_CAL_SYS_FULL */
} AD7793_Mode;

/**
 * Enum to set clock source
 */
typedef enum {
	AD7793_CLK_INT = 0,
	AD7793_CLK_INT_CO,
	AD7793_CLK_EXT,
	AD7793_CLK_EXT_DIV2
} AD7793_Clock;

/**
 * Enum to set channel
 */
typedef enum {
	AD7793_CH_AIN1P_AIN1M,
	AD7793_CH_AIN2P_AIN2M,
	AD7793_CH_AIN3P_AIN3M,
	AD7793_CH_AIN1M_AIN1M,
	AD7793_CH_AVDD_MONITOR
} AD7793_Channel;

/**
 * Enum to set excitation current direction
 */
typedef enum {
	AD7793_DIR_IEXC1_IOUT1_IEXC2_IOUT2 = 0,
	AD7793_DIR_IEXC1_IOUT2_IEXC2_IOUT1,
	AD7793_DIR_IEXC1_IEXC2_IOUT1,
	AD7793_DIR_IEXC1_IEXC2_IOUT2
} AD7793_Iexdir;

/**
 * Enum to set excitation current
 */
typedef enum {
	AD7793_EN_IXCEN_DISABLE = 0,
	AD7793_EN_IXCEN_10uA,
	AD7793_EN_IXCEN_210uA,
	AD7793_EN_IXCEN_1mA
} AD7793_Iexen;

/**
 * Enum to set bias voltage connection
 */
typedef enum {
	AD7793_VBIAS_GEN_DISABL = 0,
	AD7793_VBIAS_AIN1_NEG,
	AD7793_VBIAS_AIN2_NEG
} AD7793_Vbias;

/**
 * Enum to set PGA Gain
 */
typedef enum {
	AD7793_GAIN_1 = 0, /**< AD7793_GAIN_1 */
	AD7793_GAIN_2, /**< AD7793_GAIN_2 */
	AD7793_GAIN_4, /**< AD7793_GAIN_4 */
	AD7793_GAIN_8, /**< AD7793_GAIN_8 */
	AD7793_GAIN_16, /**< AD7793_GAIN_16 */
	AD7793_GAIN_32, /**< AD7793_GAIN_32 */
	AD7793_GAIN_64, /**< AD7793_GAIN_64 */
	AD7793_GAIN_128 /**< AD7793_GAIN_128 */
} AD7793_Gain;

/**
 * Enum to set reference voltage source
 */
typedef enum {
	AD7793_REFSEL_EXT = 0,
	AD7793_REFSEL_INT
} AD7793_Refsel;

/**
 * enum to set polarity
 */
typedef enum {
	AD7793_BIPOLAR = 0, /**< AD7793_BIPOLAR */
	AD7793_UNIPOLAR /**< AD7793_UNIPOLAR */
} AD7793_Polarity;

/**
 * Struct to easily use configuration and values from conversions
 */
typedef struct {
	SPI_HandleTypeDef *adcspi; /**< SPI handler */
	GPIO_TypeDef *CSPort; /**< CS GPIO Port */
	uint16_t CSPin; /**< CS GPIO Pin */

	AD7793_Clock clock; /**< Selected clock source */
	AD7793_Rate rate; /**< Selected sample rate */
	AD7793_Mode mode; /**< Selected mode */
	AD7793_Channel channel; /**< Selected channel */
	AD7793_Iexdir iexdir; /**< Selected excitation current direction */
	AD7793_Iexen iexen; /**< Selected excitation current */
	AD7793_Vbias vbias; /**< Selected vbias connection */
	AD7793_Gain gain; /**< Selected gain */
	AD7793_Refsel refsel; /**< Selected reference source */
	AD7793_Polarity polarity; /**< Selected polarity */

	float vref; /**< Voltage reference */

	uint32_t rawConversion; /**< Value of raw conversion */
	float voltConversion; /**< Value of voltage */
} AD7793;

HAL_StatusTypeDef AD7793_Init(AD7793 *ad7793);

void AD7793_Reset(AD7793 *ad7793);

uint32_t AD7793_GetRegisterValue(AD7793 *ad7793, uint8_t regAddress, uint8_t size);

void AD7793_SetRegisterValue(AD7793 *ad7793, uint8_t regAddress, uint32_t regValue, uint8_t size);

void AD7793_SetRate(AD7793 *ad7793, AD7793_Rate rate);

void AD7793_SetClockSource(AD7793 *ad7793, AD7793_Clock clock);

void AD7793_SetMode(AD7793 *ad7793, AD7793_Mode mode);

uint8_t AD7793_Ready(AD7793 *ad7793);

void AD7793_SetChannel(AD7793 *ad7793, AD7793_Channel channel);

void AD7793_SetBiasVoltage(AD7793 *ad7793, AD7793_Vbias vbias);

void AD7793_SetExcitDirection(AD7793 *ad7793, AD7793_Iexdir iexdir);

void AD7793_SetExcitCurrent(AD7793 *ad7793, AD7793_Iexen iexen);

void AD7793_SetGain(AD7793 *ad7793, AD7793_Gain gain);

void AD7793_SetReference(AD7793 *ad7793, AD7793_Refsel refsel);

void AD7793_SetPolarity(AD7793 *ad7793, AD7793_Polarity polarity);

HAL_StatusTypeDef AD7793_SingleConversion(AD7793 *ad7793);

void AD7793_RawToVolt(AD7793 *ad7793);

#endif /* INC_AD7793_H_ */
