/*
 * ack_handler.h
 *
 *  Created on: Mar 22, 2025
 *      Author: Eng Hassan Sameh
 */

#ifndef INC_ACK_HANDLER_H_
#define INC_ACK_HANDLER_H_

#include <stdbool.h>
#include "SX1278.h"
#include "usbd_cdc_if.h"

extern LoRa myLoRa;

bool receive_ack_TX(void);
//void sendMessageAndWaitForAck(LoRa *lora, uint8_t *messageBuffer);
void sendMessageAndWaitForAck_TX(LoRa *lora, const uint8_t *messageBuffer);

void send_ack_RX(bool isAck);
uint8_t receiveAndsendACK_RX(LoRa *_LoRa);

#endif /* INC_ACK_HANDLER_H_ */
