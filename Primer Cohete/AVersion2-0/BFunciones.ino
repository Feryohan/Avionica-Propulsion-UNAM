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
        alturaMediaMovil = round((altura1+altura2+altura3+altura4+altura5+altura6+altura7+altura8)/(8));
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
  //Se toma una muestra de 350 valores
  //Para obtener un promedio de la presión
  //Y obtener una altura inicial
  float suma = 0;                                      //Suma de datos
  int muestra = 350;
  //-->Inicio de ciclo
  for(int i=0;i<muestra;i++)
  {
    status = bmp180.startTemperature();                //Pedimos que mida la temperatura
    if(status != 0)                                    //Si no hay error
    {
      delay(status);                                   //Damos tiempo para hacer la medicion
      status = bmp180.getTemperature(temperature);     //Lectura de temperatura en temperature
      if(status != 0)
      {
        //Obtencion de presion
        status = bmp180.startPressure(3);              //Se mide la presión con resolución nivel 3
        if(status != 0)
        {
          delay(status);
          status = bmp180.getPressure(pressure,temperature);   //Lectura de presion en pressure
          if(status != 0)
          {
            suma = suma + pressure;
          }
        }//Fin de obtencion de presion
      }
    }
  }//Fin de ciclo
  promPressure = (suma/muestra)*100;
}

double altitud()
{
  TempK = temperature + 273.15;       //Calcular altitud en [m]
  double altitud = (log(pressure/promPressure))*((R*TempK)/(M*g))*-1;
  return altitud;
}

void medirPresion()
{
  status = bmp180.startTemperature();                  //Pedimos que mida la temperatura
    if(status != 0)                                    //Si no hay error
    {
      delay(status);                                   //Damos tiempo para hacer la medicion
      status = bmp180.getTemperature(temperature);     //Lectura de temperatura en temperature
      if(status != 0)
      {
        //Obtencion de presion
        status = bmp180.startPressure(2);              //Se mide la presión con resolución nivel 2
        if(status != 0)
        {
          delay(status);
          double Presionp;
          status = bmp180.getPressure(Presionp,temperature);   //Lectura de presion en pressure
          if(status != 0)
          {
            pressure = Presionp*100;
          }
        }//Fin de obtencion de presion
      }
    }
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
    Xa=X_out/AceleracionXBytes;                        // Xa = output value from -1 to +1, Gravity acceleration acting on the X-Axis
  }
  return Xa;
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
    Ya=Y_out/AceleracionXBytes;
  }
  return Ya;
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
    Za=Z_out/AceleracionXBytes;
  }
  return Za;
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
  angleX = angleX + angleXc;

  return angleX;
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
  angleY = angleY + angleYc;

  return angleY;
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
  angleZ = angleZ + angleZc;

  return angleZ;
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
    Xm = mX_out*GaussXDigit;                         // Gauss unit
    //Earth magnetic field ranges from 0.25 to 0.65 Gauss, so these are the values that we need to get approximately.
  }
  return Xm;
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
    Ym = mY_out*GaussXDigit;  
  }
  return Ym;
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
    Zm = mZ_out*GaussXDigit;  
  }
  return Zm;
}
