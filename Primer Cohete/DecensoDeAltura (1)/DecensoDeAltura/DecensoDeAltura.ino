// BIBLIOTECAS
#include <EEPROM.h>
#include <SD.h>
#include <SFE_BMP180.h>
#include <Wire.h>

// DEFINICIONES
#define LEDR 3      //Led indicador
#define chipSelect 10//Pin de seleccion para la SD

// VARIABLES GLOBALES
SFE_BMP180 bmp180;  //Sensor de presion
File Datos;         //Archivo de escritura
int status;         //Estado y delays del sensor de presion
double T,P;         //Variables de temperatura y presion
float h, h_ante;    //Variables de altura y altura anterior
float prom;         //Promedio de presion
byte nFile;         //Numero de archivo 
word n;             //Numero de datos registrados
word t0;            //Tiempo al terminar de configurar
word t_ante;        //Tiempo de la altura anterior

int contador = 0;
