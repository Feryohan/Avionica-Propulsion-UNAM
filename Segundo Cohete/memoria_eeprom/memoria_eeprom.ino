#include <EEPROM.h>

//Este programa coloca las siguientes posiciones de la memoria EEPROM
//en uno 

char letra;

// - - Direcciones de la memoria EEPROM - -
//   0 -> Estado de vuelo: E (69), A (65), D (68)
      byte estadoVuelo = 0;
//   1 -> Estado Acelerometro y Giroscopio (MPU6050): 0 (Fallo MPU), 1 (Inicio MPU)
      byte estadoMPU = 1;
//   2 -> Datos Acelerometro de la MPU6050: 0 (Fallo funcion obtenerDatosAcelerometro, no se pueden leer los datos), 1 (Datos Acelerometro accesibles)
      byte datosAcelerometro = 2;      
//   3 -> Datos Giroscopio de la MPU6050: 0 (Fallo funcion obtenerDatosGiroscopio, no se pueden leer los datos), 1 (Datos Giroscopio accesibles)
      byte datosGiroscopio = 3;
//   4 -> Estado RTC: 0 (Fallo RTC), 1 (Inicio RTC)
      byte estadoRTC = 4;
//   5 -> Datos RTC: 0 (Fallo funcion obtenerDatosRTC, no se pueden leer los datos), 1 (Datos RTC accesibles)
      byte datosRTC = 5;
//   6 -> Estado Modulo microSD:  0 (Fallo Modulo microSD), 1 (Inicio Modulo microSD)
      byte estadoModuloSD = 6;
//   7 -> Estado Archivo microSD: 0 (Fallo funcion iniciarArchivo, no se puede acceder a la SD), 1 (Se creo el archivo en la SD)
      byte archivoMemoriaSD = 7;
//   8 -> Registro de datos en archivo microSD: 0 (Fallo el acceso al archivo SD durante el loop), 1 (Se accede al archivo SD sin problemas)
      byte registroDatosSD = 8;
//   9 -> Estado Barometro (BMP180): 0 (Fallo BMP180), 1 (Inicio BMP180)
      byte estadoBMP = 9;
//   10 -> Datos BMP180: 0 (Fallo funcion datosBMP180, no se pueden leer los datos), 1 (Datos BMP180 accesibles)
      byte datosBMP = 10;
//   11 -> Estado GPS: 0 (Fallo GPS), 1 (Inicio GPS)
      byte estadoGPS = 11;
//   12 -> Datos GPS: 0 (Fallo funcion datosGPS, no se pueden leer los datos), 1 (Datos GPS accesibles)
      byte datosGPS = 12;

void leerMemoria(){
  Serial.println("---- Valores en la memoria EEPROM ----");
  Serial.println();
  Serial.print("Estado Vuelo             ");
  Serial.println(EEPROM.read(estadoVuelo));
  Serial.print("Estado MPU             ");
  Serial.println(EEPROM.read(estadoMPU));
  Serial.print("Datos Acelerometro             ");
  Serial.println(EEPROM.read(datosAcelerometro));
  Serial.print("Datos Giroscopio             ");
  Serial.println(EEPROM.read(datosGiroscopio));
  Serial.print("Estado RTC             ");
  Serial.println(EEPROM.read(estadoRTC));
  Serial.print("Datos RTC             ");
  Serial.println(EEPROM.read(datosRTC));
  Serial.print("Estado Modulo SD             ");
  Serial.println(EEPROM.read(estadoModuloSD));
  Serial.print("Archivo Memoria SD             ");
  Serial.println(EEPROM.read(archivoMemoriaSD));
  Serial.print("Registro Datos SD             ");
  Serial.println(EEPROM.read(registroDatosSD));  
  Serial.print("Estado BMP             ");
  Serial.println(EEPROM.read(estadoBMP));
  Serial.print("Datos BMP             ");
  Serial.println(EEPROM.read(datosBMP));
  Serial.print("Estado GPS             ");
  Serial.println(EEPROM.read(estadoGPS));
  Serial.print("Datos GPS             ");
  Serial.println(EEPROM.read(datosGPS));
  Serial.print("Numero de ultimo archivo             ");
  Serial.println(EEPROM.read(500));
}

void restaurarMemoria(){
  EEPROM.put(estadoVuelo,1);
  EEPROM.put(estadoMPU,1);
  EEPROM.put(datosAcelerometro,1);
  EEPROM.put(datosGiroscopio,1);
  EEPROM.put(estadoRTC,1);
  EEPROM.put(datosRTC,1);
  EEPROM.put(estadoModuloSD,1);
  EEPROM.put(archivoMemoriaSD,1);
  EEPROM.put(registroDatosSD,1);  
  EEPROM.put(estadoBMP,1);
  EEPROM.put(datosBMP,1);
  EEPROM.put(estadoGPS,1);
  EEPROM.put(datosGPS,1);
}

void setup() {
  Serial.begin(9600);
  leerMemoria();
  Serial.println("¿Deseas restaurar la memoria (Y/N)?");
}

void loop() {
  // put your main code here, to run repeatedly:
    if(Serial.available()>0){
      letra = Serial.read();
      if(letra == 'Y' || letra == 'y'){
        restaurarMemoria();
        Serial.println("La memoria se ha restaurado");
        leerMemoria();
      }
      if(letra == 'N' || letra == 'n'){
        Serial.println("Los valores no se han modificado");
      }
      else{
        Serial.println("Boton incorrecto");
      }
  }
}
