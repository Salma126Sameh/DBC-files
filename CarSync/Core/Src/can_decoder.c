/*
 * can_decoder.c
 *
 *  Created on: May 8, 2025
 *      Author: omar7
 */


#include "can_decoder.h"
#include "bmw_e9x_e8x.h"
#include "main.h"
#include <stdio.h>  // Include for snprintf
#include <string.h>


int decoded_speed = 0;
int decoded_temp = 0;
int decoded_fuel = 0;

#define SPEED_THRESHOLD 50.0
#define TEMP_THRESHOLD      80.0
#define FUEL_THRESHOLD      70.0
//double decoded_speed = 0.0;
//uint8_t charToTransmit[24];
extern UART_HandleTypeDef huart1;
void CAN_Decode(uint8_t* rawData, uint8_t length)
{
    if (length < 12) return;

    uint32_t messageId = (rawData[0] << 24) | (rawData[1] << 16) |
                         (rawData[2] << 8) | rawData[3];

    uint8_t payload[8];
    for (int i = 0; i < 8; ++i)
        payload[i] = rawData[4 + i];

    if (messageId == BMW_E9X_E8X_SPEED_FRAME_ID)
    {
        struct bmw_e9x_e8x_speed_t speedMsg;
        if (bmw_e9x_e8x_speed_unpack(&speedMsg, payload, sizeof(payload)) == 0)
        {
            double speed = bmw_e9x_e8x_speed_vehicle_speed_decode(speedMsg.vehicle_speed);
            decoded_speed = (int)speed;

            if (speed > SPEED_THRESHOLD)
            {
                HAL_GPIO_WritePin(Run_GPIO_Port, Run_Pin, GPIO_PIN_SET);
            }
            else
            {
                HAL_GPIO_WritePin(Run_GPIO_Port, Run_Pin, GPIO_PIN_RESET);
            }


//          HAL_UART_Transmit_DMA(&huart2, (uint8_t*)&speed, sizeof(speed));
            int int_speed = (int)speed;
            char speedStr[8];
            int len = sprintf(speedStr, "%d\n", int_speed);
            HAL_UART_Transmit(&huart1, (uint8_t*)speedStr, len, HAL_MAX_DELAY);

        }


    }

    else if (messageId == BMW_E9X_E8X_ENGINE_DATA_FRAME_ID)
        {
            struct bmw_e9x_e8x_engine_data_t engineData;
            if (bmw_e9x_e8x_engine_data_unpack(&engineData, payload, sizeof(payload)) == 0)
            {
                double temp = bmw_e9x_e8x_engine_data_engine_temperature_decode(engineData.engine_temperature);
                decoded_temp = (int)temp;

                if (temp > TEMP_THRESHOLD)
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);  // Temp LED ON
                else
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // Temp LED OFF

                int int_temp = (int)temp;
                char tempStr[16];
                int len = snprintf(tempStr, sizeof(tempStr), "TMP:%d\n", int_temp);
                HAL_UART_Transmit(&huart1, (uint8_t*)tempStr, len, HAL_MAX_DELAY);
            }
        }

    else if (messageId == BMW_E9X_E8X_FUEL_LEVEL_FRAME_ID)
        {
            struct bmw_e9x_e8x_fuel_level_t fuelMsg;
            if (bmw_e9x_e8x_fuel_level_unpack(&fuelMsg, payload, sizeof(payload)) == 0)
            {
                double fuel = bmw_e9x_e8x_fuel_level_fuel_percentage_decode(fuelMsg.fuel_percentage);
                decoded_fuel = (int)fuel;

                if (fuel < FUEL_THRESHOLD)
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);  // Fuel LED ON
                else
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET); // Fuel LED OFF

                int int_fuel = (int)fuel;
                char fuelStr[16];
                int len = snprintf(fuelStr, sizeof(fuelStr), "FUL:%d\n", int_fuel);
                HAL_UART_Transmit(&huart1, (uint8_t*)fuelStr, len, HAL_MAX_DELAY);

            }
        }
}
