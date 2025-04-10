
#include "ack_handler.h"
#include "lora_config.h"
#include "main.h"

void send_ack(bool isAck) {
    char response[5];
    if (isAck) {
        strcpy(response, "ACK");
    } else {
        strcpy(response, "NACK");
    }
    LoRa_transmit(&myLoRa, (uint8_t*)response, strlen(response), 1000);
}
