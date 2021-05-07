//DECLARACION DE FUNCIONES

//--- Wire Transmission ---
void wireTransmission(int direccion,int registro,int valor)
{
  Wire.beginTransmission(direccion);
  Wire.write(registro);              
  Wire.write(valor); 
  Wire.endTransmission();
}

void wireTransmissionSimple(int direccion,int registro)
{
  Wire.beginTransmission(direccion);
  Wire.write(registro);    
  Wire.endTransmission();
}

//--- Funcion de registro de datos ---
void registroDatos()
{
  archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
  if(archivo){
    archivo.print(numeroLectura);
    archivo.print(",");
    archivo.print(millis());
    archivo.print(",");
    archivo.print(fix.dateTime.minutes);
    archivo.print(":");
    archivo.print(fix.dateTime.seconds);
    archivo.print(",");
    archivo.print(alturaActual);
    archivo.print(",");
    archivo.print(aceleracionX);
    archivo.print(",");
    archivo.print(aceleracionY);
    archivo.print(",");
    archivo.print(aceleracionZ);
    archivo.print(",");
    archivo.print(magnetometroX);
    archivo.print(",");
    archivo.print(magnetometroY);
    archivo.print(",");
    archivo.print(magnetometroZ);
    archivo.print(",");
    archivo.print(giroscopioX);
    archivo.print(",");
    archivo.print(giroscopioY);
    archivo.print(",");
    archivo.print(giroscopioZ);
    archivo.print(",");
    archivo.print(Estado);
    archivo.print(",");
    archivo.print(fix.longitude(), 6);
    archivo.print(",");
    archivo.print(fix.latitude(), 6);
    archivo.print(",");
    archivo.println(fix.altitude());
    archivo.close();
  }
}

void calculoMediaMovil(){
  //Se leen las primeros nAlturas en los espacio de arregloAlturas
  if(numeroLectura <= nAlturas){
    arregloAlturas[numeroLectura-1] = altura;
  }
  else{
    alturaAnterior = promedioAlturas();   //Altura filtrada
    recorrer();                           //Agregar nueva lectura 
    alturaActual = promedioAlturas();
    alturaMAX();
    validacion();
  }
}

float alturaMAX(){
  if (alturaActual > alturaAnterior){
    alturaMaxima = alturaActual;
    Estado = 'A';
  }
  else if(alturaActual == alturaAnterior){
    Estado = 'Q';
  }
  else{
    Estado = 'D';
  }
}

long promedioAlturas(){
  long suma = 0;
  for(int i=0; i<nAlturas;i++){
    suma += arregloAlturas[i];
  }
  return suma/nAlturas;
}

void recorrer(){
  for(int i=0;i<nAlturas-1;i++){
    arregloAlturas[i] = arregloAlturas[i+1];
  }
  arregloAlturas[nAlturas-1] = altura;
}

void datosGPS(){
  gps.available( gpsPort );
  fix = gps.read();
}

void validacion(){
  if(alturaActual <= alturaMaxima - rango){
    digitalWrite(Relay, HIGH);
    Estado = 'E';
    analogWrite(Buzzer,1500);
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
  wireTransmissionSimple(BMP085_ADDRESS,address);
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
  wireTransmissionSimple(BMP085_ADDRESS,address);
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
  wireTransmission(BMP085_ADDRESS,0xF4,0x2E);
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
  wireTransmission(BMP085_ADDRESS,0xF4,0x34 + (OSS<<6));
  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));
  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  wireTransmissionSimple(BMP085_ADDRESS,0xF6);
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

//Cálculo de los valores del acelerometro
void valoresAcelerometro(){
  //---> Valor del acelerometro en el eje X
  wireTransmission(ADXL345_ADDRESS,X_Axis_Register_DATAX0,X_Axis_Register_DATAX1);
  // Begin transmission to the Sensor 
  //Ask the particular registers for data
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
  //---> Valor del acelerometro en el eje Y
  wireTransmission(ADXL345_ADDRESS,Y_Axis_Register_DATAY0,Y_Axis_Register_DATAY1);
  Wire.requestFrom(ADXL345_ADDRESS,2);
  if(Wire.available()<=2) { 
    Y0 = Wire.read();
    Y1 = Wire.read();
    Y1=Y1<<8;
    Y_out= Y0 + Y1;
    aceleracionY = Y_out/AceleracionXBytes;
  }
  //---> Valor del acelerometro en el eje Z
  wireTransmission(ADXL345_ADDRESS,Z_Axis_Register_DATAZ0,Z_Axis_Register_DATAZ1);
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

//Cálculo de los valores del giroscopio
void valoresGiroscopio()
{
  //---> Valor del giroscopio en el eje X
  wireTransmissionSimple(Gyro_L3G4200D_ADDRESS,Gyro_gX0);
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gX0 = Wire.read();
  }
  wireTransmissionSimple(Gyro_L3G4200D_ADDRESS,Gyro_gX1);
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gX1 = Wire.read();
  }
  // Raw Data
  gX1=gX1<<8;
  gX_out =gX0+gX1;
  // From the datasheet: 70 mdps/digit
  giroscopioX = gX_out*dpsXDigit; // Angular rate
  //---> Valor del giroscopio en el eje Y
  wireTransmissionSimple(Gyro_L3G4200D_ADDRESS,Gyro_gY0);
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gY0 = Wire.read();
  }
  wireTransmissionSimple(Gyro_L3G4200D_ADDRESS,Gyro_gY1);
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gY1 = Wire.read();
  }
  gY1=gY1<<8;
  gY_out =gY0+gY1;
  giroscopioY = gY_out*dpsXDigit;
  //---> Valor del giroscopio en el eje Z
  wireTransmissionSimple(Gyro_L3G4200D_ADDRESS,Gyro_gZ0);
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gZ0 = Wire.read();
  }
  wireTransmissionSimple(Gyro_L3G4200D_ADDRESS,Gyro_gZ1);
  Wire.requestFrom(Gyro_L3G4200D_ADDRESS,1); 
  if(Wire.available()<=1)   
  {
    gZ1 = Wire.read();
  }
  gZ1=gZ1<<8;
  gZ_out =gZ0+gZ1;
  giroscopioZ = gZ_out*dpsXDigit;
}

//*********************************************************************************************************************************************************************************************
//************************************************************************************* Métodos MC5883L ****************************************************************************************
//*********************************************************************************************************************************************************************************************

//Cálculo de los valores del magnetometro
float valoresMagnetometro(){
  //---> Valor del magnetometro en el eje X
  wireTransmission(Magnet_MC5883L_ADDRESS,Magnetometer_mX0,Magnetometer_mX1);
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
  //---> Valor del magnetometro en el eje Y
  wireTransmission(Magnet_MC5883L_ADDRESS,Magnetometer_mY0,Magnetometer_mY1);
  Wire.requestFrom(Magnet_MC5883L_ADDRESS,2); 
  if(Wire.available()<=2)   
  {
    mY0 = Wire.read();
    mY1 = Wire.read();
    mY1= mY1<<8;
    mY_out= mY0 + mY1; 
    magnetometroY = mY_out*GaussXDigit;  
  }
  //---> Valor del magnetometro en el eje Z
  wireTransmission(Magnet_MC5883L_ADDRESS,Magnetometer_mZ0,Magnetometer_mZ1);
  Wire.requestFrom(Magnet_MC5883L_ADDRESS,2); 
  if(Wire.available()<=2)   
  {
    mZ0 = Wire.read();
    mZ1 = Wire.read();
    mZ1= mZ1<<8;
    mZ_out= mZ0 + mZ1; 
    magnetometroZ = mZ_out*GaussXDigit;  
  }
  //---> Ajuste de los valores del magnetometro
  //float x = matrizMagB[0]*magnetometroX;
  //float y = matrizMagB[1]*magnetometroY;
  //float z = matrizMagB[2]*magnetometroZ;
  //magnetometroX = x*matrizMagA[0][0]+y*matrizMagA[1][0]+z*matrizMagA[2][0];
  //magnetometroY = x*matrizMagA[0][1]+y*matrizMagA[1][1]+z*matrizMagA[2][1];
  //magnetometroZ = x*matrizMagA[0][2]+y*matrizMagA[1][2]+z*matrizMagA[2][2];
}
