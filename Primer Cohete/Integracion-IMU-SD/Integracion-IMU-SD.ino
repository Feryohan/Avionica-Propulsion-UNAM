//Conexiones Módulo MicroSD
//   GND ---> GND
//    5V ---> Vcc
//   D12 ---> MISO  
//   D11 ---> MOSI  
//   D13 ---> SCK
//   D10 ---> CS  

//Conexiones IMU: GY-80
//  3.3V ---> Vcc
//   GND ---> GND
//    A5 ---> SCL
//    A4 ---> SDA

int LEDverde = 2;
int LEDrojo = 3;

//================================================================================================================================================================================================
//======================================================================================== BMP085 ==============================================================================================
//================================================================================================================================================================================================

#include <Wire.h> //Libreria
#define BMP085_ADDRESS 0x77 //Dirección I2C del BMP085
const unsigned char OSS = 0; //Configuración de sobremuestreo
//Valores de calibración
int ac1;
int ac2;
int ac3;
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1;
int b2;
int mb;
int mc; 
int md;
//Valores para la formula barométrica
float Po = 101325; //Presión al nivel del mar [Pa]
float M = 0.02896; //Masa molar del aire [kg/mol]
float T; //Temperatura en grados kelvin
float R = 8.3143; //Constante universal de los gases [(N*m)/(mol*K)]
float g = 9.807; //Aceleración gravitacional [m/s^2]
//b5 es calculada en bmp085GetTemperature(...), esta variable se utiliza también en 
//bmp085GetPressure(...) por lo tanto ...Temperature(...) deberá llamarse antes
//de ...Pressure(...)
long b5;

//================================================================================================================================================================================================
//===================================================================================== Módulo Micro SD ========================================================================================
//================================================================================================================================================================================================

#include <SPI.h>    // incluye libreria interfaz SPI
#include <SD.h>     // incluye libreria para tarjetas SD
#define SSpin 10    // Slave Select en pin digital 10
File archivo;     // objeto archivo del tipo File

void setup() {
  Serial.begin(9600);
  Wire.begin();
  bmp085Calibration();

  pinMode(LEDverde, OUTPUT);
  pinMode(LEDrojo, OUTPUT);
  digitalWrite(LEDverde, LOW);
  digitalWrite(LEDrojo, LOW);

  //Conexión con el módulo micro SD
  Serial.println("Inicializando tarjeta ...");  
  if (!SD.begin(SSpin)) {     
    Serial.println("Fallo en inicializacion !");
    return;         
  }
  Serial.println("inicializacion correcta"); 

  //Escritura de los datos en la micro SD
  archivo = SD.open("datos.txt", FILE_WRITE);  
  if (archivo) {
    Serial.println("********** Serie de datos **********");

    digitalWrite(LEDrojo,HIGH);
    
    for (int i = 1; i < 31; i++){
      float temperature = bmp085GetTemperature(bmp085ReadUT()); 
      float pressure = bmp085GetPressure(bmp085ReadUP());
      float altura = altitud(pressure, temperature);
  
      archivo.print(i);
      archivo.print(",");
      archivo.print(temperature);
      archivo.print(",");
      archivo.print(pressure);
      archivo.print(",");
      archivo.println(altura);

      Serial.print(i);
      Serial.print(",");
      Serial.print(temperature);
      Serial.print(",");
      Serial.print(pressure);
      Serial.print(",");
      Serial.println(altura);

      //Tarda 1 segundo
      delay(1000);
    }

    digitalWrite(LEDrojo,LOW);
    archivo.close();       
    digitalWrite(LEDverde, HIGH);
    Serial.println("---> Escritura correcta"); 
    delay(5000);
  } 
  else {
    Serial.println("Error en apertura de datos.txt");  
  }

  digitalWrite(LEDverde, LOW);
 
  //Lectura de los datos de la micro SD
  archivo = SD.open("datos.txt");   
  if (archivo) {
    Serial.println("---> Datos contenidos en Datos.txt:"); 
    while (archivo.available()) {   
      Serial.write(archivo.read());    
    }
    archivo.close();    
  } 
  else {
    Serial.println("error en la apertura de prueba.txt");
  }
}

void loop() {   
  //Nada por aquí
  
}

//*********************************************************************************************************************************************************************************************
//************************************************************************************* Métodos BMP085 ****************************************************************************************
//*********************************************************************************************************************************************************************************************

//Almacena todos los valores de calibración del BMP085 en variables globales
//Valores de calibración son requeridos para calcular temperatura y presión
//Esta función deberá ser llamada en el inicio del programa
void bmp085Calibration(){
  ac1 = bmp085ReadInt(0xAA);
  ac2 = bmp085ReadInt(0xAC);
  ac3 = bmp085ReadInt(0xAE);
  ac4 = bmp085ReadInt(0xB0);
  ac5 = bmp085ReadInt(0xB2);
  ac6 = bmp085ReadInt(0xB4);
  b1 = bmp085ReadInt(0xB6);
  b2 = bmp085ReadInt(0xB8);
  mb = bmp085ReadInt(0xBA);
  mc = bmp085ReadInt(0xBC);
  md = bmp085ReadInt(0xBE);
}

//Calcular temperatura en [ºC]
float bmp085GetTemperature(unsigned int ut){
  long x1,x2;
  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1+md);
  b5 = x1 + x2;
  float temp = ((b5 + 8)>>4);
  temp = temp / 10;

   return temp;
}

//Calcular presión en [Pa]
//Los valores de calibración deben conocerse
//b5 también se necesita, por lo que bmp085GetTemperature(...) debe ser llamada primero
long bmp085GetPressure(unsigned long up){
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;

  b6 = b5 - 4000;
  //Calcular B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

  //Calcular B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

  //Calcular B7
  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if(b7 < 0x80000000) 
    p = (b7<<1)/b4;
  else 
    p = (b7/b4)<<1;
    
  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;

  long temp = p;
  return temp;
}

//Calcular altitud en [m]
float altitud(float pressure, float temperature){
  T = temperature + 273.15;
  float altitud = (log(pressure/Po))*((R*T)/(M*g))*-1;

  return altitud;
}

//Lee 1 byte desde el BMP085 en "address"
char bmp085Read(unsigned char address){
  unsigned char data;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available())
    ;
    return Wire.read();
}

//Lee 2 bytes desde el BMP085
//El primer byte será desde "address"
//El segundo byte será desde "address"+1
int bmp085ReadInt(unsigned char address){
  unsigned char msb, lsb;

  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();

  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2) 
    ; 
  msb = Wire.read(); 
  lsb = Wire.read(); 
  
  return (int) msb<<8 | lsb;
}

//Lee el valor de temperatura descompensada unsigned
int bmp085ReadUT(){
  unsigned int ut; 

  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  
  // Wait at least 4.5ms
  delay(5);
  
  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

// Lee el valor de presión descompensada 
unsigned long bmp085ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;
  
  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();
  
  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));
  
  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);
  
  // Wait for data to become available
  while(Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();
  
  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
  
  return up;
}
