/*
 * communication.c
 *
 *  Created on: Jul 5, 2025
 *      Author: Eng Hassan Sameh
 */

#include "communication.h"
#include "SIM868.h"
#include "main.h"
#include "can_decoder.h"

void CheckSignalAndSwitchCommunication(UART_HandleTypeDef *huart, bool *useLoRa)
{
    SIM868_Status_t signalStatus = SIM868_check_signal(huart);
    if (signalStatus == SIM868_SUCCESS)
    {
        *useLoRa = false;
    }
    else
    {
        *useLoRa = true;
    }
}

void SendGPSDataViaMQTT(UART_HandleTypeDef *huart)
{
    GPS_Location_t location;
    SIM868_GetGPSLocation(huart, &location);
    if (location.valid)
    {
        char gpsMessage[128];
        // Include predefined carID in the message
        const char *carID = "CAR12345";
        snprintf(gpsMessage, sizeof(gpsMessage), "GPS Data: CarID=%s, Lat=%s, Lon=%s", carID, location.lat_str, location.lon_str);
        mqtt_publish_packet(huart, "gps_topic", gpsMessage);
    }
}
void SendMPU6050DataViaMQTT(UART_HandleTypeDef *huart, MPU6050_t *MPU6050, bool useLoRa)
{
    if (!useLoRa)
    {
        char mqttMessage[128];
        // Send Kalman-filtered angles instead of raw accelerometer data
         snprintf(mqttMessage, sizeof(mqttMessage), "MPU6050 Data: AngleX=%f, AngleY=%f", MPU6050->KalmanAngleX, MPU6050->KalmanAngleY);
        mqtt_publish_packet(huart, "mpu6050_topic", mqttMessage);
    }
}

void SendCANDataViaMQTT(UART_HandleTypeDef *huart, bool useLoRa)
{
    uint8_t canData[16];
    CAN_Decode(canData, sizeof(canData));

    if (!useLoRa)
    {
        char canMessage[128];
        snprintf(canMessage, sizeof(canMessage), "Speed: %d, Temp: %d, Fuel: %d", decoded_speed, decoded_temp, decoded_fuel);
        mqtt_publish_packet(huart, "can_topic", canMessage);
    }
}

void SendAccidentDataViaLoRa(LoRa *lora)
{
	const uint8_t loraMessage[] = "Vehicle flipped! Alert received.";
    sendMessageAndWaitForAck_TX(lora, loraMessage);
}


void HandleLoRaCommunication(LoRa *lora, uint8_t *receivedFlag)
{
    if (*receivedFlag)
    {
        *receivedFlag = 0; // Clear the flag
        receiveAndsendACK_RX(lora); // Process the received message
    }
}

