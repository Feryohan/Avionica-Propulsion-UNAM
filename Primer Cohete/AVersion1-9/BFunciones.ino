//DECLARACION DE FUNCIONES

//--- Funcion de registro de datos ---
void registroDatos()
{
  archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
  if(archivo){
      archivo.print(numeroDatos);
      archivo.print(",");
      archivo.print(millis());
      archivo.print(",");
      archivo.print(temperature);
      archivo.print(",");
      archivo.print(pressure);
      archivo.print(",");
      archivo.print(aceleracionX);
      archivo.print(",");
      archivo.print(aceleracionY);
      archivo.print(",");
      archivo.print(aceleracionZ);
      archivo.print(",");
      archivo.print(giroscopioX);
      archivo.print(",");
      archivo.print(giroscopioY);
      archivo.print(",");
      archivo.print(giroscopioZ);
      archivo.print(",");
      archivo.print(magnetometroX);
      archivo.print(",");
      archivo.print(magnetometroY);
      archivo.print(",");
      archivo.print(magnetometroZ);
      archivo.print(",");
      archivo.print(altura);
      archivo.print(",");
      archivo.print(alturaMediaMovil);
      archivo.print(",");
      archivo.println(alturaPendiente);

      archivo.close();
  }
}

void calculoMediaMovil(){
      //--> Escritura de las ultimas 8 alturas
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
      
      if(numeroDatos > 8){
        //Promedio de las ultimas 8 alturas
        alturaMediaMovil = round((altura1+altura2+altura3+altura4+altura5+altura6+altura7+altura8)/8);
        AMM2 = alturaMediaMovil;
        //Se van registrando las ultimas 8 alturas que sean calculadas
        altura1 = altura2;
        altura2 = altura3;
        altura3 = altura4;
        altura4 = altura5;
        altura5 = altura6;
        altura6 = altura7;
        altura7 = altura8;
        altura8 = altura;
      }
      //Calculo de la pendiente de las alturas medias moviles
      if(numeroDatos > 9){
        alturaPendiente = (AMM2 - AMM1);
      }
}

//*********************************************************************************************************************************************************************************************
//************************************************************************************* Métodos BMP085 ****************************************************************************************
//*********************************************************************************************************************************************************************************************
void bmp085Calibration(){
  ac1 = bmp085ReadInt(0xAA);          //Almacena todos los valores de calibración del BMP085 en variables globales
  ac2 = bmp085ReadInt(0xAC);          //Valores de calibración son requeridos para calcular temperatura y presión
  ac3 = bmp085ReadInt(0xAE);          //Esta función deberá ser llamada en el inicio del programa
  ac4 = bmp085ReadInt(0xB0);
  ac5 = bmp085ReadInt(0xB2);
  ac6 = bmp085ReadInt(0xB4);
  b1 = bmp085ReadInt(0xB6);
  b2 = bmp085ReadInt(0xB8);
  mb = bmp085ReadInt(0xBA);
  mc = bmp085ReadInt(0xBC);
  md = bmp085ReadInt(0xBE);
}

float bmp085GetTemperature(unsigned int ut){
  long x1,x2;
  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1+md);
  b5 = x1 + x2;                       //Calcula temperatura en [ºC]
  float temp = ((b5 + 8)>>4);
  temp = temp / 10;
  return temp;
}

long bmp085GetPressure(unsigned long up){
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;               //Calcula presión en [Pa]
  b6 = b5 - 4000;                     //Los valores de calibración deben conocerse
  //Calcular B3                       //b5 también se necesita, por lo que bmp085GetTemperature(...) debe ser llamada primero
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

float altitud(float pressure, float temperature){
  TempK = temperature + 273.15;       //Calcular altitud en [m]
  altura = (log(pressure/Po))*((R*TempK)/(M*g))*-1;
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
  Wire.beginTransmission(ADXL345_ADDRESS);       // Begin transmission to the Sensor 
  //Ask the particular registers for data
  Wire.write(X_Axis_Register_DATAX0);
  Wire.write(X_Axis_Register_DATAX1);  
  Wire.endTransmission();                        // Ends the transmission and transmits the data from the two registers
  Wire.requestFrom(ADXL345_ADDRESS,2);           // Request the transmitted two bytes from the two registers
  if(Wire.available()<=2) {   
    X0 = Wire.read();                            //Reads the data from the register
    X1 = Wire.read();
    // Converting the raw data of the X-Axis into X-Axis Acceleration
    // - The output data is Two's complement 
    // - X0 as the least significant byte
    // - X1 as the most significant byte 
    X1=X1<<8;
    X_out= X0 + X1;
    aceleracionX = X_out/AceleracionXBytes;                        // Xa = output value from -1 to +1, Gravity acceleration acting on the X-Axis
  }
}

//Cálculo del eje Y
float ejeYacelerometro(){
  Wire.beginTransmission(ADXL345_ADDRESS); 
  Wire.write(Y_Axis_Register_DATAY0);
  Wire.write(Y_Axis_Register_DATAY1);  
  Wire.endTransmission(); 
  Wire.requestFrom(ADXL345_ADDRESS,2);
  if(Wire.available()<=2) { 
    Y0 = Wire.read();
    Y1 = Wire.read();
    Y1=Y1<<8;
    Y_out= Y0 + Y1;
    aceleracionY = Y_out/AceleracionXBytes;
  }
}

//Cálculo del eje Z
float ejeZacelerometro(){
  Wire.beginTransmission(ADXL345_ADDRESS); 
  Wire.write(Z_Axis_Register_DATAZ0);
  Wire.write(Z_Axis_Register_DATAZ1);  
  Wire.endTransmission(); 
  Wire.requestFrom(ADXL345_ADDRESS,2);
  if(Wire.available()<=2) { 
    Z0 = Wire.read();
    Z1 = Wire.read();
    Z1=Z1<<8;
    Z_out= Z0 + Z1;
    aceleracionZ = Z_out/AceleracionXBytes;
  }
}

//*********************************************************************************************************************************************************************************************
//************************************************************************************* Métodos L3G4200D ****************************************************************************************
//*********************************************************************************************************************************************************************************************
//Cálculo del eje X
float ejeXgiroscopio()
{
Wire.beginTransmission(Gyro_L3G4200D_ADDRESS); // transmit to device
  Wire.write(Gyro_gX0);
  Wire.endTransmission();
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gX0 = Wire.read();
  }
  Wire.beginTransmission(Gyro_L3G4200D_ADDRESS); // transmit to device
  Wire.write(Gyro_gX1);
  Wire.endTransmission();
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gX1 = Wire.read();
  }
  // Raw Data
  gX1=gX1<<8;
  gX_out =gX0+gX1;
  
  // From the datasheet: 70 mdps/digit
  Xg=gX_out*dpsXDigit; // Angular rate
  // Angular_rate * dt = angle
  angleXc = Xg*dt;
  giroscopioX = angleX + angleXc;
}

//Cálculo del eje Y
float ejeYgiroscopio()
{
  Wire.beginTransmission(Gyro_L3G4200D_ADDRESS); // transmit to device
  Wire.write(Gyro_gY0);
  Wire.endTransmission();
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gY0 = Wire.read();
  }
  Wire.beginTransmission(Gyro_L3G4200D_ADDRESS); // transmit to device
  Wire.write(Gyro_gY1);
  Wire.endTransmission();
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gY1 = Wire.read();
  }
  gY1=gY1<<8;
  gY_out =gY0+gY1;
  Yg=gY_out*dpsXDigit;
  angleYc = Yg*dt;
  giroscopioY = angleY + angleYc;
}

//Cálculo del eje Z
float ejeZgiroscopio()
{
  Wire.beginTransmission(Gyro_L3G4200D_ADDRESS); // transmit to device
  Wire.write(Gyro_gZ0);
  Wire.endTransmission();
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gZ0 = Wire.read();
  }
  Wire.beginTransmission(Gyro_L3G4200D_ADDRESS); // transmit to device
  Wire.write(Gyro_gZ1);
  Wire.endTransmission();
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gZ1 = Wire.read();
  }
  gZ1=gZ1<<8;
  gZ_out =gZ0+gZ1;
  Zg=gZ_out*dpsXDigit;
  angleZc = Zg*dt;
  giroscopioZ = angleZ + angleZc;
}

//*********************************************************************************************************************************************************************************************
//************************************************************************************* Métodos MC5883L ****************************************************************************************
//*********************************************************************************************************************************************************************************************
//Cálculo del eje X
float ejeXmagnetometro(){
  Wire.beginTransmission(Magnet_MC5883L_ADDRESS);    // transmit to device
  Wire.write(Magnetometer_mX0);
  Wire.write(Magnetometer_mX1);
  Wire.endTransmission();
  Wire.requestFrom(Magnet_MC5883L_ADDRESS,2); 
  if(Wire.available()<=2)   
  {
    mX0 = Wire.read();
    mX1 = Wire.read();
    mX1= mX1<<8;
    mX_out= mX0 + mX1;                           // Raw data
    // From the datasheet: 0.92 mG/digit
    magnetometroX = mX_out*GaussXDigit;                         // Gauss unit
    //Earth magnetic field ranges from 0.25 to 0.65 Gauss, so these are the values that we need to get approximately.
  }
}

float ejeYmagnetometro(){
  Wire.beginTransmission(Magnet_MC5883L_ADDRESS);    // transmit to device
  Wire.write(Magnetometer_mY0);
  Wire.write(Magnetometer_mY1);
  Wire.endTransmission();
  Wire.requestFrom(Magnet_MC5883L_ADDRESS,2); 
  if(Wire.available()<=2)   
  {
    mY0 = Wire.read();
    mY1 = Wire.read();
    mY1= mY1<<8;
    mY_out= mY0 + mY1; 
    magnetometroY = mY_out*GaussXDigit;  
  }
}

float ejeZmagnetometro(){
  Wire.beginTransmission(Magnet_MC5883L_ADDRESS);    // transmit to device
  Wire.write(Magnetometer_mZ0);
  Wire.write(Magnetometer_mZ1);
  Wire.endTransmission();
  Wire.requestFrom(Magnet_MC5883L_ADDRESS,2); 
  if(Wire.available()<=2)   
  {
    mZ0 = Wire.read();
    mZ1 = Wire.read();
    mZ1= mZ1<<8;
    mZ_out= mZ0 + mZ1; 
    magnetometroZ = mZ_out*GaussXDigit;  
  }
}

float ajusteMagnetometro(){
  ejeXmagnetometro();
  ejeYmagnetometro();
  ejeZmagnetometro();
  float x = matrizMagB[0]*magnetometroX;
  float y = matrizMagB[1]*magnetometroY;
  float z = matrizMagB[2]*magnetometroZ;
  magnetometroX = x*matrizMagA[0][0]+y*matrizMagA[1][0]+z*matrizMagA[2][0];
  magnetometroY = x*matrizMagA[0][1]+y*matrizMagA[1][1]+z*matrizMagA[2][1];
  magnetometroZ = x*matrizMagA[0][2]+y*matrizMagA[1][2]+z*matrizMagA[2][2];
}
