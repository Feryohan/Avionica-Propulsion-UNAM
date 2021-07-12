/* Este codigo se comunica con el Modulo de GPS de u-blox atrabes de UART 
para conigurar primero el modulo se le mandan comandos sacados del programa de U-center
o igual de la datasheet larga. Se configura a 5Hz y para que unicamente mande el paquete 
POSLLH. El cual tiene cierta estrutura en su trama. Con base a la estrutura del frame obtendremos
unicamente la coordenadas, longitud, latitud, altitud respecto al nivel del mar
aun se puede configurar a 10Hz, se puede obtener el tiempo de la semana, asi com el numero de satelites,
Incertidumbre, etc
*/
#include <SPI.h>         //Libreria interfaz SPI
#include <SD.h>          //Libreria para tarjetas SD

File archivo;                  //Objeto "archivo" del tipo File
byte nFile = 2;                    //Numero de archivo
#define SSpin 10         //Pin Slave Select para el modulo micro SD

#include <SoftwareSerial.h>
// Connect the GPS RX/TX to arduino pins 3 and 5
SoftwareSerial serial = SoftwareSerial(3,5);


const unsigned char UBX_HEADER[] = { 0xB5, 0x62,0x01, 0x02 };
uint8_t Array[4]={0x0,0x0,0x0,0x0};
int pos=0;    //posicion en el Frame

const char UBLOX_INIT[] PROGMEM = {
  // Disable NMEA
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x24, // GxGGA off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x2B, // GxGLL off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x32, // GxGSA off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x39, // GxGSV off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x01,0x04,0x40, // GxRMC off
  0xB5,0x62,0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x01,0x05,0x47, // GxVTG off

  // Disable UBX
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x17,0xDC, //NAV-PVT off
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x12,0xB9, //NAV-POSLLH off
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x13,0xC0, //NAV-STATUS off

  // Enable UBX
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x07,0x00,0x01,0x00,0x00,0x00,0x00,0x18,0xE1, //NAV-PVT on
  0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x13,0xBE, //NAV-POSLLH on
  //0xB5,0x62,0x06,0x01,0x08,0x00,0x01,0x03,0x00,0x01,0x00,0x00,0x00,0x00,0x14,0xC5, //NAV-STATUS on

  // Rate
  //0xB5,0x62,0x06,0x08,0x06,0x00,0x64,0x00,0x01,0x00,0x01,0x00,0x7A,0x12, //(10Hz)
  0xB5,0x62,0x06,0x08,0x06,0x00,0xC8,0x00,0x01,0x00,0x01,0x00,0xDE,0x6A, //(5Hz)
  //0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39, //(1Hz)
};

void datosGPS(){
  
if(serial.available()){
  
  long lon;     //Longitud deg e-7
  long lat;     //latitude deg e-7
  long hMSL;    // altura Nivel del Mar mm
  //Revisar que 
  Array[0]=Array[1];
  Array[1]=Array[2];
  Array[2]=Array[3];
  Array[3]=serial.read();

  //Serial.print(Array[3], HEX);
  //Serial.print(" ");
  //Revisar header de la trama, para indicar que incio el mensaje
  if(Array[0]==UBX_HEADER[0] && Array[1]==UBX_HEADER[1] && Array[2]==UBX_HEADER[2] && Array[3]==UBX_HEADER[3]){
    pos=1;
    lon=0;lat=0;hMSL=0;
  }else{
    pos++;  
          if (pos<=8 && 4<pos){     //Bytes del 5 al 8
      if (pos-4<=1)lon=+Array[3]*16*16*16;
      if (pos-4<=2)lon=+Array[3]*16*16;
      if (pos-4<=3)lon=+Array[3]*16;
      if (pos-4<=4)lon=+Array[3];
    }else if (pos<=12 && 8<pos){  //Bytes del 9 al 12
      if (pos-8<=1)lat=+Array[3]*16*16*16;
      if (pos-8<=2)lat=+Array[3]*16*16;
      if (pos-8<=3)lat=+Array[3]*16;
      if (pos-8<=4)lat=+Array[3];
    }else if (pos<=20 && 16<pos){ //Bytes del 17 al 20
      if (pos-16<=1)hMSL=+Array[3]*16*16*16;
      if (pos-16<=2)hMSL=+Array[3]*16*16;
      if (pos-16<=3)hMSL=+Array[3]*16;
      if (pos-16<=4)hMSL=+Array[3];
    }else if(pos==33){
      serial.println("");
      serial.print("lon=");
      serial.print(lon);
      serial.print(" lat=");
      serial.print(lat);
      serial.print(" hMSL=");
      serial.println(hMSL);
     
    }
  }
   archivo = SD.open("GPS"+String(nFile)+".txt",FILE_WRITE);
    if(archivo){
      archivo.print(lat);
      archivo.print(",");
      archivo.print(lon);
       archivo.print(",");
      archivo.println(hMSL);
    }
    archivo.close();
}
}

void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  serial.begin(9600);
  for(int i = 0; i < sizeof(UBLOX_INIT); i++) {                        
  serial.write( pgm_read_byte(UBLOX_INIT+i) );
  delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
  }

  if(SD.begin(SSpin)){
    archivo = SD.open("GPS"+String(nFile)+".txt",FILE_WRITE);
    if(archivo){
      //Si el archivo se crea correctamente
      archivo.println("Datos GPS");
    }
   archivo.close();
   }
}

void loop() {
  // put your main code here, to run repeatedly:
  datosGPS();
}
