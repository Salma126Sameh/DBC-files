/*
 * crc_utils.h
 *
 *  Created on: Mar 8, 2025
 *      Author: CarSync
 */

#ifndef INC_CRC_UTILS_H_
#define INC_CRC_UTILS_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

uint32_t CalculateCRC(CRC_HandleTypeDef *hcrc, const uint8_t *data, uint32_t length);

#endif /* INC_CRC_UTILS_H_ */

