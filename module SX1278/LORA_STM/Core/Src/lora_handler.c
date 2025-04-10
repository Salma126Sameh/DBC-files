/*
 * lora_handler.c
 *
 *  Created on: Mar 8, 2025
 *      Author: CarSync
 */

#include "lora_handler.h"
#include "usbd_cdc_if.h"
#include "SX1278.h"
#include "crc_utils.h"
#include <string.h>

#define USB_TX_BUFFER_SIZE  255
#define MIN_PACKET_SIZE     5    // Data + 4-byte CRC

static uint8_t usb_tx_buffer[USB_TX_BUFFER_SIZE];
static volatile bool is_receiving = false;

extern SPI_HandleTypeDef hspi1;
extern CRC_HandleTypeDef hcrc;

LoRa myLoRa;

void LoRa_handle_init(void) {
    myLoRa = newLoRa();
    myLoRa.hSPIx       = &hspi1;
    myLoRa.CS_port     = NSS_GPIO_Port;
    myLoRa.CS_pin      = NSS_Pin;
    myLoRa.reset_port  = RESET_GPIO_Port;
    myLoRa.reset_pin   = RESET_Pin;
    myLoRa.DIO0_port   = DIO0_GPIO_Port;
    myLoRa.DIO0_pin    = DIO0_Pin;

    myLoRa.frequency             = 433;  
    myLoRa.spredingFactor        = SF_7;
    myLoRa.bandWidth             = BW_31_25KHz;
    myLoRa.crcRate               = CR_4_5;
    myLoRa.power                 = POWER_20db;
    myLoRa.overCurrentProtection = 130;
    myLoRa.preamble              = 9;

    uint16_t init_status = LoRa_init(&myLoRa);
    if (init_status == LORA_OK) {
        CDC_Transmit_FS((uint8_t*)"LoRa Transmitter Initialized\r\n", 18);
        LoRa_startReceiving(&myLoRa);
        is_receiving = true;
    } else {
        snprintf((char*)usb_tx_buffer, USB_TX_BUFFER_SIZE,
                "LoRa Init Failed - Status: %d\r\n", init_status);
        CDC_Transmit_FS(usb_tx_buffer, strlen((char*)usb_tx_buffer));
        Error_Handler();
    }
    HAL_Delay(3000);
}


uint8_t LoRa_SendData(uint8_t *data, uint8_t length) {
    if (!data || length == 0) return 0;
    return LoRa_transmit(&myLoRa, data, length, 1000);
}

uint8_t LoRa_ReceiveData(uint8_t *buffer, uint8_t max_length) {
    if (!buffer || max_length == 0) return 0;
    return LoRa_receive(&myLoRa, buffer, max_length);
}

void LoRa_SendACK(void) {
    uint8_t ack[] = "ACK";
    LoRa_transmit(&myLoRa, ack, 3, 1000);
}

void LoRa_SendNotACK(void) {
    uint8_t nack[] = "NACK";
    LoRa_transmit(&myLoRa, nack, 4, 1000);
}

bool WaitForACK(void) {
    uint32_t start_time = HAL_GetTick();
    uint8_t ack_buffer[10];

    while (HAL_GetTick() - start_time < ACK_TIMEOUT_MS) {
        uint8_t len = LoRa_receive(&myLoRa, ack_buffer, sizeof(ack_buffer) - 1);
        if (len > 0) {
            ack_buffer[len] = '\0';
            snprintf((char*)usb_tx_buffer, USB_TX_BUFFER_SIZE,
                    "Received response: %s\r\n", ack_buffer);
            CDC_Transmit_FS(usb_tx_buffer, strlen((char*)usb_tx_buffer));
            HAL_Delay(1);  // Small delay to ensure buffer flush
            if (strncmp((char*)ack_buffer, "ACK", 3) == 0) return true;
            if (strncmp((char*)ack_buffer, "NACK", 4) == 0) return false;
        }
    }
    CDC_Transmit_FS((uint8_t*)"No response received within timeout\r\n", 36);
    HAL_Delay(1);  // Small delay to ensure buffer flush
    return false;
}

bool LoRa_SendWithRetry(uint8_t *data, uint8_t length, uint16_t *status) {
    if (!data || length == 0 || length > 251) {
        if (status) *status = LORA_LARGE_PAYLOAD;
        return false;
    }

    uint8_t retry_count = 0;
    uint32_t crc = CalculateCRC(&hcrc, data, length);
    uint8_t tx_buffer[length + 4];

    memcpy(tx_buffer, data, length);
    memcpy(tx_buffer + length, &crc, 4);

    // Debug: Print the data being sent
    data[length] = '\0';  // Null-terminate for printing
    snprintf((char*)usb_tx_buffer, USB_TX_BUFFER_SIZE,
            "Sending data: %s\r\n", data);
    CDC_Transmit_FS(usb_tx_buffer, strlen((char*)usb_tx_buffer));
    HAL_Delay(1);

    while (retry_count < MAX_RETRIES) {
        CDC_Transmit_FS((uint8_t*)"Attempting transmission...\r\n", 27);
        HAL_Delay(1);
        if (LoRa_transmit(&myLoRa, tx_buffer, length + 4, 1000)) {
            CDC_Transmit_FS((uint8_t*)"Transmission sent, waiting for ACK...\r\n", 38);
            HAL_Delay(1);
            bool ack_received = WaitForACK();
            if (ack_received) {
                CDC_Transmit_FS((uint8_t*)"Transmission successful\r\n", 25);
                HAL_Delay(1);
                if (status) *status = LORA_OK;
                return true;
            }
            CDC_Transmit_FS((uint8_t*)"\r\n", 2);
            HAL_Delay(1);
        } else {
            snprintf((char*)usb_tx_buffer, USB_TX_BUFFER_SIZE,
                    "Transmission failed at attempt %d\r\n", retry_count + 1);
            CDC_Transmit_FS(usb_tx_buffer, strlen((char*)usb_tx_buffer));
            HAL_Delay(1);
        }

        retry_count++;
        if (retry_count < MAX_RETRIES) {
            snprintf((char*)usb_tx_buffer, USB_TX_BUFFER_SIZE,
                    "Retry attempt: %d\r\n", retry_count + 1);
            CDC_Transmit_FS(usb_tx_buffer, strlen((char*)usb_tx_buffer));
            HAL_Delay(1);
            HAL_Delay(RETRY_DELAY_MS);
        }
    }

    CDC_Transmit_FS((uint8_t*)"Max retries reached, transmission failed\r\n", 42);
    HAL_Delay(1);
    if (status) *status = LORA_UNAVAILABLE;
    return false;
}

bool ForwardDataToSecondLoRa(uint8_t *data, uint8_t length) {
    if (!data || length == 0) {
        CDC_Transmit_FS((uint8_t*)"Invalid forward data\r\n", 22);
        return false;
    }

    uint8_t status = LoRa_transmit(&myLoRa, data, length, 1000);
    if (status) {
        CDC_Transmit_FS((uint8_t*)"Data forwarded successfully\r\n", 29);
        return true;
    }
    CDC_Transmit_FS((uint8_t*)"Failed to forward data\r\n", 24);
    return false;
}



void ProcessReceivedData(uint8_t *data, uint8_t length) {
    CDC_Transmit_FS((uint8_t*)"Processing received data...\r\n", 29);
    if (!data || length < MIN_PACKET_SIZE) {
        LoRa_SendNotACK();
        CDC_Transmit_FS((uint8_t*)"Received packet too small\r\n", 27);
        return;
    }

    uint8_t payload_length = length - 4;
    uint32_t received_crc;
    memcpy(&received_crc, data + payload_length, 4);

    uint32_t calculated_crc = CalculateCRC(&hcrc, data, payload_length);
    if (received_crc == calculated_crc) {
        data[payload_length] = '\0';
        snprintf((char*)usb_tx_buffer, USB_TX_BUFFER_SIZE,
                "Received: %u bytes\r\nCRC: 0x%08lX\r\nData: %s\r\n",
                payload_length, received_crc, data);
        CDC_Transmit_FS(usb_tx_buffer, strlen((char*)usb_tx_buffer));

        CDC_Transmit_FS((uint8_t*)"Sending ACK...\r\n", 16);
        LoRa_SendACK();

        if (!ForwardDataToSecondLoRa(data, payload_length)) {
            CDC_Transmit_FS((uint8_t*)"Forwarding failed\r\n", 19);
        }
    } else {
        snprintf((char*)usb_tx_buffer, USB_TX_BUFFER_SIZE,
                "CRC Error - Received: 0x%08lX, Calculated: 0x%08lX\r\n",
                received_crc, calculated_crc);
        CDC_Transmit_FS(usb_tx_buffer, strlen((char*)usb_tx_buffer));
        CDC_Transmit_FS((uint8_t*)"Sending NACK...\r\n", 17);
        LoRa_SendNotACK();
    }

    if (is_receiving) {
        LoRa_startReceiving(&myLoRa);
    }
}
void SendExampleMessage(void) {
    uint8_t message[] = "Hello LoRa";
    uint16_t status = 0;

    if (LoRa_SendWithRetry(message, sizeof(message) - 1, &status)) {
        CDC_Transmit_FS((uint8_t*)"Example message sent successfully\r\n", 35);
    } else {
        snprintf((char*)usb_tx_buffer, USB_TX_BUFFER_SIZE,
                "Example message failed - Status: %d\r\n", status);
        CDC_Transmit_FS(usb_tx_buffer, strlen((char*)usb_tx_buffer));
    }
    HAL_Delay(LED_BLINK_TIME);  // Ensure LED blinks even if USB fails
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}
