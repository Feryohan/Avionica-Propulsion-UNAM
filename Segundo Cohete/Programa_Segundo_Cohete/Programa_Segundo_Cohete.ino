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

//                     ----------- Condición para estar en Ascenso ------------
//((Alt_Actual_RAM >> AltTerr_EEPROM)&&(estadoSeguro)) ||
//((estadoSeguro)&&(AccXYZ_Max>>1)) ||
//((AccXYZ_Max>>1)&&(Alt_Actual_RAM >> AltTerr_EEPROM))

//                          ----------- Ecuacion de Vuelo ------------
//(Altura_Maxima_EEPROM>>Altura_Actual_RAM>>AltTerr_EEPROM) || 
//(contadorTiempo>>Tiempo_Ascenso_EEPROM) ||
//((AccXYZ_Min<<1)&&(Altura_Maxima_EEPROM>>Altura_Actual_RAM))

//                                    --- Direcciones de los sensores ---
//-> MPU6050   - 0x69 (Con una alimentación de 3.3 V en el pin AD0)
//-> BMP180    - 0x77

//                                  --- Librerías ---
#include <avr/wdt.h>     //Libreria para el watchdog
#include <Wire.h>        //Libreria para la comunicacion I2C
#include <SFE_BMP180.h>  //Libreria para el Barometro 180
#include <SPI.h>         //Libreria interfaz SPI
#include <SD.h>          //Libreria para tarjetas SD
#include <EEPROM.h>      //Libreria que permite guardar valores cuando el arduino se apaga            
#include <RTClib.h>      //Libreria para el manejo del modulo RTC


//                              --- Direcciones de la memoria EEPROM ---
//   0 -> Estado de vuelo: 0 (Espera), 1 (Ascenso), 2 (Descenso)
      byte estadoVuelo = 0;
//   1 -> Estado Acelerometro (MPU6050): 0 (Fallo MPU), 1 (Inicio MPU)
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
//   20 -> Seguro: 0 (desconectado), 1 (conectado)
     byte direccSeguro = 20;     

//   80 -> Direccion de archivo: De esta dirección se sacan los numeros para los archivos "Datos#.txt"
     byte direccFile = 80;       
//   150 -> Direccion para guardar la AltTerr_EEPROM
     float direcc_AltTerr_EEPROM = 150;
//   250 -> Direccion para guardar la Altura_Maxima_EEPROM
     float direcc_Alt_Max_EEPROM = 250;
//   350 -> Direccion para guardar el Tiempo_Ascenso_EEPROM;
     float direcc_Tiempo_Ascenso_EEPROM = 350;     

//                                  --- Variables ---
#define SSpin 10                     //Pin Slave Select para el modulo micro SD
//--> Para la ecuacion de vuelo
float Altura_Actual_RAM;             //Es la que se va guardando en la micro SD
float Altura_Anterior_RAM;
float Altura_Maxima_EEPROM;
float AltTerr_EEPROM;
float Altura_No_Filtrada;
//float Altura_Maxima_EEPROM;          //Se guarda cada segundo en la dirección: direcc_Alt_Max_EEPROM
byte  estadoSeguro;                  //0 (desconectado), 1 (conectado)
#define   numeroMediaMovil 8            //Numero de lecturas que consideramos para el cálculo de las medias móviles
int numeroDeLectura = 1;
long arregloAccXYZ[numeroMediaMovil];   //Arreglo en el que se almacenarán los valores de moduloAccXYZ
long arregloAlturas[numeroMediaMovil];
float moduloAccXYZ;                  //moduloAccXYZ = sqrt( (ax^2) + (ay^2) + (az^2) )
float AccXYZAnterior;                //Resultado de la media movil de la aceleracion un instante antes
float AccXYZ;                        //Resultado de la media movil de la aceleracion en este instante
float AccXYZMax;                     //Aceleración Máxima de media movil
float AccXYZMin;                     //Aceleración Mínima de media movil
byte contadorTiempo;

//                                  --- Variables ---
//--> MPU <--
byte Ascale = 3;
//Para los valores de salida
float Aescala = (16.0 / 32768.0)*1000;
int16_t accelCount[3];           // Stores the 16-bit signed accelerometer sensor output
float ax, ay, az;                // Unidades = G's
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
