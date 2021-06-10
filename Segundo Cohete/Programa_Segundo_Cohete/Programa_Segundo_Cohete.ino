//                                    --- CONEXIONES ---
//   Magnetometro GY-271      IMU GY-521     Modulo MicroSD        RTC            GPS
//        VCC - 5V             VCC - 5V        GND - GND        SCL - A5       VCC - 5V
//        GND - GND            GND - GND       VCC - 5V         SDA - A4        RX - D3 TX
//        SCL - A5             SCL - A5       MISO - D12        VCC - 5V        TX - D4 RX
//        SDA - A4             SDA - A4       MOSI - D11        GND - GND      GND - GND
//                                             SCK - D13
//                                              CS - D10

//Se obtienen los valores raw de ambos SENSORES con el uso de las librerias que se encuentran en las siguientes páginas:
//-> https://www.luisllamas.es/brujula-magnetica-con-arduino-compass-digital-hmc5883/
//-> https://github.com/kriswiner/MPU6050/tree/master/MPU6050Library
//-> El RTC envía información cada vez que transcurre un segundo

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
byte addressVuelo = 0, addressBarometro = 1, addressMPU = 2, addressMagnetometro = 3, addressMicroSD = 4, addressRTC = 5, addressGPS = 6;

//--- BIBLIOTECAS ---
#include <Wire.h>        //Libreria para la comunicacion I2C
#include <HMC5883L.h>    //Libreria para el magnetometro
#include <MPU6050.h>     //Libreria para el MPU
//#include <RTClib.h>      //Libreria para el manejo del modulo RTC
#include <SPI.h>         //Libreria interfaz SPI
#include <SD.h>          //Libreria para tarjetas SD
#include <EEPROM.h>      //Libreria que permite guardar valores cuando el arduino se apaga            
#include <NMEAGPS.h>     //Libreria NEOGPS requerida para el uso del GPS
#include <GPSport.h>     //Dentro del archivo se utiliza la libreria NeoSWSerial.h, que solo trabaja con 9600, 19200 o 38400 baud
                         //Pero permite utilizar el pin 4 (RX) y el pin 3 (TX)

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
NMEAGPS gps;
gps_fix fix;
