/*
bmi270.h

Created: 2025-11-06
Author: Viktor, Nicklas

*/

#ifndef INC_BMI_270_H_
#define INC_BMI_270_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines ------------------------------------------------------------------*/
#define BMI270_I2C hi2c1

extern I2C_HandleTypeDef BMI270_I2C;

typedef enum
{
    BMI270_OK,
    BMI270_ERR_I2C,
    BMI270_ERR_CHIP_ID,
    BMI270_ERR_TIMEOUT
}BMI270_Status;

#define BMI270_ADDRESS              (0x68 << 1)

#define REG_CHIP_ID_BMI270          0x00        // Chip ID
#define REG_CMD                     0x7E        
#define REG_GYR_CONF                0x42
#define REG_ACC_RANGE               0x41
#define REG_GYR_RANGE               0x43
#define REG_ACC_CONF                0x40
#define REG_PWR_CONF                0x7C
#define REG_PWR_CTRL                0x7D
#define REG_INIT_CTRL               0x59
#define REG_INIT_DATA               0x5E
#define REG_INTERNAL_STATUS         0x21
#define REG_ACC_GYRO_DATA           0x0C
#define REG_INIT_ADDR_0             0x5b
#define REG_INIT_ADDR_1             0x5c
#define CHIP_ID                     0x24
//#define CMD_REG                     0x7E

#define SOFT_RESET_CMD              0xB6

int BMI270_Configure(void);
void BMI270_ReadMeasurement(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);

void BMI270_ReadMeasurement_raw(uint8_t sensor_raw[12]);

#endif