//Este archivo lee la información del archivo TXT que deseemos abrir

#include <SPI.h>
#include <SD.h>

File myFile;

#define SSpin 10         //Pin Slave Select para el modulo micro SD
/////////////////////////////////////////////////////////////////////////////////////////////////////////
byte nFile = 114;         //¿Que archivo vamos a abrir?//////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  if (!SD.begin(SSpin)) {
    Serial.println("Error: No hay comunicacion con el moduloSD");
    while (1);
  }
  
  myFile = SD.open("Datos"+String(nFile)+".txt");
  if (myFile) {
    Serial.println("Información obtenida del archivo: Datos"+String(nFile)+".txt");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.println("Error: No se puede acceder al archivo Datos"+String(nFile)+".txt");
  }
}

void loop() {
  // nothing happens after setup
}
