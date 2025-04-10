/*
 * crc_utils.c
 *
 *  Created on: Mar 8, 2025
 *      Author: CarSync
 */


#include "crc_utils.h"
#include<string.h>

uint32_t CalculateCRC(CRC_HandleTypeDef *hcrc, const uint8_t *data, uint32_t length) {

    uint32_t crc = 0;
    if (length == 0)
        return crc;

    uint32_t word_count = length / 4;
    if (word_count > 0) {
        crc = HAL_CRC_Calculate(hcrc, (uint32_t*)data, word_count);
    }

    uint8_t remaining = length % 4;
    if (remaining) {
        uint32_t temp = 0;
        memcpy(&temp, data + (length - remaining), remaining);
        crc = HAL_CRC_Accumulate(hcrc, &temp, 1);
    }
    return crc;
}
