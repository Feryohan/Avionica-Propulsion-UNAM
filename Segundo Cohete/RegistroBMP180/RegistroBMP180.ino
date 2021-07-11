#include <EEPROM.h>
#include <SD.h>
#include <SFE_BMP180.h>
#include <Wire.h>
#define LEDR 3
#define chipSelect 4

SFE_BMP180 bmp180;
File Datos;
int status;
double T,P;
int i,nFile;
int t0;

void setup()
{
  Serial.begin(9600);
  if (!bmp180.begin())
  {
    Serial.println("Error en sensor de presión");
    while(1); // bucle infinito
  }
  
  if (!SD.begin(chipSelect)) {
    Serial.println("Error en memoria SD");
    while(1);
  }
  nFile = EEPROM.read(0);
  EEPROM.write(0,nFile+1);//Tenemos un maximo de 255 archivos
  nFile = EEPROM.read(0);
  Datos = SD.open("Prueba" + String(nFile) + ".csv",FILE_WRITE);
  
  if (Datos) {
    i = 0;
    Datos.println("Numero,Tiempo_ms,Temperatura_C,Presion_Pa");
    Datos.close();
    digitalWrite(LEDR,LOW);
    Serial.println("Listo archivo!");  
  } 
  else 
  {
    while(1);
  }
  t0 = millis();
}

void loop()
{
  status = bmp180.startTemperature();
  if (status != 0)
  {   
    Serial.print("Delay temperatura: "); 
    Serial.println(status); 
    delay(status);
    
    status = bmp180.getTemperature(T);
     
    if (status != 0)
    {
      status = bmp180.startPressure(3);
      if (status != 0)
      { 
        Serial.print("Delay Presion: "); 
        Serial.println(status);        
        delay(status);       
        status = bmp180.getPressure(P,T); 
        P = P*100;
        if (status != 0)
        {                  
          Datos = SD.open("Prueba" + String(nFile) + ".csv",FILE_WRITE);
            
          if(Datos)
          {
              i++;
              Datos.print(i);
              Datos.print(",");
              Datos.print(millis());
              Datos.print(",");
              Datos.print(T);
              Datos.print(",");
              Datos.println(P);
              Datos.close();
          }
          else
          {
            Serial.println("ERROR ------- X"); 
          }
                  
        }      
      }      
    }   
  } 
  if(millis()-t0 >= 59000)
  {
    digitalWrite(LEDR, HIGH);
    Serial.println("Fin de la ejecución");
    while(1){}
  }
}
