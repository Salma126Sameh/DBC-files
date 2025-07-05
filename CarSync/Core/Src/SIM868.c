/*
 * SIM868.c
 *
 *  Created on: Mar 1, 2025
 *      Author: CarSync
 */
#include "main.h"
#include "stdio.h"
#include "SIM868.h"
#include <string.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_uart.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
//#undef DEBUG
#include "debug.h"
//extern UART_HandleTypeDef huart1;

#define MAX_RESPONSE_LEN 128 // Reduced buffer size for response

static uint8_t rxBuffer[MAX_RESPONSE_LEN]; // Reuse this buffer
uint8_t mqtt_connect_packet[] = { 0x10, 0x12, 0x00, 0x04, 0x4D, 0x51, 0x54,
		0x54, 0x04, 0X02, 0x00, 0x3C, 0x00, 0x06, 0x41, 0x42, 0x43, 0x44, 0x45,
		0x46, 0x1A // <- This is the CTRL+Z (0x1A) to mark end of transmission
		};
SIM868_Status_t sendATCommand(UART_HandleTypeDef *huart, const char *command,
		char *response, uint16_t timeout) {

	// Clear only when necessary
	memset(rxBuffer, 0, MAX_RESPONSE_LEN);
	memset(response, 0, MAX_RESPONSE_LEN);

	// Send the AT command
	if (HAL_UART_Transmit(huart, (uint8_t*) command, strlen(command), timeout)
			!= HAL_OK) {
		DEBUG_PRINT("#RED#Failed to transmit command\n");
		return SIM868_ERROR;
	}
	if (HAL_UART_Transmit(huart, (uint8_t*) "\r\n", strlen("\r\n"), timeout)
			!= HAL_OK) {
		DEBUG_PRINT("#RED#Failed to transmit command\n");
		return SIM868_ERROR;
	}

	// Receive the response
	HAL_UART_Receive(huart, rxBuffer, MAX_RESPONSE_LEN - 1, timeout);

	// Copy and null-terminate the response
	memcpy(response, (char*) rxBuffer + 2, MAX_RESPONSE_LEN - 2);
	response[MAX_RESPONSE_LEN - 2] = '\0';
	DEBUG_PRINT("#GRN#SendAtCommand = %s\nRespond = %s\n", command, response);
	return SIM868_SUCCESS;
}

SIM868_Status_t SIM868_Init(UART_HandleTypeDef *huart) {
	char response[MAX_RESPONSE_LEN];

	// Check basic communication
	if (sendATCommand(huart, "AT", response, UART_TIMEOUT) != SIM868_SUCCESS) {
		DEBUG_PRINT("#RED#Didn't send AT command\n");
		return SIM868_ERROR;
	}

	// Check for OK response
	if (strstr(response, "OK") == NULL) {
		DEBUG_PRINT("Didn't get OK from AT\n");
		return SIM868_ERROR;
	}

	// Continue with the rest of initialization
	const char *init_commands[] = {
//		"AT+SSLCIPH?",
//		"AT+CGMR", 				   //check the firmware version
			"ATE0",// Disable echo
			"AT+CMEE=2", 			   // to get detailed error messages
			"AT+CGNSPWR=1",            // Enable GPS (if needed)
			"AT+CIPSHUT", "AT+CIPMUX=0",             // Single connection mode
			"AT+CIPTKA=1,300,60,5",    // Keepalive settings
			"AT+CIPQSEND=1",           // Quick send mode
			"AT+CSQ",				   // Check the signal just for debug
			"AT+CSTT=\"internet\",\"\",\"\"",    // Set APN
			"AT+CGATT=1",              // Attach GPRS
			"AT+CIICR",                // Activate context
			"AT+CIFSR", 		  	   // Get the IP
			"AT+CIPSPRT=0", "AT+CIPSTART=\"TCP\",\"broker.hivemq.com\",\"1883\"" // Connect to broker
			};

	for (int i = 0; i < sizeof(init_commands) / sizeof(init_commands[0]); ++i) {
		if (sendATCommand(huart, init_commands[i], response, UART_TIMEOUT)
				!= SIM868_SUCCESS) {
			DEBUG_PRINT("#RED#Failed command: %s\n", init_commands[i]);
			return SIM868_ERROR;
		}
	}
//	/*Initialize Network + MQTT connect*/
//	const char *commands[] = {
//	//			"AT+CSQ",
//	//			"AT+CIFSR", // Debug to get the IP
//			"AT+CIPCLOSE",// to make sure the connection closed before starting a new one
//			"AT+CIPSTART=\"TCP\",\"broker.hivemq.com\",\"1883\""
//
//	};
//	for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
//		if (sendATCommand(huart, commands[i], response, UART_TIMEOUT)
//				!= SIM868_SUCCESS) {
//			DEBUG_PRINT("Failed to send close command: %s\n", commands[i]);
//			return;
//		}
//	}
	/* ============================================= MQTT CONNECT PACKET =====================================================
	 | Byte Index | 							Hex Value        		   	 	   | 		   	  Description                |
	 |------------|----------------------------------------------------------------|-----------------------------------------|
	 | 0          | 0x10             			 							       | Packet Type: CONNECT                    |
	 | 1          | 0x2C         			     							       | Remaining Length: 44 bytes              |
	 |------------|----------------------------------------------------------------|-----------------------------------------|
	 | 2-3        | 0x00 0x06        		     							       | Protocol Name Length = 6                |
	 | 4-9        | 0x4D 0x51 0x49 0x73 0x64 0x70 							       | Protocol Name = "MQIsdp"     			 |
	 | 10         | 0x03             										       | Protocol Level = 3 (MQTT 3.1)           |
	 | 11         | 0xC2             										       | Connect Flags = 11000010                |
	 |            |                  										       |  - Username Flag: 1                     |
	 |            |                  										       |  - Password Flag: 1                     |
	 |            |                  										       |  - Will Retain: 0                       |
	 |            |                  										       |  - Will QoS: 00                         |
	 |            |                  										       |  - Will Flag: 0                         |
	 |            |                  										       |  - Clean Session: 1                     |
	 |            |                  										       |  - Reserved: 0                          |
	 | 12-13      | 0x00 0x3C        										       | Keep Alive = 60 seconds                 |
	 |------------|----------------------------------------------------------------|-----------------------------------------|
	 | 14-15      | 0x00 0x06        										       | Client ID Length = 6                    |
	 | 16-21      | 0x41 0x42 0x43 0x44 0x45 0x46 		                           | Client ID = "ABCDEF"       			 |
	 |------------|----------------------------------------------------------------|-----------------------------------------|
	 | 22-23      | 0x00 0x07       						                       | Username Length = 7                     |
	 | 24-31      | 0x43 0x61 0x72 0x53 0x79 0x6E 0x63	                           | Username = "CarSync" 					 |
	 | 32-33      | 0x00 0x0C       						                       | Password Length = 12                    |
	 | 34-45      | 0x71 0x41 0x55 0x5A 0x42 0x64 0x61 0x53 0x6C 0x55 0x4C 0x78    | Password = "qAUZBdaSlULx" 				 |
	 ====================================================================================================================== */

	//	10 12 00 04 4D 51 54 54 04 02 00 3C 00 06 41 42 43 44 45 46 1A # connect packet
	//	10 2C 00 06 4D 51 49 73 64 70 03 C2 00 3C 00 06 41 42 43 44 45 46 00 07 43 61 72 53 79 6E 63 00 0C 71 41 55 5A 42 64 61 53 49 55 4C 78 1A
	// send MQTT connection packet
	HAL_Delay(1000);
	sendATCommand(huart, "AT+CIPSEND", response, UART_TIMEOUT);
	sendMQTTPacket(huart, mqtt_connect_packet, sizeof(mqtt_connect_packet),
			4000);

	return SIM868_SUCCESS;
}
SIM868_Status_t SIM868_check_signal(UART_HandleTypeDef *huart) {
    char response[128];
    char *data;
    char *token;
    uint8_t rssi, ber;

    while (1) {
        sendATCommand(huart, "AT+CSQ", response, UART_TIMEOUT);

        // Ensure the response is null-terminated
        response[127] = '\0';

        // Locate the start of the signal quality data
        data = strstr(response, "+CSQ: ");
        if (data != NULL) {
            // Skip the "+CSQ: " part
            data += 6;

            // Tokenize the response to extract RSSI and BER
            token = strtok(data, ",");
            if (token != NULL) {
                rssi = (uint8_t)atoi(token);
                DEBUG_PRINT("RSSI: %d\n", rssi);

                // Check if RSSI is above threshold
                if (rssi > 10) {
                    token = strtok(NULL, ",");
                    if (token != NULL) {
                        ber = (uint8_t)atoi(token);
                        DEBUG_PRINT("BER: %d\n", ber);

                        // Check if BER is acceptable
                        if (ber == 0) {
                            DEBUG_PRINT("---------------------------------\n");
                            DEBUG_PRINT("-------------- Done -------------\n");
                            DEBUG_PRINT("---------------------------------\n");
                            return SIM868_SUCCESS;
                        }
                    }
                }
            }
        }

        // If we reach here, the signal quality is not acceptable, retry
        DEBUG_PRINT("Signal quality not acceptable, retrying...\n");
    }

    // If the loop exits without returning, there was an error
    DEBUG_PRINT("Failed to parse input.\n");
    return SIM868_ERROR;
}

void SIM868_PowerON(GPIO_TypeDef *port, uint16_t pin) {
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);  // Power on
}
void SIM868_PowerOFF(GPIO_TypeDef *port, uint16_t pin) {
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}
SIM868_Status_t SIM868_GetGPSLocation(UART_HandleTypeDef *huart,
		GPS_Location_t *location) {
	static char response[128]; // Static buffer to save memory

	// Initialize location to default values
	memset(location, 0, sizeof(GPS_Location_t));
	location->valid = 0;  // Mark as invalid

	// Request GPS information
	sendATCommand(huart, "AT+CGNSINF", response, UART_TIMEOUT);

	// Extract GPS data
	char *data_start = strstr(response, "+CGNSINF:");
	if (data_start == NULL) {
		DEBUG_PRINT("Failed to find +CGNSINF: in response\n");
		return SIM868_INVALID_RESPONSE;
	}

	// Parse using tokenization
	char *saveptr = NULL;
	char *token = strtok_r(data_start + 10, ",", &saveptr); // Skip "+CGNSINF: "
	int field_index = 0;

	// Process each token
	while (token != NULL && field_index <= 7) { // Only process up to course field
		switch (field_index) {
		case 0: // Run status
			if (*token != '1') {
				DEBUG_PRINT("GNSS not running\n");
				return SIM868_NO_FIX;
			}
			break;

		case 1: // Fix status
			if (*token != '1') {
				DEBUG_PRINT("No GPS fix\n");
				return SIM868_NO_FIX;
			}
			break;

		case 2: // UTC time
			if (strlen(token) >= 14) {
				// Extract time components
				memcpy(location->time_str.year, token, 4);
				location->time_str.year[4] = 0;

				memcpy(location->time_str.month, token + 4, 2);
				location->time_str.month[2] = 0;

				memcpy(location->time_str.day, token + 6, 2);
				location->time_str.day[2] = 0;

				memcpy(location->time_str.hour, token + 8, 2);
				location->time_str.hour[2] = 0;

				memcpy(location->time_str.minute, token + 10, 2);
				location->time_str.minute[2] = 0;

				memcpy(location->time_str.second, token + 12, 2);
				location->time_str.second[2] = 0;
			}
			break;

		case 3: // Latitude
			strncpy(location->lat_str, token, sizeof(location->lat_str) - 1);
			location->lat_str[sizeof(location->lat_str) - 1] = 0; // Ensure null termination
			break;

		case 4: // Longitude
			strncpy(location->lon_str, token, sizeof(location->lon_str) - 1);
			location->lon_str[sizeof(location->lon_str) - 1] = 0; // Ensure null termination
			break;

		case 5: // Altitude (skip)
			break;

		case 6: // Speed
			strncpy(location->speed_str, token,
					sizeof(location->speed_str) - 1);
			location->speed_str[sizeof(location->speed_str) - 1] = 0; // Ensure null termination
			break;

		case 7: // Course
			strncpy(location->course_str, token,
					sizeof(location->course_str) - 1);
			location->course_str[sizeof(location->course_str) - 1] = 0; // Ensure null termination
			break;
		}

		// Get next token
		token = strtok_r(NULL, ",", &saveptr);
		field_index++;
	}

	// Set validity flag based on having latitude and longitude
	location->valid =
			(location->lat_str[0] != '\0' && location->lon_str[0] != '\0') ?
					1 : 0;

	DEBUG_PRINT("Parsed location: lat=%s, lon=%s\n", location->lat_str,
			location->lon_str);

	return (location->valid) ? SIM868_SUCCESS : SIM868_NO_FIX;
}

void SIM868_sendGetRequest(UART_HandleTypeDef *huart) {
	char response[MAX_RESPONSE_LEN];
	// List of AT commands to execute
	const char *commands[] = { "AT",  // Check module responsiveness
			"AT+CIPSHUT",  // Close any open network connection
			"AT+SAPBR=3,1,\"Contype\",\"GPRS\"",  // Open GPRS context
			"AT+SAPBR=3,1,\"APN\",\"internet\"",  // Set APN for network
			"AT+SAPBR=1,1",  // Open GPRS context bearer
			"AT+HTTPINIT",  // Initialize HTTP service
			"AT+HTTPPARA=\"CID\",1",  // Set HTTP context identifier
			"AT+HTTPPARA=\"URL\",\"https://httpbin.org/get\"", // Set URL for HTTP request
			"AT+HTTPSSL=1",  // Enable SSL (optional)
			"AT+HTTPACTION=0",  // Send HTTP GET request
			"AT+HTTPREAD",  // Read HTTP response
			"AT+HTTPTERM",  // Terminate HTTP service
			"AT+CIPSHUT",  // Close network connection
			"AT+SAPBR=0,1"  // Close GPRS context bearer
			};

	// Iterate over the command list and send each command
	for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
		if (sendATCommand(huart, commands[i], response, UART_TIMEOUT)
				!= SIM868_SUCCESS) {
			DEBUG_PRINT("Command failed: %s\n", commands[i]);
			return;
		}
	}

	// Optional: Add custom handling for specific responses, like checking HTTP status code or data
}

void SIM868_sendPostData(UART_HandleTypeDef *huart, const char *json) {
	char response[MAX_RESPONSE_LEN];
	char cmd[200];

	// List of AT commands to execute before sending data
	const char *commands[] = { "AT",  // 1. Basic connectivity
			"AT+CIPSHUT",  // 2. Shutdown previous connections
			"AT+SAPBR=3,1,\"Contype\",\"GPRS\"",  // 3. GPRS Setup
			"AT+SAPBR=3,1,\"APN\",\"internet\"",  // APN setup
			"AT+SAPBR=1,1",  // Open GPRS context bearer
			"AT+HTTPINIT",  // 4. Init HTTP
			"AT+HTTPPARA=\"CID\",1",  // Set HTTP context ID
			"AT+HTTPSSL=1",  // Enable SSL for HTTPS
			"AT+HTTPPARA=\"URL\",\"https://httpbin.org/post\"", // 5. Set URL
			"AT+HTTPPARA=\"CONTENT\",\"application/json\"" // Set content type
			};

	// Iterate over the command list and send each command
	for (int i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
		if (sendATCommand(huart, commands[i], response, UART_TIMEOUT)
				!= SIM868_SUCCESS) {
			DEBUG_PRINT("Command failed: %s\n", commands[i]);
			return;
		}
	}

	// 6. Prepare JSON data length
	DEBUG_PRINT(cmd, sizeof(cmd), "AT+HTTPDATA=%lu,10000",
			(unsigned long) strlen(json));
	if (sendATCommand(huart, cmd, response, UART_TIMEOUT) != SIM868_SUCCESS) {
		DEBUG_PRINT("Failed to send HTTP data length\n");
		return;
	}

	// 7. Wait for the "DOWNLOAD" prompt and then send the JSON data
	HAL_Delay(500); // Wait for "DOWNLOAD" prompt
	HAL_UART_Transmit(huart, (uint8_t*) json, strlen(json), UART_TIMEOUT);
	HAL_Delay(1000);  // Give time for data to be accepted

	// 8. Execute POST
	if (sendATCommand(huart, "AT+HTTPACTION=1", response, 10000)
			!= SIM868_SUCCESS) {
		DEBUG_PRINT("Failed to execute POST request\n");
		return;
	}

	// 9. Read response
	if (sendATCommand(huart, "AT+HTTPREAD", response, 10000)
			!= SIM868_SUCCESS) {
		DEBUG_PRINT("Failed to read HTTP response\n");
		return;
	}

	// 10. Close everything
	const char *closeCommands[] = { "AT+HTTPTERM",  // Terminate HTTP
			"AT+CIPSHUT",  // Shut down network connection
			"AT+SAPBR=0,1"  // Close GPRS context
			};

	// Iterate over the close command list and send each command
	for (int i = 0; i < sizeof(closeCommands) / sizeof(closeCommands[0]); i++) {
		if (sendATCommand(huart, closeCommands[i], response, UART_TIMEOUT)
				!= SIM868_SUCCESS) {
			DEBUG_PRINT("Failed to send close command: %s\n", closeCommands[i]);
			return;
		}
	}
}

SIM868_Status_t sendMQTTPacket(UART_HandleTypeDef *huart, uint8_t *data,
		uint16_t len, uint16_t timeout) {

	DEBUG_PRINT(">> Sending MQTT Packet...\n");

	// Transmit raw binary data
	if (HAL_UART_Transmit(huart, data, len, timeout) != HAL_OK) {
		DEBUG_PRINT("#RED#Failed to transmit raw data\n");
		return SIM868_ERROR;
	}

//	 Wait for a response from SIM868 after raw data (e.g. "SEND OK" or "ERROR") (debug)
	memset(rxBuffer, 0, MAX_RESPONSE_LEN);
//
	HAL_UART_Receive(huart, rxBuffer, MAX_RESPONSE_LEN - 1, HAL_TIMEOUT);

//	DEBUG_PRINT("#GRN#Response after raw send: %s\n", rxBuffer);

//	 Copy response and add null terminator
//	char response[MAX_RESPONSE_LEN];
//	memcpy(response, (char*) rxBuffer + 2, MAX_RESPONSE_LEN - 1);

// Always null-terminate the received buffer
	rxBuffer[sizeof(rxBuffer) - 1] = '\0';
	DEBUG_PRINT("#ORG#SendAtCommand = %s\n", (char* ) rxBuffer + 2);
//	DEBUG_PRINT("#ORG#Raw data transmitted (%d bytes)\n", len);
//	DEBUG_PRINT(">> Done!\n");
	return SIM868_SUCCESS;
}
void mqtt_publish_packet(UART_HandleTypeDef *huart, const char *topic,
		const char *payload) {
	uint8_t mqtt_packet[MAX_PACKET_SIZE];
	// Calculate lengths of topic and payload
	size_t topic_length = strlen(topic);
	size_t payload_length = strlen(payload);

	// Remaining length: topic length (2 bytes) + topic + payload
	size_t remaining_length = 2 + topic_length + payload_length;

	// Fixed header: PUBLISH (0x30), Remaining Length (calculated)
	mqtt_packet[0] = 0x30;                     // PUBLISH packet type
	mqtt_packet[1] = (uint8_t) remaining_length; // Remaining Length

	// Topic length: 2 bytes (high byte, low byte)
	mqtt_packet[2] = (uint8_t) (topic_length >> 8);   // High byte
	mqtt_packet[3] = (uint8_t) (topic_length & 0xFF); // Low byte

	// Topic: Copy the topic string into the packet
	memcpy(&mqtt_packet[4], topic, topic_length);

	// Payload: Copy the payload string into the packetC
	memcpy(&mqtt_packet[4 + topic_length], payload, payload_length);

	// Add 0x1A at the end of the payload
	mqtt_packet[4 + topic_length + payload_length] = 0x1A;
	/**********************************************************************/
//	size_t packet_length = 2 + 2 + strlen(topic) + strlen(payload) + 1;
	// Print the generated packet (debug)
//	DEBUG_PRINT("Generated MQTT Publish Packet: \n");
//	for (size_t i = 0; i < packet_length; i++) {
//		DEBUG_PRINT("0x%02X ", mqtt_packet[i]);
//	}
//	DEBUG_PRINT("\n");
	/**********************************************************************/
	char response[MAX_RESPONSE_LEN];
	// send MQTT data packet
//	HAL_Delay(1000);
//	sendATCommand(huart, "AT+CSQ", response, UART_TIMEOUT);
	sendATCommand(huart, "AT+CIPSEND", response, UART_TIMEOUT);
	sendMQTTPacket(huart, mqtt_packet, sizeof(mqtt_packet), 2000);
//	HAL_Delay(500);
}
