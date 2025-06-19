/*
 * mpu_6050_task.c
 *
 *  Created on: Apr 12, 2025
 *      Author: Eng Hassan Sameh
 */

#include "mpu_6050_task.h"
#include "main.h"
#include "ack_handler.h"
#include <math.h>
#include "cmsis_os.h"

extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
extern LoRa myLoRa;
bool vehicleFlipped = false;
extern TaskHandle_t LoRaSenderTaskHandle;

/* Global variables for the last read angles and time */
unsigned long last_read_time = 0;
double last_x_angle = 0.0;
double last_y_angle = 0.0;
//double last_z_angle = 0.0;

/* Function to update the last read angles */
void set_last_read_angle_data(unsigned long time, double x, double y, double z) {
    last_read_time = time;
    last_x_angle = x;
    last_y_angle = y;
 //   last_z_angle = z;
}

/* Function to get the last read time */
unsigned long get_last_time() {
    return last_read_time;
}

/* Function to get the last X angle */
double get_last_x_angle() {
    return last_x_angle;
}

/* Function to get the last Y angle */
double get_last_y_angle() {
    return last_y_angle;
}

/* Function to get the last Z angle */
//double get_last_z_angle() {
//    return last_z_angle;
//}

/* Function to handle MPU6050 sensor readings and processing */
void MPU6050_Data(I2C_HandleTypeDef *I2Cx, MPU6050_t *MPU6050) {
    MPU6050_Read_All(I2Cx, MPU6050);
    double angle_x = MPU6050->KalmanAngleX;
    double angle_y = MPU6050->KalmanAngleY;


    // Check if the vehicle's angle exceeds a certain threshold (e.g., 45 degrees)
      if (fabs(angle_x) > 45.0 || fabs(angle_y) > 45.0) {
    	  vehicleFlipped = true;  // Set the vehicleFlipped flag
    	   BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    	    // Send a message to the LoRa task
    	   vTaskNotifyGiveFromISR(LoRaSenderTaskHandle, &xHigherPriorityTaskWoken);

    	    if (xHigherPriorityTaskWoken) {
    	        taskYIELD();
    	    }
      }

         // const uint8_t accidentMessage[] = "Vehicle flipped! Alert received.";
          //sendMessageAndWaitForAck_TX(&myLoRa, accidentMessage);


    // Update the last read angles
    set_last_read_angle_data(HAL_GetTick(), angle_x, angle_y, 0.0);

    // Print accelerometer values
   // char datax[120];



//    // Print angle values
//    snprintf(datax, sizeof(datax),"\nAngle values\n"
//                   " X:%3.4f\n Y:%3.4f\n"
//                   "Temperature:%3.4f\n\n\n",
//                   angle_x,
//                   angle_y,
//                  MPU6050->Temperature);
//
//  CDC_Transmit_FS((uint8_t*)datax, strlen(datax));
//
}
