/*
 * ad7793.c
 *
 *  Created on: Aug 28, 2024
 *      Author: Petro Zaichenko / Axel Chabot
 */

#include "ad7793.h"
#include "math.h"
#include "stdio.h"

/**
  * @brief This function reads AD7793 ID and compares it with the defined in datasheet one (0xB).
  * @param *ad7793 is a pointer to the structure which consists all needed information about the connection and settings of AD7793.
  * @retval HAL_Status of the AD7793.
  */

HAL_StatusTypeDef AD7793_Init(AD7793 *ad7793) {

	uint32_t data = AD7793_GetRegisterValue(ad7793, AD7793_REG_ID, 1);
	printf("Init = 0x%X\r\n", data);

	if ((data & AD7793_ID_MASK) != AD7793_ID) {
		return HAL_ERROR;
	} else {
		return HAL_OK;
	}
}

/**
  * @brief This function .
  * @param
  * @retval
  */

void AD7793_Reset(AD7793 *ad7793) {
	uint8_t dataToSend[4] = { 0xff, 0xff, 0xff, 0xff };
	HAL_GPIO_WritePin(ad7793->CSPort, ad7793->CSPin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(ad7793->adcspi, dataToSend, 4, AD7793_TIMEOUT_SPI);
	HAL_GPIO_WritePin(ad7793->CSPort, ad7793->CSPin, GPIO_PIN_SET);

	ad7793->mode = AD7793_MODE_CONT;
	ad7793->gain = AD7793_GAIN_1;
	ad7793->channel = AD7793_CH_AIN1P_AIN1M;
	ad7793->polarity = AD7793_BIPOLAR;
	ad7793->rate = AD7793_RATE_16_7_50_60;
}

void AD7793_SetMode(AD7793 *ad7793, AD7793_Mode mode) {
	uint32_t command;
	command = AD7793_GetRegisterValue(ad7793, AD7793_REG_MODE, 2);
	command &= ~AD7793_MODE_SEL(0xFF);
	command |= AD7793_MODE_SEL((uint32_t ) mode);
	AD7793_SetRegisterValue(ad7793, AD7793_REG_MODE, command, 2);
	ad7793->mode = mode;
}

void AD7793_SetClockSource(AD7793 *ad7793, AD7793_Clock clock) {
	uint32_t command;
	command = AD7793_GetRegisterValue(ad7793, AD7793_REG_MODE, 2);
	command &= ~AD7793_MODE_CLKSRC(0xFF);
	command |= AD7793_MODE_CLKSRC((uint32_t ) clock);
	AD7793_SetRegisterValue(ad7793, AD7793_REG_MODE, command, 2);
	ad7793->clock = clock;
}

void AD7793_SetRate(AD7793 *ad7793, AD7793_Rate rate) {
	uint32_t command;
	command = AD7793_GetRegisterValue(ad7793, AD7793_REG_MODE, 2);
	command &= ~AD7793_MODE_RATE(0xFF);
	command |= AD7793_MODE_RATE((uint32_t ) rate);
	AD7793_SetRegisterValue(ad7793, AD7793_REG_MODE, command, 2);
	ad7793->rate = rate;
}

void AD7793_SetBiasVoltage(AD7793 *ad7793, AD7793_Vbias vbias) {
	uint32_t command;
	command = AD7793_GetRegisterValue(ad7793, AD7793_REG_CONF, 2);
	command &= ~AD7793_CONF_VBIAS(0xFF);
	command |= AD7793_CONF_VBIAS((uint32_t) vbias);
	AD7793_SetRegisterValue(ad7793, AD7793_REG_CONF, command, 2);
	ad7793->vbias = vbias;
}

void AD7793_SetExcitDirection(AD7793 *ad7793, AD7793_Iexdir iexdir) {
	uint32_t command;
	command = AD7793_GetRegisterValue(ad7793, AD7793_REG_IO, 1);
	command &= ~AD7793_IEXCDIR(0xF);
	command |= AD7793_IEXCDIR((uint32_t) iexdir);
	AD7793_SetRegisterValue(ad7793, AD7793_REG_IO, command, 1);
	ad7793->iexdir = iexdir;
}

void AD7793_SetExcitCurrent(AD7793 *ad7793, AD7793_Iexen iexen) {
	uint32_t command;
	command = AD7793_GetRegisterValue(ad7793, AD7793_REG_IO, 1);
	command &= ~AD7793_IEXCEN(0xF);
	command |= AD7793_IEXCEN((uint32_t) iexen);
	AD7793_SetRegisterValue(ad7793, AD7793_REG_IO, command, 1);
	ad7793->iexen = iexen;
}

void AD7793_SetChannel(AD7793 *ad7793, AD7793_Channel channel) {
	uint32_t command;
	command = AD7793_GetRegisterValue(ad7793, AD7793_REG_CONF, 2);
	command &= ~AD7793_CONF_CHAN(0xFF);
	command |= AD7793_CONF_CHAN((uint32_t ) channel);
	AD7793_SetRegisterValue(ad7793, AD7793_REG_CONF, command, 2);
	ad7793->channel = channel;
}

void AD7793_SetGain(AD7793 *ad7793, AD7793_Gain gain) {
	uint32_t command;
	command = AD7793_GetRegisterValue(ad7793, AD7793_REG_CONF, 2);
	command &= ~AD7793_CONF_GAIN(0xFF);
	command |= AD7793_CONF_GAIN((uint32_t ) gain);
	AD7793_SetRegisterValue(ad7793, AD7793_REG_CONF, command, 2);
	ad7793->gain = gain;
}

void AD7793_SetReference(AD7793 *ad7793, AD7793_Refsel refsel) {
	uint32_t command;
	command = AD7793_GetRegisterValue(ad7793, AD7793_REG_CONF, 2);
	command &= ~AD7793_CONF_REFSEL(1);
	command |= AD7793_CONF_REFSEL((uint32_t) refsel);
	AD7793_SetRegisterValue(ad7793, AD7793_REG_CONF, command, 2);
	if (refsel == AD7793_REFSEL_INT) {
		ad7793->vref = 1170;
	}
}

void AD7793_SetPolarity(AD7793 *ad7793, AD7793_Polarity polarity) {
	uint32_t command = 0;
	command = AD7793_GetRegisterValue(ad7793, AD7793_REG_CONF, 2);
	command &= ~AD7793_CONF_UNIPOLAR(1);
	command |= AD7793_CONF_UNIPOLAR((uint32_t ) polarity);
	AD7793_SetRegisterValue(ad7793, AD7793_REG_CONF, command, 2);
	ad7793->polarity = polarity;
}

HAL_StatusTypeDef AD7793_SingleConversion(AD7793 *ad7793) {
	AD7793_SetMode(ad7793, AD7793_MODE_SINGLE);

	uint32_t startTime = HAL_GetTick();
	while (!AD7793_Ready(ad7793)) {
		if (HAL_GetTick() - startTime > (uint32_t) (1.5 * settle_time_ms[(uint8_t) ad7793->rate])) {
			return HAL_TIMEOUT;
		}
	}

	ad7793->rawConversion = AD7793_GetRegisterValue(ad7793, AD7793_REG_DATA, 3);

	AD7793_RawToVolt(ad7793);

	return HAL_OK;
}

void AD7793_RawToVolt(AD7793 *ad7793) {
	float gain = (float) pow(2.0, (double) ad7793->gain);

	if (ad7793->polarity == AD7793_UNIPOLAR) {
		float fullscale = (float) pow(2.0, (double) N);
		ad7793->voltConversion = (float) ad7793->rawConversion * (ad7793->vref / (fullscale * gain));
	} else {
		float fullscale = (float) pow(2.0, (double) (N - 1));
		ad7793->voltConversion = (((float) ad7793->rawConversion - fullscale) * ad7793->vref) / (fullscale * gain);
	}
}

uint8_t AD7793_Ready(AD7793 *ad7793) {
	uint8_t rdy = 0;
	rdy = (AD7793_GetRegisterValue(ad7793, AD7793_REG_STAT, 1) & 0x80);
	return (!rdy);
}

uint32_t AD7793_GetRegisterValue(AD7793 *ad7793, uint8_t regAddress, uint8_t size) {
	uint8_t data[4] = { 0x00, 0x00, 0x00, 0x00 };
	uint32_t receivedData = 0x00;
	data[0] = AD7793_COMM_READ | AD7793_COMM_ADDR(regAddress);
	HAL_GPIO_WritePin(ad7793->CSPort, ad7793->CSPin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(ad7793->adcspi, data, 1, AD7793_TIMEOUT_SPI);
	HAL_SPI_Receive(ad7793->adcspi, data, size, AD7793_TIMEOUT_SPI);
	HAL_GPIO_WritePin(ad7793->CSPort, ad7793->CSPin, GPIO_PIN_SET);
	if (size == 1) {
		receivedData += (data[0] << 0);
	}
	if (size == 2) {
		receivedData += (data[0] << 8);
		receivedData += (data[1] << 0);
	}
	if (size == 3) {
		receivedData += (data[0] << 16);
		receivedData += (data[1] << 8);
		receivedData += (data[2] << 0);
	}
	return receivedData;
}

void AD7793_SetRegisterValue(AD7793 *ad7793, uint8_t regAddress, uint32_t regValue, uint8_t size) {
	uint8_t data[4] = { 0x00, 0x00, 0x00, 0x00 };
	data[0] = AD7793_COMM_WRITE | AD7793_COMM_ADDR(regAddress);
	if (size == 1) {
		data[1] = (uint8_t) regValue;
	}
	if (size == 2) {
		data[2] = (uint8_t) ((regValue & 0x0000FF) >> 0);
		data[1] = (uint8_t) ((regValue & 0x00FF00) >> 8);
	}
	if (size == 3) {
		data[3] = (uint8_t) ((regValue & 0x0000FF) >> 0);
		data[2] = (uint8_t) ((regValue & 0x00FF00) >> 8);
		data[1] = (uint8_t) ((regValue & 0xFF0000) >> 16);
	}
	HAL_GPIO_WritePin(ad7793->CSPort, ad7793->CSPin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(ad7793->adcspi, data, 1 + size, AD7793_TIMEOUT_SPI);
	HAL_GPIO_WritePin(ad7793->CSPort, ad7793->CSPin, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}
