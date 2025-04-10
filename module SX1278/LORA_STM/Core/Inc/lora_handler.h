/*
 * lora_handler.h
 *
 *  Created on: Mar 8, 2025
 *      Author: CarSync
 */

#ifndef INC_LORA_HANDLER_H_
#define INC_LORA_HANDLER_H_


#include <stdbool.h>
#include "SX1278.h"
#include "crc_utils.h"

#define ACK_TIMEOUT_MS      5000  
#define RETRY_DELAY_MS      1000
#define MAX_RETRIES         5
#define LED_BLINK_TIME      100

void LoRa_handle_init(void);
uint8_t LoRa_SendData(uint8_t *data, uint8_t length);
uint8_t LoRa_ReceiveData(uint8_t *buffer, uint8_t max_length);
void LoRa_SendACK(void);
void LoRa_SendNotACK(void);
bool WaitForACK(void);
bool ForwardDataToSecondLoRa(uint8_t *data, uint8_t length);
bool LoRa_SendWithRetry(uint8_t *data, uint8_t length, uint16_t *status);
void ProcessReceivedData(uint8_t *data, uint8_t length);
void SendExampleMessage(void);

#endif /* INC_LORA_HANDLER_H_ */
