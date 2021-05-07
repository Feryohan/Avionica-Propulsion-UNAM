//                                --- CONEXIONES ---
//   Magnetometro GY-271      IMU GY-521     Modulo MicroSD        RTC
//        VCC - 5V             VCC - 5V        GND - GND        SCL - A5
//        GND - GND            GND - GND       VCC - 5V         SDA - A4
//        SCL - A5             SCL - A5       MISO - D12        VCC - 5V
//        SDA - A4             SDA - A4       MOSI - D11        GND - GND
//                                             SCK - D13
//                                              CS - D10

//Se obtienen los valores raw de ambos GY con el uso de las librerias
//que se encuentran en las siguientes páginas:
//-> https://www.luisllamas.es/brujula-magnetica-con-arduino-compass-digital-hmc5883/
//-> https://github.com/kriswiner/MPU6050/tree/master/MPU6050Library
//-> El RTC envía información cada vez que transcurre un segundo

//  - - Direcciones - -
//-> MPU6050   - 0x69 (Con una alimentación de 3.3 V en el pin AD0)
//-> HMC5883L  - 0x1E
//-> RTC - 0x57 / 0x68

//--- BIBLIOTECAS ---
#include <Wire.h>        //Libreria para la comunicacion I2C
#include <HMC5883L.h>    //Libreria para el magnetometro
#include <MPU6050.h>     //Libreria para la IMU
#include <SPI.h>         //Libreria interfaz SPI
#include <SD.h>          //Libreria para tarjetas SD
#include <EEPROM.h>      //Libreria que permite guardar valores cuando el arduino se apaga            
#include <RTClib.h>      //Libreria para el manejo del modulo RTC

//--- DEFINICIONES ---
#define SSpin 10         //Pin Slave Select para el modulo micro SD

//--- VARIABLES ---
//-> Magnetometro
HMC5883L magnetometro;
int16_t mx, my, mz;

//-> IMU
int16_t accelCount[3];           // Stores the 16-bit signed accelerometer sensor output
float ax, ay, az;                // Stores the real accel value in g's
int16_t gyroCount[3];            // Stores the 16-bit signed gyro sensor output
float gx, gy, gz;                // Stores the real gyro value in degrees per seconds
float gyroBias[3], accelBias[3]; // Bias corrections for gyro and accelerometer
float SelfTest[6];               // Gyro and accelerometer self-test sensor output
uint32_t count = 0;
float aRes, gRes;                // Scale resolutions per LSB for the sensors
MPU6050lib mpu;

//-> RTC
RTC_DS3231 rtc;                  // crea objeto del tipo RTC_DS3231
unsigned long tiempo1 = 0;       //Tiempos necesarios para determinar cuando a pasado un segundo
unsigned long tiempo2 = 0;       //y así tomar la lectura del reloj

//-> Módulo MicroSD
//File archivo;                  //Objeto "archivo" del tipo File
//byte nFile;                    //Numero de archivo
