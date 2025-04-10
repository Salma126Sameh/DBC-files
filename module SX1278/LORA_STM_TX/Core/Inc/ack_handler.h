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

bool receive_ack(void);
bool sendMessageAndWaitForAck(LoRa *lora, uint8_t *messageBuffer, uint8_t totalLen);

#endif /* INC_ACK_HANDLER_H_ */
