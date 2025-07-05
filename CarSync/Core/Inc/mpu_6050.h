/*
 * mpu_6050.h
 *
 *  Created on: Dec 18, 2024
 */

#ifndef MPU_6050_H_
#define MPU_6050_H_

// Header Files
#include "main.h"


// Define constants and register addresses for MPU6050
#define RAD_TO_DEG 57.295779513082320876798154814105  // Conversion factor from radians to degrees
#define WHO_AM_I_REG         0x75  // Register to verify device identity
#define PWR_MGMT_1_REG       0x6B  // Power management register
#define SMPLRT_DIV_REG       0x19  // Sample rate divider register
#define ACCEL_CONFIG_REG     0x1C  // Accelerometer configuration register
#define ACCEL_XOUT_H_REG     0x3B  // Accelerometer X-axis high byte output register
#define TEMP_OUT_H_REG       0x41  // Temperature high byte output register
#define GYRO_CONFIG_REG      0x1B  // Gyroscope configuration register
#define GYRO_XOUT_H_REG      0x43  // Gyroscope X-axis high byte output register

// MPU6050 I2C address (default)
#define MPU6050_ADDR 0xD0

// MPU6050 data structure
typedef struct {
    // Raw accelerometer data
    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    // Processed accelerometer data in G's
    double Ax;
    double Ay;
    double Az;

    // Raw gyroscope data
    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
    // Processed gyroscope data in degrees per second (dps)
    double Gx;
    double Gy;
    double Gz;

    // Temperature data in degrees Celsius
    float Temperature;

    // Kalman-filtered angles
    double KalmanAngleX;  // Filtered angle for X-axis
    double KalmanAngleY;  // Filtered angle for Y-axis
} MPU6050_t;

// Kalman filter structure for angle calculation
typedef struct {
    double Q_angle;   // Process noise variance for the angle
    double Q_bias;    // Process noise variance for the bias
    double R_measure; // Measurement noise variance
    double angle;     // Current angle estimation
    double bias;      // Current bias estimation
    double P[2][2];   // Error covariance matrix
} Kalman_t;

// Function prototypes

/**
 * @brief Initialize the MPU6050 sensor.
 * @param I2Cx: Pointer to the I2C handler.
 * @return 0 on success, non-zero on failure.
 */
uint8_t MPU6050_Init(I2C_HandleTypeDef *I2Cx);

/**
 * @brief Read raw accelerometer data and convert to G's.
 * @param I2Cx: Pointer to the I2C handler.
 * @param DataStruct: Pointer to the MPU6050 data structure.
 */
void MPU6050_Read_Accel(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

/**
 * @brief Read raw gyroscope data and convert to degrees per second (dps).
 * @param I2Cx: Pointer to the I2C handler.
 * @param DataStruct: Pointer to the MPU6050 data structure.
 */
void MPU6050_Read_Gyro(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

/**
 * @brief Read temperature data in degrees Celsius.
 * @param I2Cx: Pointer to the I2C handler.
 * @param DataStruct: Pointer to the MPU6050 data structure.
 */
void MPU6050_Read_Temp(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

/**
 * @brief Read all sensor data: accelerometer, gyroscope, and temperature.
 * @param I2Cx: Pointer to the I2C handler.
 * @param DataStruct: Pointer to the MPU6050 data structure.
 */
void MPU6050_Read_All(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

/**
 * @brief Calculate the angle using the Kalman filter.
 * @param Kalman: Pointer to the Kalman filter structure.
 * @param newAngle: New angle measurement.
 * @param newRate: New rate measurement from gyroscope.
 * @param dt: Time difference between measurements in seconds.
 * @return Filtered angle.
 */
double Kalman_getAngle(Kalman_t *Kalman, double newAngle, double newRate, double dt);

#endif /* MPU_6050_H_ */
