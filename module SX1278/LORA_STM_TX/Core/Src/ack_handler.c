#include "ack_handler.h"
#include "main.h"
#include "crc_calculator.h"

bool receive_ack(void) {
	uint8_t ackData[16];
	char buffer[256];
	bool responseReceived = false;

	uint32_t startTime = HAL_GetTick();
	while (HAL_GetTick() - startTime < 2000) { // Wait up to 2 seconds for response
		if (LoRa_receive(&myLoRa, ackData, 16)) {
			sprintf(buffer, "Received data: %.*s\r\n", 16, ackData);
			CDC_Transmit_FS((uint8_t*) buffer, strlen(buffer));

			if (ackData[0] == 'A' && ackData[1] == 'C' && ackData[2] == 'K') {
				sprintf(buffer, "Received ACK: %s\r\n", ackData);
				CDC_Transmit_FS((uint8_t*) buffer, strlen(buffer));
				responseReceived = true;
				break;
			} else if (ackData[0] == 'N' && ackData[1] == 'A'
					&& ackData[2] == 'C' && ackData[3] == 'K') {
				sprintf(buffer, "Received NACK: %s\r\n", ackData);
				CDC_Transmit_FS((uint8_t*) buffer, strlen(buffer));
				responseReceived = true;
				break;
			}
		}
	}

	if (!responseReceived) {
		sprintf(buffer, "No response received\r\n");
		CDC_Transmit_FS((uint8_t*) buffer, strlen(buffer));
	}

	return responseReceived;
}

bool sendMessageAndWaitForAck(LoRa *lora, uint8_t *messageBuffer, uint8_t totalLen) {
    const char originalMessage[] = "Hello, LoRa!";
    uint8_t msgLen = strlen(originalMessage);

    // Calculate CRC for the original message
    uint32_t crc = Calculate_CRC((uint8_t*) originalMessage, msgLen);

    // Delay for 200ms (maybe for LoRa to settle, or other reasons)
    HAL_Delay(200);

    // Transmit "CRC: " via CDC
    CDC_Transmit_FS((uint8_t*) "CRC: ", strlen("CRC: "));

    // Format the CRC as a string
    char crcString[15]; // Buffer for CRC in hex + null terminator
    snprintf(crcString, sizeof(crcString), "0x%08X\r\n", (unsigned int) crc); // Format CRC as an 8-digit hex number

    // Transmit the formatted CRC string via CDC
    CDC_Transmit_FS((uint8_t*) crcString, strlen(crcString));

    // Combine the message and the CRC into a single packet
    char packet[256];
    snprintf(packet, sizeof(packet), "%s0x%08X", originalMessage, (unsigned int) crc);

    // Transmit the combined packet via LoRa
    uint8_t result = LoRa_transmit(lora, (uint8_t*) packet, strlen(packet), 1000);

    // Check for acknowledgment
    if (result) {
        bool responseReceived = receive_ack();
        return responseReceived;
    } else {
        return false;
    }
}

