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
int LEDamarillo = 4;
int numeroDatos = 1;

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
//Alturas para calcular la media móvil

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

//================================================================================================================================================================================================
//========================================================================================= ADXL345 ==============================================================================================
//================================================================================================================================================================================================
//--- Accelerometer Register Addresses
#define Power_Register 0x2D
#define X_Axis_Register_DATAX0 0x32 // Hexadecima address for the DATAX0 internal register.
#define X_Axis_Register_DATAX1 0x33 // Hexadecima address for the DATAX1 internal register.
#define Y_Axis_Register_DATAY0 0x34 
#define Y_Axis_Register_DATAY1 0x35
#define Z_Axis_Register_DATAZ0 0x36
#define Z_Axis_Register_DATAZ1 0x37
int ADXAddress = 0x53;  //Device address in which is also included the 8th bit for selecting the mode, read in this case.
int X0,X1,X_out;
int Y0,Y1,Y_out;
int Z1,Z0,Z_out;
float Xa,Ya,Za;

//================================================================================================================================================================================================
//========================================================================================= L3G4200D ==============================================================================================
//================================================================================================================================================================================================
//--- Gyro Register Addresses
#define Gyro_gX0 0x28  
#define Gyro_gX1 0x29
#define Gyro_gY0 0x2A
#define Gyro_gY1 0x2B
#define Gyro_gZ0 0x2C  
#define Gyro_gZ1 0x2D
int Gyro = 0x69; //Device address in which is also included the 8th bit for selecting the mode, read in this case.
int gX0, gX1, gX_out;
int gY0, gY1, gY_out;
int gZ0, gZ1, gZ_out;
float Xg,Yg,Zg;
float angleX,angleY,angleZ,angleXc,angleYc,angleZc;
unsigned long start, finished, elapsed;
float dt=0.015;

//================================================================================================================================================================================================
//========================================================================================= MC5883L ==============================================================================================
//================================================================================================================================================================================================
#define Magnetometer_mX0 0x03  
#define Magnetometer_mX1 0x04  
#define Magnetometer_mZ0 0x05  
#define Magnetometer_mZ1 0x06  
#define Magnetometer_mY0 0x07  
#define Magnetometer_mY1 0x08  
int mX0, mX1, mX_out;
int mY0, mY1, mY_out;
int mZ0, mZ1, mZ_out;
float Xm,Ym,Zm;
#define Magnetometer 0x1E //I2C 7bit address of HMC5883

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(LEDverde, OUTPUT);
  pinMode(LEDrojo, OUTPUT);
  pinMode(LEDamarillo, OUTPUT);
  digitalWrite(LEDverde, LOW);
  digitalWrite(LEDrojo, LOW);
  digitalWrite(LEDamarillo, LOW);

  ////////////////////////////////// Comunicaciones //////////////////////////////////////
  //--> BMP085 
  bmp085Calibration();
  //Alturas para el cálculo de la media móvil
  float altura1;
  float altura2;
  float altura3;
  float altura4;
  float altura5;
  float altura6;
  float altura7;
  float altura8;
  float alturaMediaMovil = 0;
  float alturaPendiente = 0;

  //--> ADXL345
  Wire.beginTransmission(ADXAddress);
  Wire.write(Power_Register); // Power_CTL Register
  // Enable measurement
  Wire.write(8); // Bit D3 High for measuring enable (0000 1000)
  Wire.endTransmission();

  //--> L3G4200D
  Wire.beginTransmission(Gyro);
  Wire.write(0x20); // CTRL_REG1 - Power Mode
  Wire.write(15);   // Normal mode: 15d - 00001111b   
  Wire.endTransmission();
  
  Wire.beginTransmission(Gyro);
  Wire.write(0x23); // CTRL_REG4 - Sensitivity, Scale Selection
  Wire.write(48);   // 2000dps: 48d - 00110000b
  Wire.endTransmission();

  //--> MC5883L
  Wire.beginTransmission(Magnetometer); 
  Wire.write(0x02); // Select mode register
  Wire.write(0x00); // Continuous measurement mode
  Wire.endTransmission();

  //--> Módulo micro SD
  Serial.println("Inicializando tarjeta ...");  
  if (!SD.begin(SSpin)) {     
    Serial.println("Fallo en inicializacion !");
    return;         
  }
  Serial.println("inicializacion correcta"); 

  SD.remove("datos1.txt"); 

  ///////////////////////// Escritura de los datos en la micro SD /////////////////////////
  archivo = SD.open("datos1.txt", FILE_WRITE);  
  if (archivo) {
    Serial.println("********** Serie de datos **********");

    digitalWrite(LEDrojo,HIGH);
    
    for (int i = 1; i < 51; i++){
      float temperature = bmp085GetTemperature(bmp085ReadUT()); 
      float pressure = bmp085GetPressure(bmp085ReadUP());
      float altura = altitud(pressure, temperature);
      float aceleracionX = ejeXacelerometro();
      float aceleracionY = ejeYacelerometro();
      float aceleracionZ = ejeZacelerometro();
        start=millis();
      float giroscopioX = ejeXgiroscopio();
      float giroscopioY = ejeYgiroscopio();
      float giroscopioZ = ejeZgiroscopio();
        // Calculating dt
        finished=millis();
        elapsed=finished-start;
        dt=elapsed/1000.0;
        start = elapsed = 0;
      float magnetometroX = ejeXmagnetometro();
      float magnetometroY = ejeYmagnetometro();
      float magnetometroZ = ejeZmagnetometro();
      
      if(numeroDatos == 1){
          altura1 = altura;
      }
      if(numeroDatos == 2){
          altura2 = altura;
      }
      if(numeroDatos == 3){
          altura3 = altura;
      }
      if(numeroDatos == 4){
          altura4 = altura;
      }
      if(numeroDatos == 5){
          altura5 = altura;
      }
      if(numeroDatos == 6){
          altura6 = altura;
      }
      if(numeroDatos == 7){
          altura7 = altura;
      }
      if(numeroDatos == 8){
          altura8 = altura;
      }

      float AMM1 = alturaMediaMovil;
      float AMM2;
      
      if(numeroDatos > 8){
        alturaMediaMovil = round((altura1+altura2+altura3+altura4+altura5+altura6+altura7+altura8)/(8));
        AMM2 = alturaMediaMovil;
        altura1 = altura2;
        altura2 = altura3;
        altura3 = altura4;
        altura4 = altura5;
        altura5 = altura6;
        altura7 = altura8;
        altura8 = altura;
      }
      if(numeroDatos > 9){
        alturaPendiente = AMM2 - AMM1 -1;
      }

      if(alturaPendiente < 0){
        digitalWrite(LEDamarillo, HIGH);
      }
      
      numeroDatos = numeroDatos + 1;
      
      archivo.print(i);
      archivo.print(",Temperatura,");
      archivo.print(temperature);
      archivo.print(",Presion,");
      archivo.print(pressure);
      archivo.print(",AEjeX,");
      archivo.print(aceleracionX);
      archivo.print(",AEjeY,");
      archivo.print(aceleracionY);
      archivo.print(",AEjeZ,");
      archivo.print(aceleracionZ);
      archivo.print(",GEjeX,");
      archivo.print(giroscopioX);
      archivo.print(",GEjeY,");
      archivo.print(giroscopioY);
      archivo.print(",GEjeZ,");
      archivo.print(giroscopioZ);
      archivo.print(",MEjeX,");
      archivo.print(magnetometroX);
      archivo.print(",MEjeY,");
      archivo.print(magnetometroY);
      archivo.print(",MEjeZ,");
      archivo.print(magnetometroZ);
      archivo.print(",Altura,");
      archivo.print(altura);
      archivo.print(",AMM,");
      archivo.print(alturaMediaMovil);
      archivo.print(",Pendiente,");
      archivo.println(alturaPendiente);


      Serial.print(i);
      Serial.print(",Temperatura,");
      Serial.print(temperature);
      Serial.print(",Presion,");
      Serial.print(pressure);
      Serial.print(",AEjeX,");
      Serial.print(aceleracionX);
      Serial.print(",AEjeY,");
      Serial.print(aceleracionY);
      Serial.print(",AEjeZ,");
      Serial.print(aceleracionZ);
      Serial.print(",GEjeX,");
      Serial.print(giroscopioX);
      Serial.print(",GEjeY,");
      Serial.print(giroscopioY);
      Serial.print(",GEjeZ,");
      Serial.print(giroscopioZ);
      Serial.print(",MEjeX,");
      Serial.print(magnetometroX);
      Serial.print(",MEjeY,");
      Serial.print(magnetometroY);
      Serial.print(",MEjeZ,");
      Serial.print(magnetometroZ);
      Serial.print(",Altura,");
      Serial.print(altura);
      Serial.print(",AMM,");
      Serial.print(alturaMediaMovil);
      Serial.print(",Pendiente,");
      Serial.println(alturaPendiente);
    }

    digitalWrite(LEDrojo,LOW);
    digitalWrite(LEDamarillo, LOW);
    archivo.close();       
    digitalWrite(LEDverde, HIGH);
    Serial.println("---> Escritura correcta"); 
    delay(5000);
  } 
  else {
    Serial.println("Error en apertura de datos.txt");  
  }

  digitalWrite(LEDverde, LOW);
 
  ///////////////////////// Escritura de los datos en la micro SD /////////////////////////
  archivo = SD.open("datos1.txt");   
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

//*********************************************************************************************************************************************************************************************
//************************************************************************************* Métodos ADXL345 ****************************************************************************************
//*********************************************************************************************************************************************************************************************

//Cálculo del eje X
float ejeXacelerometro(){
  Wire.beginTransmission(ADXAddress); // Begin transmission to the Sensor 
  //Ask the particular registers for data
  Wire.write(X_Axis_Register_DATAX0);
  Wire.write(X_Axis_Register_DATAX1);  
  Wire.endTransmission(); // Ends the transmission and transmits the data from the two registers
  Wire.requestFrom(ADXAddress,2); // Request the transmitted two bytes from the two registers
  if(Wire.available()<=2) {  // 
    X0 = Wire.read(); // Reads the data from the register
    X1 = Wire.read();
    /* Converting the raw data of the X-Axis into X-Axis Acceleration
     - The output data is Two's complement 
     - X0 as the least significant byte
     - X1 as the most significant byte */ 
    X1=X1<<8;
    X_out =X0+X1;
    Xa=X_out/256.0; // Xa = output value from -1 to +1, Gravity acceleration acting on the X-Axis
  }
  return Xa;
}

//Cálculo del eje Y
float ejeYacelerometro(){
  Wire.beginTransmission(ADXAddress); 
  Wire.write(Y_Axis_Register_DATAY0);
  Wire.write(Y_Axis_Register_DATAY1);  
  Wire.endTransmission(); 
  Wire.requestFrom(ADXAddress,2);
  if(Wire.available()<=2) { 
    Y0 = Wire.read();
    Y1 = Wire.read();
    Y1=Y1<<8;
    Y_out =Y0+Y1;
    Ya=Y_out/256.0;
  }
  return Ya;
}

//Cálculo del eje Z
float ejeZacelerometro(){
  Wire.beginTransmission(ADXAddress); 
  Wire.write(Z_Axis_Register_DATAZ0);
  Wire.write(Z_Axis_Register_DATAZ1);  
  Wire.endTransmission(); 
  Wire.requestFrom(ADXAddress,2);
  if(Wire.available()<=2) { 
    Z0 = Wire.read();
    Z1 = Wire.read();
    Z1=Z1<<8;
    Z_out =Z0+Z1;
    Za=Z_out/256.0;
  }
  return Za;
}

//*********************************************************************************************************************************************************************************************
//************************************************************************************* Métodos L3G4200D ****************************************************************************************
//*********************************************************************************************************************************************************************************************
//Cálculo del eje X
float ejeXgiroscopio(){
  Wire.beginTransmission(Gyro); // transmit to device
  Wire.write(Gyro_gX0);
  Wire.endTransmission();
  Wire.requestFrom(Gyro,1); 
  if(Wire.available()<=1)   
  {
    gX0 = Wire.read();
  }
  Wire.beginTransmission(Gyro); // transmit to device
  Wire.write(Gyro_gX1);
  Wire.endTransmission();
  Wire.requestFrom(Gyro,1); 
  if(Wire.available()<=1)   
  {
    gX1 = Wire.read();
  }
  // Raw Data
  gX1=gX1<<8;
  gX_out =gX0+gX1;
  
  // From the datasheet: 70 mdps/digit
  Xg=gX_out*0.07; // Angular rate
  // Angular_rate * dt = angle
  angleXc = Xg*dt;
  angleX = angleX + angleXc;

  return angleX;
}

//Cálculo del eje Y
float ejeYgiroscopio(){
  Wire.beginTransmission(Gyro); // transmit to device
  Wire.write(Gyro_gY0);
  Wire.endTransmission();
  Wire.requestFrom(Gyro,1); 
  if(Wire.available()<=1)   
  {
    gY0 = Wire.read();
  }
  Wire.beginTransmission(Gyro); // transmit to device
  Wire.write(Gyro_gY1);
  Wire.endTransmission();
  Wire.requestFrom(Gyro,1); 
  if(Wire.available()<=1)   
  {
    gY1 = Wire.read();
  }
  gY1=gY1<<8;
  gY_out =gY0+gY1;
  Yg=gY_out*0.07;
  angleYc = Yg*dt;
  angleY = angleY + angleYc;

  return angleY;
}

//Cálculo del eje Z
float ejeZgiroscopio(){
  Wire.beginTransmission(Gyro); // transmit to device
  Wire.write(Gyro_gZ0);
  Wire.endTransmission();
  Wire.requestFrom(Gyro,1); 
  if(Wire.available()<=1)   
  {
    gZ0 = Wire.read();
  }
  Wire.beginTransmission(Gyro); // transmit to device
  Wire.write(Gyro_gZ1);
  Wire.endTransmission();
  Wire.requestFrom(Gyro,1); 
  if(Wire.available()<=1)   
  {
    gZ1 = Wire.read();
  }
  gZ1=gZ1<<8;
  gZ_out =gZ0+gZ1;
  Zg=gZ_out*0.07;
  angleZc = Zg*dt;
  angleZ = angleZ + angleZc;

  return angleZ;
}

//*********************************************************************************************************************************************************************************************
//************************************************************************************* Métodos MC5883L ****************************************************************************************
//*********************************************************************************************************************************************************************************************
//Cálculo del eje X
float ejeXmagnetometro(){
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mX1);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mX0 = Wire.read();
  }
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mX0);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mX1 = Wire.read();
  }
  mX1=mX1<<8;
  mX_out =mX0+mX1; // Raw data
  // From the datasheet: 0.92 mG/digit
  Xm = mX_out*0.00092; // Gauss unit
  //* Earth magnetic field ranges from 0.25 to 0.65 Gauss, so these are the values that we need to get approximately.

  return Xm;
}

float ejeYmagnetometro(){
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mY1);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mY0 = Wire.read();
  }
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mY0);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mY1 = Wire.read();
  }
  mY1=mY1<<8;
  mY_out =mY0+mY1;
  Ym = mY_out*0.00092;

  return Ym;
}

float ejeZmagnetometro(){
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mZ1);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mZ0 = Wire.read();
  }
  Wire.beginTransmission(Magnetometer); // transmit to device
  Wire.write(Magnetometer_mZ0);
  Wire.endTransmission();
  Wire.requestFrom(Magnetometer,1); 
  if(Wire.available()<=1)   
  {
    mZ1 = Wire.read();
  }
  mZ1=mZ1<<8;
  mZ_out =mZ0+mZ1;
  Zm = mZ_out*0.00092;

  return Zm;
}
