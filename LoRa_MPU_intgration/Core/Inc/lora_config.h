/*
 * lora_config.h
 *
 *  Created on: Mar 22, 2025
 *      Author: Eng Hassan Sameh
 */

#ifndef INC_LORA_CONFIG_H_
#define INC_LORA_CONFIG_H_

#include "SX1278.h"
#include <string.h>
#include <stdio.h>

extern LoRa myLoRa;
extern char buffer[256];

void LoRa_Init(void);

#endif /* INC_LORA_CONFIG_H_ */
