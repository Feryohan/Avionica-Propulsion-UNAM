//                                    --- CONEXIONES ---
//   Magnetometro GY-271      IMU GY-521     Modulo MicroSD        RTC            GPS
//        VCC - 5V             VCC - 5V        GND - GND        SCL - A5       VCC - 5V
//        GND - GND            GND - GND       VCC - 5V         SDA - A4        RX - D3 TX
//        SCL - A5             SCL - A5       MISO - D12        VCC - 5V        TX - D5 RX
//        SDA - A4             SDA - A4       MOSI - D11        GND - GND      GND - GND
//                                             SCK - D13
//                                              CS - D10

//         ---- Tiempo Watchdog ----
// MPU necesita más de 1 segundo


//Se obtienen los valores raw de ambos SENSORES con el uso de las librerias que se encuentran en las siguientes páginas:
//-> https://www.luisllamas.es/brujula-magnetica-con-arduino-compass-digital-hmc5883/
//-> https://github.com/kriswiner/MPU6050/tree/master/MPU6050Library
//-> El RTC envía información cada vez que transcurre un segundo

//- - - Con: MicroSD, GPS, MPU, Magnetometro
// -> Almacenamiento: 87% (23634 bytes)
// -> Dinamica: 73% (1513 bytes)

//- - - GPS
// -> Almacenamiento: 17% (5222 bytes)
// -> Dinamica: 11% (225 bytes)

//- - - MPU
// -> Almacenamiento: 20% (6144 bytes)
// -> Dinamica: 5% (102 bytes)

//- - - Magnetometro
// -> Almacenamiento: 6% (1843 bytes)
// -> Dinamica: 2% (41 bytes)

//- - - MicroSD
// -> Almacenamiento: 44% (13516 bytes)
// -> Dinamica: 55% (1126 bytes)

//  - - Direcciones - -
//-> MPU6050   - 0x69 (Con una alimentación de 3.3 V en el pin AD0)
//-> HMC5883L  - 0x1E
//-> RTC       - 0x57 / 0x68

// - - Direcciones de la memoria EEPROM - -
//-> Estado de vuelo: E (69), A (65), D (68)
//-> Estado Barómetro: 0 (No funciona), 1 (Funciona)
//-> Estado Acelerómetro y Giroscopio (MPU):
//-> Estado Magnetómetro:
//-> Estado Módulo microSD:
//-> Estado RTC:
//-> Estado GPS:
//-> Altura Barómetro cada segundo: float (4 bytes)       = 10 - 13
//-> Aceleración x cada segundo: float (4 bytes)          = 14 - 17 
//-> Aceleración y cada segundo: float (4 bytes)          = 18 - 21
//-> Aceleración z cada segundo: float (4 bytes)          = 22 - 25
//-> Altura GPS:  float (4 bytes)                         = 26 - 29
byte addressVuelo = 0, addressBarometro = 4, addressMPU = 1, addressMagnetometro = 2, addressMicroSD = 3, addressRTC = 5, addressGPS = 6;

//--- BIBLIOTECAS ---
#include <avr/wdt.h>     //Libreria para el watchdog
#include <Wire.h>        //Libreria para la comunicacion I2C
#include <HMC5883L.h>    //Libreria para el magnetometro
#include <MPU6050.h>     //Libreria para el MPU
#include <RTClib.h>      //Libreria para el manejo del modulo RTC
#include <SPI.h>         //Libreria interfaz SPI
#include <SD.h>          //Libreria para tarjetas SD
#include <EEPROM.h>      //Libreria que permite guardar valores cuando el arduino se apaga            
//70,63
#include <SoftwareSerial.h>  //Libreria para la comunicacion serial del GPS

//--- DEFINICIONES ---
#define SSpin 10         //Pin Slave Select para el modulo micro SD

//--- VARIABLES ---
//-> Magnetometro
HMC5883L magnetometro;
int16_t mx, my, mz;              //Unidades = 

//-> IMU
MPU6050lib mpu;
int16_t accelCount[3];           // Stores the 16-bit signed accelerometer sensor output
float ax, ay, az;                // Unidades = G's
int16_t gyroCount[3];            // Stores the 16-bit signed gyro sensor output
float gx, gy, gz;                // Unidades = grados por segundo
float gyroBias[3], accelBias[3]; // Bias corrections for gyro and accelerometer
float SelfTest[6];               // Gyro and accelerometer self-test sensor output
float aRes, gRes;                // Scale resolutions per LSB for the sensors

//-> RTC
/*RTC_DS3231 rtc;
unsigned long tiempo1 = 0;       //Tiempos necesarios para determinar cuando a pasado un segundo
unsigned long tiempo2 = 0;       //y así tomar la lectura del reloj
String fecha = "";               //Esta variable concatena el dia/mes/hora:minuto:segundo del reloj  
String dosPuntos = ":";
String diagonal = "/";
*/
//-> Módulo MicroSD
File archivo;                  //Objeto "archivo" del tipo File
byte nFile;                    //Numero de archivo

//-> GPS
/*
SoftwareSerial serial = SoftwareSerial(3,5);    // Connect the GPS RX/TX to arduino pins 3 and 5

const unsigned char UBX_HEADER[] = { 0xB5, 0x62,0x01, 0x02 };
uint8_t Array[4]={0x0,0x0,0x0,0x0};
int pos=0;    //posicion en el Frame

const char UBLOX_INIT[] PROGMEM = {
  // Disable NMEA
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x24, // GxGGA off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x2B, // GxGLL off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x32, // GxGSA off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x39, // GxGSV off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x04,0x40, // GxRMC off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x01,0x05,0x47, // GxVTG off

  // Disable UBX
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0xDC, //NAV-PVT off
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0xB9, //NAV-POSLLH off
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x13,0xC0, //NAV-STATUS off

  // Enable UBX
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x18,0xE1, //NAV-PVT on
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0xBE, //NAV-POSLLH on
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x14,0xC5, //NAV-STATUS on

  // Rate
  //0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12, //(10Hz)
  0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, //(5Hz)
  //0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39, //(1Hz)
};*/
