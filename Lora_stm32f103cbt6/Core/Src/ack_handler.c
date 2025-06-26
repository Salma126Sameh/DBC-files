#include "ack_handler.h"
#include "crc_calculator.h"
#include<stdio.h>
#include<string.h>
#include "main.h"
#include "debug.h"

uint8_t loraRxBuffer[128];
extern LoRa myLoRa;


bool receive_ack_TX(void) {
	uint8_t ackByte = 0;
	char buffer[64];
	bool responseReceived = false;
	DEBUG_PRINT("Waiting for ACK...\r\n");

	uint32_t startTime = HAL_GetTick();
	while (HAL_GetTick() - startTime < 2000) { // Wait up to 2 seconds
		int receivedLen = LoRa_receive(&myLoRa, &ackByte, 1);

		if (receivedLen == 1) {
			snprintf(buffer, sizeof(buffer), "ACK byte received: %d\r\n", ackByte);
			  DEBUG_PRINT(buffer);

			if (ackByte == 1) {
				responseReceived = true;  // ACK
			} else {
				responseReceived = false; // NACK
			}
			break;
		} else {
			DEBUG_PRINT("No data received yet...\r\n");

			HAL_Delay(100);
		}
	}

	if (!responseReceived) {
		snprintf(buffer, sizeof(buffer), "No ACK received (timeout or NACK)\r\n");
		        DEBUG_PRINT(buffer);
	} else {
		DEBUG_PRINT("ACK confirmed!\r\n");
	}

	return responseReceived;
}
//void sendMessageAndWaitForAck(LoRa *lora, uint8_t *messageBuffer) {

void sendMessageAndWaitForAck_TX(LoRa *lora, const uint8_t *messageBuffer) {

	//uint8_t msgLen = strlen(messageBuffer); // Cast to char* to fix warning
	uint8_t msgLen = strlen((const char*)messageBuffer); // Cast to char* to fix warning

	// Calculate CRC for the original message
	uint32_t crc = Calculate_CRC((uint8_t*) messageBuffer, msgLen); // 0xf65adea6

	// Delay for 200ms (maybe for LoRa to settle, or other reasons)
	HAL_Delay(500);

	// Transmit "CRC: " via CDC
	    DEBUG_PRINT("CRC: ");

	// Format the CRC as a string
	char crcString[11]; // Enough space for "0xXXXXXXXX" + null terminator

	snprintf(crcString, sizeof(crcString), "0x%08lX", crc); // Format CRC as an 8-digit hex number

	// Transmit the formatted CRC string via CDC
	DEBUG_PRINT(crcString);
	 DEBUG_PRINT("\r\n");


	// Buffer to hold the CRC in hexadecimal string format
	snprintf(crcString, sizeof(crcString), "0x%08lX", crc); // Convert CRC to "0xXXXXXXXX"


	// Total length of the packet: message + CRC string
	uint8_t packetLen = msgLen + strlen(crcString);
	char packet[packetLen + 1]; // +1 for null terminator
	// Copy the message into the packet
	memcpy(packet, messageBuffer, msgLen);
	// Append the CRC string to the packet
	memcpy(packet + msgLen, crcString, strlen(crcString) + 1); // Include null terminator


	// Transmit the combined packet via LoRa
	uint8_t result = LoRa_transmit(lora, (uint8_t*) packet, packetLen, 1000);

	// Check for acknowledgment
	if (result) {
		bool ackReceived = receive_ack_TX();
		char buffer[256];
		sprintf(buffer, "ACK raw value: %s\r\n",
				ackReceived ? "true" : "false");
		DEBUG_PRINT(buffer);

		HAL_GPIO_WritePin(Run_GPIO_Port, Run_Pin, GPIO_PIN_SET);
		if (ackReceived) {
			sprintf(buffer, "ACK received successfully.\r\n");

		} else {
			sprintf(buffer, "No ACK received.\r\n");
		}
		DEBUG_PRINT("No ACK received.\r\n");

	}

}


void send_ack_RX(bool isAck) {
	// Debug message via USB
	char ackinfo[64];
	snprintf(ackinfo, sizeof(ackinfo), "ACK_INFO: %s\r\n", isAck ? "true" : "false");
	DEBUG_PRINT(ackinfo);

	// Send 1-byte boolean value over LoRa
	uint8_t response = isAck ? 1 : 0;
	LoRa_transmit(&myLoRa, &response, 1, 1000);
}


uint8_t receiveAndsendACK_RX(LoRa *_LoRa) {

	uint8_t rxSize = LoRa_receive(&myLoRa, loraRxBuffer,sizeof(loraRxBuffer));
	if (rxSize > 0) {
	uint8_t dataLength = rxSize - 10;
	if (dataLength <= 0) {
	 send_ack_RX(false);
	  return 0;
	}

	// Extract the main message
	uint8_t mainMessage[dataLength];
    memcpy(mainMessage, loraRxBuffer, dataLength);

    // Extract CRC string
    char crcStr[11];
    memcpy(crcStr, &loraRxBuffer[dataLength], 10);
	crcStr[10] = '\0';

	// Convert CRC string to actual number
	uint32_t receivedCrc = (uint32_t) strtoul(crcStr, NULL, 16);
	// Calculate expected CRC using only the main message
	uint32_t expectedCrc = Calculate_CRC(mainMessage,dataLength);

	// Compare CRCs
	if (receivedCrc == expectedCrc) {
		DEBUG_PRINT("matched\r\n");
	HAL_Delay(500);
	send_ack_RX(true);
	} else {
	send_ack_RX(false);
    return 0;
	}

	// Print the main message
	char tempBuffer[256];
	for (int i = 0; i < dataLength; i++) {
        tempBuffer[i] = (mainMessage[i] >= 32 && mainMessage[i] <= 126) ? mainMessage[i] : '.';
	}
        tempBuffer[dataLength] = '\0';


        char formattedMessage[1024];
        snprintf(formattedMessage, sizeof(formattedMessage), "Received message: %s\r\n", tempBuffer);
        DEBUG_PRINT(formattedMessage);
    }

    return 1;

}
