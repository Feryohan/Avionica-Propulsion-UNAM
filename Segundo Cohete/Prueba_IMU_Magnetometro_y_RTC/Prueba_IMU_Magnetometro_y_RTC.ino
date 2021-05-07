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
//-> https://www.luisllamas.es/arduino-orientacion-imu-mpu-6050/
//-> El RTC envía información cada vez que transcurre un segundo

//--- BIBLIOTECAS ---
#include <Wire.h>        //Libreria para la comunicacion I2C
#include <I2Cdev.h>      //Libreria modificada para la comunicacion I2C
#include <HMC5883L.h>    //Libreria para el magnetometro
#include <MPU6050.h>     //Libreria para la IMU
#include <SPI.h>         //Libreria interfaz SPI
#include <SD.h>          //Libreria para tarjetas SD
#include <EEPROM.h>      //Libreria que permite guardar valores cuando el arduino se apaga            
#include <RTClib.h>      //Libreria para el manejo del modulo RTC

//--- DEFINICIONES ---
#define SSpin 10         //Pin Slave Select para el modulo micro SD

//--- VARIABLES ---
HMC5883L magnetometro;
int16_t mx, my, mz;

const int mpuAddress = 0x68;
MPU6050 mpu(mpuAddress);
int ax, ay, az;
int gx, gy, gz;

RTC_DS3231 rtc;          // crea objeto del tipo RTC_DS3231
unsigned long tiempo1 = 0;
unsigned long tiempo2 = 0;

//File archivo;            //Objeto "archivo" del tipo File
//byte nFile;              //Numero de archivo
