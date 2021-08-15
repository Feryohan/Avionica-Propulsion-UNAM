//                                         --- CONEXIONES ---
//   Magnetometro GY-271      IMU GY-521     Modulo MicroSD        RTC            GPS               BMP180
//        VCC - 5V             VCC - 5V        GND - GND        SCL - A5       VCC - 5V            Vin - 5V
//        GND - GND            GND - GND       VCC - 5V         SDA - A4        RX - D3 TX         GND - GND
//        SCL - A5             SCL - A5       MISO - D12        VCC - 5V        TX - D5 RX         SCL - A5
//        SDA - A4             SDA - A4       MOSI - D11        GND - GND      GND - GND           SDA - A4
//                                             SCK - D13
//                                              CS - D10

//                                  --- Fuentes y Documentación ---
//-> Repositorio de Github: https://github.com/Feryohan/Avionica-Propulsion-UNAM
      //Nota: En la dirección Segundo Cohete/Programa ARDUINO se encuentra el programa de Arduino con todas las librerías que se han utilizado
//-> Libreria del MPU: https://github.com/kriswiner/MPU6050/tree/master/MPU6050Library
//-> Register Map del MPU6050: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf

//                                       --- Uso de Memoria ---
//- - - Con: MicroSD, GPS, MPU, Magnetometro
// -> Almacenamiento: 87% (23634 bytes)
// -> Dinamica: 73% (1513 bytes)
//- - - GPS
// -> Almacenamiento: 17% (5222 bytes)
// -> Dinamica: 11% (225 bytes)
//- - - MPU
// -> Almacenamiento: 20% (6144 bytes)
// -> Dinamica: 5% (102 bytes)
//- - - MicroSD
// -> Almacenamiento: 44% (13516 bytes)
// -> Dinamica: 55% (1126 bytes)

//                                    --- Direcciones de los sensores ---
//-> MPU6050   - 0x69 (Con una alimentación de 3.3 V en el pin AD0)
//-> BMP180    - 0x77

//                                  --- Librerías ---
#include <avr/wdt.h>     //Libreria para el watchdog
#include <Wire.h>        //Libreria para la comunicacion I2C
#include <SFE_BMP180.h>  //Libreria para el Barometro 180
//#include <NMEAGPS.h>     //Libreria para el GPS
//#include <GPSport.h>     //Libreria para el GPS
#include <SPI.h>         //Libreria interfaz SPI
#include <SD.h>          //Libreria para tarjetas SD
#include <EEPROM.h>      //Libreria que permite guardar valores cuando el arduino se apaga            
#include <RTClib.h>    //Libreria para el manejo del modulo RTC


//                              --- Direcciones de la memoria EEPROM ---
//   0 -> Estado de vuelo: E (69), A (65), D (68)
      byte estadoVuelo = 0;
//   1 -> Estado Acelerometro y Giroscopio (MPU6050): 0 (Fallo MPU), 1 (Inicio MPU)
      byte estadoMPU = 1;
//   2 -> Datos MPU6050: 0 (Fallo funcion obtenerDatosMPU, no se pueden leer los datos), 1 (Datos MPU accesibles)
      byte datosMPU = 2;
//   3 -> Estado RTC: 0 (Fallo RTC), 1 (Inicio RTC)
      byte estadoRTC = 3;
//   4 -> Datos RTC: 0 (Fallo funcion obtenerDatosRTC, no se pueden leer los datos), 1 (Datos RTC accesibles)
      byte datosRTC = 4;
//   5 -> Estado Modulo microSD:  0 (Fallo Modulo microSD), 1 (Inicio Modulo microSD)
      byte estadoModuloSD = 5;
//   6 -> Estado Archivo microSD: 0 (Fallo funcion iniciarArchivo, no se puede acceder a la SD), 1 (Se creo el archivo en la SD)
      byte archivoMemoriaSD = 6;
//   7 -> Registro de datos en archivo microSD: 0 (Fallo el acceso al archivo SD durante el loop), 1 (Se accede al archivo SD sin problemas)
      byte registroDatosSD = 7;
//   8 -> Estado Barometro (BMP180): 0 (Fallo BMP180), 1 (Inicio BMP180)
      byte estadoBMP = 8;
//   9 -> Datos BMP180: 0 (Fallo funcion obtenerDatosBMP180, no se pueden leer los datos), 1 (Datos BMP180 accesibles)
      byte datosBMP = 9;
//   10 -> Estado GPS: 0 (Fallo GPS), 1 (Inicio GPS)
      byte estadoGPS = 10;
//   11 -> Datos GPS: 0 (Fallo funcion obtenerDatosGPS, no se pueden leer los datos), 1 (Datos GPS accesibles)
      byte datosGPS = 11;
//   20 -> Seguro: 0 (desconectado), 1 (conectado)
     byte direccSeguro = 20;     

//   80 -> Direccion de archivo: De esta dirección se secan los numeros para los archivos "Datos#.txt"
     byte direccFile = 80;       
//   150 -> Direccion de AltTerr_EEPROM
     byte direcc_AltTerr_EEPROM = 150;
//   250 -> Direccion de Altura_Maxima_EEPROM
     byte direcc_Alt_Max_EEPROM = 250;           
//   350 -> Direccion de Acc_const_XYZ;
     byte direcc_Acc_const_XYZ = 350;
//   450 -> Direccion de Tiempo_Ascenso_EEPROM;
     byte direcc_Tiempo_Ascenso_EEPROM = 450;     

//                                  --- Definiciones ---
#define SSpin 10         //Pin Slave Select para el modulo micro SD

//                          ----------- Ecuacion de Vuelo ------------
//Considerando el uso de:
//- Barometro
//- Acelerometro
//- RTC

//(Altura_Maxima_EEPROM>>Altura_Actual_RAM>>AltTerr_EEPROM)*(Seguro Desconectado) + 
//(Acc_const_XYZ>>1)*(Tiempo_Ascenso_EEPROM>>TIMER) + 
//(Acc_const_XYZ<<1)*(Seguro Desconectado)*(Altura_Actual_RAM>>AltTerr_EEPROM)

float Altura_Actual_RAM;
float Altura_Maxima_EEPROM;
float AltTerr_EEPROM = 2300;         //metros sobre el nivel del mar en el lugar que nos encontremos
boolean Seguro = true;               //True = Conectado; False = Desconectado
float Acc_const_XYZ;               
byte Tiempo_Ascenso_EEPROM = 8;     //Tiempo estimado (en segundos) para alcanzar el apogeo una vez que el cohete ha despegado

//                                  --- Variables ---
//--> MPU <--
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

//--> RTC <--
RTC_DS3231 rtc;
unsigned long tiempo1 = 0;       //Tiempos necesarios para determinar cuando a pasado un segundo
unsigned long tiempo2 = 0;       //y así tomar la lectura del reloj
String fecha = "";               //Esta variable concatena el dia/mes/hora:minuto:segundo del reloj  
String dosPuntos = ":";
String diagonal = "/";


//--> Módulo MicroSD <--
File archivo;                  //Objeto "archivo" del tipo File
byte nFile;                    //Numero de archivo

//--> BMP180 <--
SFE_BMP180 bmp180;
int status;
double T,P;
float Po = 101325;             //Presión al nivel del mar [Pa]
float M = 0.02896;             //Masa molar del aire [kg/mol]
float R = 8.3143;              //Constante universal de los gases [(N*m)/(mol*K)]
float g = 9.807;               //Aceleración gravitacional [m/s^2]


//--> GPS <--
/*NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values
double altitudDato;
double longitudDato;
double latitudDato;
*/
