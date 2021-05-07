#include <NMEAGPS.h>
#include <GPSport.h>
#include <SD.h>          //Libreria para tarjetas SD

NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values
#define SSpin 10                    //Pin Slave Select para el modulo micro SD
int Buzzer = 6;                //Pin de salida para el buzzer
File archivo;                  //Objeto "archivo" del tipo File

void setup()
{
  pinMode(Buzzer,OUTPUT);           //Buzzer es salida
  digitalWrite(Buzzer, LOW);        //Buzzer apagado

   if (!SD.begin(SSpin))                    //Si la tarjeta SD no inicia correctamente  
   {     
    while(1);                                //El ciclo setup termina
   }
  analogWrite(Buzzer, 200);
  delay(1000);
  analogWrite(Buzzer,LOW);
  delay(3000);
  
  DEBUG_PORT.begin(38400);
  while (!Serial)
    ;
  DEBUG_PORT.print( F("NMEAsimple.INO: started\n") );

  gpsPort.begin(9600);
}

//--------------------------

void loop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();

    DEBUG_PORT.print( F("Location: ") );
    if (fix.valid.location) {
      DEBUG_PORT.print( fix.latitude(), 6 );
      DEBUG_PORT.print( ',' );
      DEBUG_PORT.print( fix.longitude(), 6 );
    }

    DEBUG_PORT.print( F(", Altitude: ") );
    if (fix.valid.altitude)
      DEBUG_PORT.print( fix.altitude() );

    DEBUG_PORT.println();

    archivo = SD.open("GPS.txt",FILE_WRITE);
      if(archivo){
      archivo.print(fix.latitude(),6);
      archivo.print(",");
      archivo.print(fix.longitude(),6);
      archivo.print(",");
      archivo.println(fix.altitude());
      archivo.close();
      }
  }
}
