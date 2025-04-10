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

void send_ack(bool isAck);

#endif /* INC_ACK_HANDLER_H_ */
