#include <SFE_BMP180.h>
#include <Wire.h>
#define chipSelect 10

SFE_BMP180 bmp180;
int status;
double T,P;
float Po = 101325;             //Presión al nivel del mar [Pa]
float M = 0.02896;             //Masa molar del aire [kg/mol]
float TempK;                   //Temperatura en grados kelvin
float R = 8.3143;              //Constante universal de los gases [(N*m)/(mol*K)]
float g = 9.807;               //Aceleración gravitacional [m/s^2]
float altura;
double P0 = 77612;


void setup()
{
  Serial.begin(9600);
  if (!bmp180.begin())
  {
    Serial.println("Error en sensor de presión");
    while(1); // bucle infinito
  }
  Serial.println("Continuamos");
  
  Serial.println("Temperatura_C,Presion_Pa,Altura");
}

void loop()
{
  status = bmp180.startTemperature();
  if (status != 0)
  {   
    delay(status);
    status = bmp180.getTemperature(T);
     
    if (status != 0)
    {
      status = bmp180.startPressure(3);
      if (status != 0)
      {     
        delay(status);       
        status = bmp180.getPressure(P,T); 
        P = P*100;
        if (status != 0)
        {                 
              Serial.print(T);
              Serial.print(",");
              Serial.print(P);
              Serial.print(",");
              Serial.println(bmp180.altitude(P,P0));         
        }      
      }      
    }   
  } 
}
