/*
  Capitulo 37 de Arduino desde cero en Español.
  Mediante un modulo lector de memoria MicroSD conectado mediante interfaz SPI
        crearemos un archivo de texto de prueba y grabaremos una linea en el mismo, 
  luego haremos la apertura para su lectura y asi verificar el correcto funcionamiemto.

  Autor: bitwiseAr  

*/

//Conexiones
//   GND ---> GND
//    5V ---> Vcc
//   D12 ---> MISO  
//   D11 ---> MOSI  
//   D13 ---> SCK
//   D10 ---> CS  

#include <SPI.h>    // incluye libreria interfaz SPI
#include <SD.h>     // incluye libreria para tarjetas SD

#define SSpin 10    // Slave Select en pin digital 10

//El slave slect puede definicirse en cualquier otro PIN DIGITAL, pero con un solo 
//esclavo, se aconseja usar el pin 10

//Objeto que forma parte de la librería SD
File archivo;     // objeto archivo del tipo File

void setup() {
  Serial.begin(9600);       // inicializa monitor serie a 9600 bps
  Serial.println("Inicializando tarjeta ...");  // texto en ventana de monitor
  //el signo ! sirve para invertir el valor que SD.begin() regresa
  if (!SD.begin(SSpin)) {     // inicializacion de tarjeta SD
    Serial.println("fallo en inicializacion !");// si falla se muestra texto correspondiente y
    return;         // se sale del setup() para finalizar el programa
  }
  
  Serial.println("inicializacion correcta");  // texto de inicializacion correcta
  archivo = SD.open("prueba.txt", FILE_WRITE);  // apertura para lectura/escritura de archivo prueba.txt

  //si el archivo no existe, FILE_WRITE lo crea
  //en caso de que se pueda abrir el archivo, SD.open() devuelve true
  
  if (archivo) {
    archivo.println("Probando 1, 2, 3");  // escritura de una linea de texto en archivo
    Serial.println("Escribiendo en archivo prueba.txt..."); // texto en monitor serie
    archivo.close();        // cierre del archivo
    Serial.println("escritura correcta"); // texto de escritura correcta en monitor serie
  } else {
    Serial.println("error en apertura de prueba.txt");  // texto de falla en apertura de archivo
  }

  archivo = SD.open("prueba.txt");    // apertura de archivo prueba.txt
  if (archivo) {
    Serial.println("Contenido de prueba.txt:"); // texto en monitor serie
    while (archivo.available()) {   // mientras exista contenido en el archivo
      Serial.write(archivo.read());     // lectura de a un caracter por vez
    }
    archivo.close();        // cierre de archivo
  } else {
    Serial.println("error en la apertura de prueba.txt");// texto de falla en apertura de archivo
  }
}

void loop() {     // funcion loop() obligatoria de declarar pero no utilizada
  // nada por aqui
}
