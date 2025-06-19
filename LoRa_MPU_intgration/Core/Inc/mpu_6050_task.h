/*
 * mpu_6050_task.h
 *
 *  Created on: Apr 12, 2025
 *      Author: Eng Hassan Sameh
 */

#ifndef INC_MPU_6050_TASK_H_
#define INC_MPU_6050_TASK_H_


#include "mpu_6050.h"
#include <stdbool.h>

extern bool vehicleFlipped;

// Function to handle MPU6050 sensor readings and processing
void MPU6050_Data(I2C_HandleTypeDef *I2Cx, MPU6050_t *MPU6050);
#endif /* INC_MPU_6050_TASK_H_ */
