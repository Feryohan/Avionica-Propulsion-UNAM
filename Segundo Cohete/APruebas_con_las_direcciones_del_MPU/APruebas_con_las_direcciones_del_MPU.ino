#include <Wire.h>        //Libreria para la comunicacion I2C

//Para los registros
byte Gscale = 3;
byte Ascale = 3;
//Para los valores de salida
float Gescala = 2000.0 / 32768.0;
float Aescala = (16.0 / 32768.0)*1000;
int16_t accelCount[3];           // Stores the 16-bit signed accelerometer sensor output
float ax, ay, az;                // Unidades = G's
int16_t gyroCount[3];            // Stores the 16-bit signed gyro sensor output
float gx, gy, gz;  
#define MPU6050_ADDRESS  0x69
#define PWR_MGMT_1       0x6B
#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define INT_STATUS       0x3A
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B  
#define ACCEL_CONFIG     0x1C     
#define ACCEL_XOUT_H  0x3B
#define ACCEL_XOUT_L  0x3C
#define ACCEL_YOUT_H  0x3D
#define ACCEL_YOUT_L  0x3E
#define ACCEL_ZOUT_H  0x3F
#define ACCEL_ZOUT_L  0x40
#define GYRO_XOUT_H   0x43
#define GYRO_XOUT_L   0x44
#define GYRO_YOUT_H   0x45
#define GYRO_YOUT_L   0x46
#define GYRO_ZOUT_H   0x47
#define GYRO_ZOUT_L   0x48
