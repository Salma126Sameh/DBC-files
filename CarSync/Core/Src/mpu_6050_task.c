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

//#undef DEBUG
#include "debug.h"

extern LoRa myLoRa;
extern bool vehicleFlipped;

/* Global variables for the last read angles and time */
unsigned long last_read_time = 0;
double last_x_angle = 0.0;
double last_y_angle = 0.0;

/* Function to update the last read angles */
void set_last_read_angle_data(unsigned long time, double x, double y, double z) {
    last_read_time = time;
    last_x_angle = x;
    last_y_angle = y;
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


/* Function to handle MPU6050 sensor readings and processing */
void MPU6050_Data(I2C_HandleTypeDef *I2Cx, MPU6050_t *MPU6050, bool *vehicleFlipped) {
    MPU6050_Read_All(I2Cx, MPU6050);
    double angle_x = MPU6050->KalmanAngleX;
    double angle_y = MPU6050->KalmanAngleY;

    // Update the last read angles
    set_last_read_angle_data(HAL_GetTick(), angle_x, angle_y, 0.0);

    // Print angle values using DEBUG_PRINT
         DEBUG_PRINT("\nAngle values\n"
                     " X:%3.4f\n Y:%3.4f\n"
                     "Temperature:%3.4f\n\n\n",
                     angle_x,
                     angle_y,
                     MPU6050->Temperature);

         // Detect if the vehicle has flipped (e.g., tilt > 45 degrees)
             if (fabs(angle_x) > 45.0 || fabs(angle_y) > 45.0) {
                 *vehicleFlipped = true;  // Set the global accident flag
             } else {
                 *vehicleFlipped = false;
             }

   }


