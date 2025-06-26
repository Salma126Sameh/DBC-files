#include "lora_config.h"
#include "main.h"
#include "SX1278.h"
#include "debug.h"

extern SPI_HandleTypeDef hspi1;
char buffer[256];

void LoRa_Init(void) {
    myLoRa.CS_port = GPIOA;
    myLoRa.CS_pin = GPIO_PIN_4;
    myLoRa.reset_port = GPIOA;
    myLoRa.reset_pin = GPIO_PIN_3;
    myLoRa.DIO0_port = GPIOB;
    myLoRa.DIO0_pin = GPIO_PIN_0;
    myLoRa.hSPIx = &hspi1;

    myLoRa.frequency = 433;
    myLoRa.spredingFactor = SF_7;
    myLoRa.bandWidth = BW_125KHz;
    myLoRa.crcRate = CR_4_5;
    myLoRa.power = POWER_20db;
    myLoRa.overCurrentProtection = 130;
    myLoRa.preamble = 8;


    // Initialize LoRa
    if (LoRa_init(&myLoRa) != LORA_OK) {
    	DEBUG_PRINT("LoRa Init Failed\r\n");
        while (1);
    }
    // Send initialization message
    DEBUG_PRINT("LoRa Transmitter & Receiver initialized\r\n");
       DEBUG_PRINT("CarSYNC_1\r\n");



}
