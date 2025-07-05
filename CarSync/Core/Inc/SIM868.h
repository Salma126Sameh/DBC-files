/*
 * SIM868.H
 *
 *  Created on: Mar 1, 2025
 *      Author: CarSync
 */

#ifndef INC_SIM868_H_
#define INC_SIM868_H_
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define UART_TIMEOUT 1000  // 2 seconds timeout
//#define RESPONSE_MAX_LEN 128
//#define RESPONSE_MAX_LEN 1000
//#define TX_DELAY_MS 200
#define JSON_WAIT_MS 10000
#define HTTP_ACTION_TIMEOUT_MS 15000
#define MAX_TOPIC_LENGTH 20   // Adjust as needed for your use case
#define MAX_PAYLOAD_LENGTH 256 // Adjust as needed for your use case
#define MAX_PACKET_SIZE (2 + 2 + MAX_TOPIC_LENGTH + MAX_PAYLOAD_LENGTH)
typedef enum {
    SIM868_SUCCESS = 0,
    SIM868_NO_FIX,
    SIM868_INVALID_RESPONSE,
    SIM868_ERROR
} SIM868_Status_t;

typedef struct {
    char year[5];    // YYYY + null
    char month[3];   // MM + null
    char day[3];     // DD + null
    char hour[3];    // HH + null
    char minute[3];  // MM + null
    char second[3];  // SS + null
} GPS_Time_Str_t;

typedef struct {
    uint8_t valid : 1;    // Validity flag
    uint8_t reserved : 7; // Reserved bits
    char lat_str[12];     // Latitude as string
    char speed_str[8];    // Speed as string
    char course_str[8];   // Course as string
    char lon_str[12];     // Longitude as string
    GPS_Time_Str_t time_str;
} GPS_Location_t;

SIM868_Status_t sendATCommand(UART_HandleTypeDef *huart, const char *command,
		char *response, uint16_t timeout);
SIM868_Status_t SIM868_GetGPSLocation(UART_HandleTypeDef *huart, GPS_Location_t *location);
void SIM868_PowerON(GPIO_TypeDef *port, uint16_t pin);
void SIM868_PowerOFF(GPIO_TypeDef *port, uint16_t pin);
SIM868_Status_t SIM868_Init(UART_HandleTypeDef *huart);
SIM868_Status_t SIM868_check_signal(UART_HandleTypeDef *huart);
//float convertToDecimal(float coordinate);
void SIM868_sendGetRequest(UART_HandleTypeDef *huart);
void SIM868_sendPostData(UART_HandleTypeDef *huart, const char *json);
//void testPostRequest(UART_HandleTypeDef *huart, String jsonToSend);
SIM868_Status_t sendMQTTPacket(UART_HandleTypeDef *huart, uint8_t *data,
		uint16_t len, uint16_t timeout);
void mqtt_publish_packet(UART_HandleTypeDef *huart,const char *topic, const char *payload);
#endif /* INC_SIM868_H_ */
