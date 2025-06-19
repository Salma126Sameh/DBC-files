#include "lora_config.h"
#include "main.h"
#include "SX1278.h"
#include "usbd_cdc_if.h"

extern SPI_HandleTypeDef hspi1;
char buffer[256];

void LoRa_Init(void) {
    myLoRa.CS_port = GPIOB;
    myLoRa.CS_pin = GPIO_PIN_0;
    myLoRa.reset_port = GPIOB;
    myLoRa.reset_pin = GPIO_PIN_1;
    myLoRa.DIO0_port = GPIOB;
    myLoRa.DIO0_pin = GPIO_PIN_2;
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
        sprintf(buffer, "LoRa Init Failed\r\n");
        CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));
        while (1);
    }
    // Send initialization message
    sprintf(buffer, "LoRa Transmitter & Receiver initialized\r\n");
    CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));
	HAL_Delay(3000);
    sprintf(buffer, "CarSYNC_1\r\n");
    CDC_Transmit_FS((uint8_t*)buffer, strlen(buffer));



}
