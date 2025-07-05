/*
 * communication.h
 *
 *  Created on: Jul 5, 2025
 *      Author: Eng Hassan Sameh
 */

#ifndef INC_COMMUNICATION_H_
#define INC_COMMUNICATION_H_

#include "main.h"
#include<stdbool.h>
#include "mpu_6050_task.h"
#include "can_decoder.h"
#include "ack_handler.h"
#include <stdio.h>

void CheckSignalAndSwitchCommunication(UART_HandleTypeDef *huart, bool *useLoRa);
void SendGPSDataViaMQTT(UART_HandleTypeDef *huart);
void SendMPU6050DataViaMQTT(UART_HandleTypeDef *huart, MPU6050_t *MPU6050, bool useLoRa);
void SendCANDataViaMQTT(UART_HandleTypeDef *huart, bool useLoRa);

void SendAccidentDataViaLoRa(LoRa *lora);
void HandleLoRaCommunication(LoRa *lora, uint8_t *receivedFlag);
#endif /* INC_COMMUNICATION_H_ */
