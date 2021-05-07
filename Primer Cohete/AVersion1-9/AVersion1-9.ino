//No utiliza la biblioteca SFE_BMP180
//72% de almacenamiento
//72% de memoria dinámica
//Problema con la pendiente
//Magnetometro arreglado

//Conexiones Módulo MicroSD
//   GND ---> GND
//    5V ---> Vcc
//   D12 ---> MISO  
//   D11 ---> MOSI  
//   D13 ---> SCK
//   D10 ---> CS  

//Conexiones IMU: GY-80
//  3.3V ---> Vcc
//   GND ---> GND
//    A5 ---> SCL
//    A4 ---> SDA

//Conexiones Relevador
//    D8 ---> IN
//   GND ---> GND
//    5V ---> Vcc

/////////////////////////////////////////BIBLIOTECAS/////////////////////////////////////////
#include <Wire.h>        //Libreria para la comunicacion I2C
#include <SFE_BMP180.h>  //Libreria para el barometro
#include <SPI.h>         //Libreria interfaz SPI
#include <SD.h>          //Libreria para tarjetas SD
#include <EEPROM.h>      //Libreria que permite guardar valores cuando el arduino se apaga

/////////////////////////////////////////DEFINICIONES/////////////////////////////////////////
        //--------Micro SD---------
#define SSpin 10                    //Pin Slave Select para el modulo micro SD
        //--------BMP085----------
#define BMP085_ADDRESS 0x77         //Dirección I2C del barometro BMP085
        //--------ADXL345---------
#define ADXL345_ADDRESS 0x53        //Device address in which is also included the 8th bit for selecting the mode, read in this case.
#define Power_Register 0x2D         //Accelerometer Register Addresses
#define X_Axis_Register_DATAX0 0x32 //Hexadecima address for the DATAX0 internal register.
#define X_Axis_Register_DATAX1 0x33 //Hexadecima address for the DATAX1 internal register.
#define Y_Axis_Register_DATAY0 0x34 
#define Y_Axis_Register_DATAY1 0x35
#define Z_Axis_Register_DATAZ0 0x36
#define Z_Axis_Register_DATAZ1 0x37
        //--------L3G4200D---------
#define Gyro_L3G4200D_ADDRESS 0x69  //Device address in which is also included the 8th bit for selecting the mode, read in this case.
#define Gyro_gX0 0x28               //Gyro Register Addresses
#define Gyro_gX1 0x29
#define Gyro_gY0 0x2A
#define Gyro_gY1 0x2B
#define Gyro_gZ0 0x2C  
#define Gyro_gZ1 0x2D
        //--------MC5883L---------
#define Magnet_MC5883L_ADDRESS 0x1E //I2C 7bit address of HMC5883
#define Magnetometer_mX0 0x03  
#define Magnetometer_mX1 0x04  
#define Magnetometer_mZ0 0x05  
#define Magnetometer_mZ1 0x06  
#define Magnetometer_mY0 0x07  
#define Magnetometer_mY1 0x08

/////////////////////////////////////////VARIABLES/////////////////////////////////////////       
        //--------GLOBALES---------
int Relay = 8;                 //Pin de salida para el relevador
int Buzzer = 6;                //Pin de salida para el buzzer
int numeroDatos = 1;           //Para obtener el numero de datos que queremos
word t0;                       //Tiempo al terminar de configurar
        //--------BMP085---------
float temperature,pressure,altura;
//-->Alturas para el cálculo de la media móvil
    float altura1,altura2,altura3,altura4,altura5,altura6,altura7,altura8;
    float alturaMediaMovil = 0;
    float alturaPendiente = 0;
//--> Ultimos 2 valores de la altura media movil
    float AMM1 = alturaMediaMovil;
    float AMM2;
//-->Formula barometrica
    float Po = 101325;             //Presión al nivel del mar [Pa]
    float M = 0.02896;             //Masa molar del aire [kg/mol]
    float TempK;                   //Temperatura en grados kelvin
    float R = 8.3143;              //Constante universal de los gases [(N*m)/(mol*K)]
    float g = 9.807;               //Aceleración gravitacional [m/s^2]
//-->Valores de calibración
    const unsigned char OSS = 0;   //Configuración de sobremuestreo
    int ac1,ac2,ac3;
    unsigned int ac4,ac5,ac6;
    int b1,b2,mb,mc,md;
    long b5;
        //--------Modulo Micro SD---------
File archivo;                  //Objeto "archivo" del tipo File
byte nFile;                    //Numero de archivo
        //--------ADXL345---------
int X0,X1,X_out;
int Y0,Y1,Y_out;
int Z1,Z0,Z_out;
float aceleracionX,aceleracionY,aceleracionZ;
//output value from -1 to +1, Gravity acceleration acting on the X-Axis
//2^8 bytes
float AceleracionXBytes = 256;
        //--------L3G4200D---------
int gX0,gX1,gX_out;
int gY0,gY1,gY_out;
int gZ0,gZ1,gZ_out;
float Xg,Yg,Zg;
float angleX,angleY,angleZ,angleXc,angleYc,angleZc;
float giroscopioX,giroscopioY,giroscopioZ;
unsigned long start,finished,elapsed;
// From the datasheet: 70 mdps/digit
float dpsXDigit = 0.07;      //Angular rate [dps]
float dt=0.015;
        //--------MC5883L---------  
int mX0,mX1,mX_out;
int mY0,mY1,mY_out;
int mZ0,mZ1,mZ_out;
float magnetometroX,magnetometroY,magnetometroZ;
// From the datasheet: 0.92 mG/digit
float GaussXDigit = 0.00092;     // Gauss unit [G]
//Earth magnetic field ranges from 0.25 to 0.65 Gauss, so these are the values that we need to get approximately.
//->Arreglos para ajuste      m = (x-b)*A
double matrizMagB[3] = {-0.0389,-0.2945,0.2303};
double matrizMagA[3][3] = {{0.8951,-0.0019,0.0046},{-0.0019,0.9303,-0.0272},{0.0046,-0.0272,1.2017}};
