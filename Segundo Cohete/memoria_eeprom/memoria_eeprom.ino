#include <EEPROM.h>

//Este programa consulta las direcciones de la memoria EEPROM y permite
//restaurarlas (poniendo un 1 en todas las direcciones)

char entradaUsuario;

// - - Direcciones de la memoria EEPROM - -
//   0 -> Estado de vuelo: E (69), A (65), D (68)
      byte estadoVuelo = 0;
//   1 -> Estado Acelerometro y Giroscopio (MPU6050): 0 (Fallo MPU), 1 (Inicio MPU)
      byte estadoMPU = 1;
//   2 -> Datos MPU6050: 0 (Fallo funcion datosMPU, no se pueden leer los datos), 1 (Datos MPU accesibles)
      byte datosMPU = 2;
//   3 -> Estado RTC: 0 (Fallo RTC), 1 (Inicio RTC)
      byte estadoRTC = 3;
//   4 -> Datos RTC: 0 (Fallo funcion datosRTC, no se pueden leer los datos), 1 (Datos RTC accesibles)
      byte datosRTC = 4;
//   5 -> Estado Modulo microSD:  0 (Fallo Modulo microSD), 1 (Inicio Modulo microSD)
      byte estadoModuloSD = 5;
//   6 -> Estado Archivo microSD: 0 (Fallo funcion iniciarArchivo, no se puede acceder a la SD), 1 (Se creo el archivo en la SD)
      byte archivoMemoriaSD = 6;
//   7 -> Registro de datos en archivo microSD: 0 (Fallo el acceso al archivo SD durante el loop), 1 (Se accede al archivo SD sin problemas)
      byte registroDatosSD = 7;
//   8 -> Estado Barometro (BMP180): 0 (Fallo BMP180), 1 (Inicio BMP180)
      byte estadoBMP = 8;
//   9 -> Datos BMP180: 0 (Fallo funcion datosBMP180, no se pueden leer los datos), 1 (Datos BMP180 accesibles)
      byte datosBMP = 9;
//   10 -> Estado GPS: 0 (Fallo GPS), 1 (Inicio GPS)
      byte estadoGPS = 10;
//   11 -> Datos GPS: 0 (Fallo funcion datosGPS, no se pueden leer los datos), 1 (Datos GPS accesibles)
      byte datosGPS = 11;

void leerMemoria(){
  Serial.println("---- Valores en la memoria EEPROM ----");
  Serial.println();
  Serial.print("Estado Vuelo             ");
  Serial.println(EEPROM.read(estadoVuelo));
  Serial.print("Estado MPU             ");
  Serial.println(EEPROM.read(estadoMPU));
  Serial.print("Datos MPU             ");
  Serial.println(EEPROM.read(datosMPU));
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
  EEPROM.put(datosMPU,1);
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
      entradaUsuario = Serial.read();
      if(entradaUsuario == 'Y' || entradaUsuario == 'y'){
        restaurarMemoria();
        Serial.println("La memoria se ha restaurado");
        leerMemoria();
      }
      if(entradaUsuario == 'N' || entradaUsuario == 'n'){
        Serial.println("Los valores no se han modificado");
      }
      else{
        Serial.println("Boton incorrecto");
      }
  }
}
