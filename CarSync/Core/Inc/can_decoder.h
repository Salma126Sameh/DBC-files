/*
 * can_decoder.h
 *
 *  Created on: May 10, 2025
 *      Author: omar7
 */

#ifndef INC_CAN_DECODER_H_
#define INC_CAN_DECODER_H_

#include <stdint.h>
#include <stdbool.h>

extern int decoded_speed;
extern int decoded_temp;
extern int decoded_fuel;

void CAN_Decode(uint8_t* rawData, uint8_t length);

#endif /* INC_CAN_DECODER_H_ */
