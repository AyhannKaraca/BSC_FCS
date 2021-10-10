/*
 * mpu6050.h
 *
 *  Created on: Sep 7, 2021
 *      Author: Ayhan Karaca
 */

#ifndef MODULES_INC_MPU6050_H_
#define MODULES_INC_MPU6050_H_

#include <stdbool.h>
#include "stm32f407xx.h"
#include <math.h>
#include "stm32f4xx_hal.h"
#include "../../Drivers/Modules/Inc/delay.h"

#define MPU6050_ADRS 0xD0 // slave adresinin 1 bit sola kaydırılmış değeri.
#define MPU6050_VERIFICATION 0x68 // sensörün çalıştığını doğrulayan değer.
#define WHO_AM_I 0x75 // sensörün çalıştığının anlaşılacağı register adresi.
#define PWR_MGMT_1 0x6B // güç1 register adresi.
#define SMPRT_DIV 0x19 // Örnekleme hızının ayarlandığı register adresi.
#define ACCEL_CONFIG 0x1C //ivmeölcer konfigürasyon register adresi.
#define GYRO_CONFIG 0x1B //gyro konfigürasyon register adresi.
#define DATA_STRT_REG 0x3B // bütün datayı okuyacağım register baslangıc adresi.
#define CONFIG 0x1A // filtre ayarlarının yapıldığı register adresi.

I2C_HandleTypeDef hi2c1;

int16_t raw_gyro_x, raw_gyro_y, raw_gyro_z, raw_acc_x, raw_acc_y, raw_acc_z, raw_temperature;
int32_t gyro_x_cal, gyro_y_cal, gyro_z_cal, acc_x_cal, acc_y_cal;
uint8_t check, data, mpu6050_datas[14];
uint32_t time,acc_total_vector;
float temperature, pitch, roll, angle_roll_acc, angle_pitch_acc, angle_pitch_output, angle_roll_output, angle_yaw_output;
bool set_gyro_angles;

void MPU6050_Init(void);
void MPU6050_raw_data(void);
void MPU6050_Calibration(void);
void MPU6050_Gyro_data(void);

#endif /* MODULES_INC_MPU6050_H_ */
