/*
 * crc_calculator.h
 *
 *  Created on: Mar 22, 2025
 *      Author: Eng Hassan Sameh
 */

#ifndef INC_CRC_CALCULATOR_H_
#define INC_CRC_CALCULATOR_H_

#include <stdint.h>
#include "main.h"

extern CRC_HandleTypeDef hcrc;

uint32_t Calculate_CRC(const uint8_t *data, uint8_t length);

#endif /* INC_CRC_CALCULATOR_H_ */
