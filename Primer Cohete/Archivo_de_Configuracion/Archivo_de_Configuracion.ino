//Guarda los valors de configuracion del Barometro
//En un archivo .txt

//Conexiones Módulo MicroSD      Conexiones IMU: GY-80       Conexiones Relevador
//   GND ---> GND                  3.3V ---> Vcc                  D8 ---> IN
//    5V ---> Vcc                   GND ---> GND                 GND ---> GND
//   D12 ---> MISO                   A5 ---> SCL                  5V ---> Vcc
//   D11 ---> MOSI                   A4 ---> SDA
//   D13 ---> SCK
//   D10 ---> CS  

/////////////////////////////////////////BIBLIOTECAS/////////////////////////////////////////
#include <Wire.h>        //Libreria para la comunicacion I2C

/////////////////////////////////////////DEFINICIONES/////////////////////////////////////////
        //--------ADXL345---------
#define ADXL345_ADDRESS 0x53        //Device address in which is also included the 8th bit for selecting the mode, read in this case.
#define Power_Register 0x2D         //Accelerometer Register Addresses
        //--------L3G4200D---------
#define Gyro_L3G4200D_ADDRESS 0x69  //Device address in which is also included the 8th bit for selecting the mode, read in this case.
        //--------MC5883L---------
#define Magnet_MC5883L_ADDRESS 0x1E //I2C 7bit address of HMC5883

/////////////////////////////////////////VARIABLES/////////////////////////////////////////       
        //--------GLOBALES---------
int Buzzer = 6;                //Pin de salida para el buzzer
        //--------BMP085---------
//-->Valores de calibración
    const unsigned char OSS = 0;   //Configuración de sobremuestreo
    int ac1,ac2,ac3;
    unsigned int ac4,ac5,ac6;
    int b1,b2,mb,mc,md;
    long b5;
