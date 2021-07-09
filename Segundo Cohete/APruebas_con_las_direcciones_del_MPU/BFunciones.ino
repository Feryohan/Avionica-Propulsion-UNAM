void wireTransmission(int dispositivo,int registro0,int registro1)
{
  Wire.beginTransmission(dispositivo);
  Wire.write(registro0);              
  Wire.write(registro1); 
  Wire.endTransmission();
  Wire.requestFrom(dispositivo,2);
}

void wireTransmissionSimple(int dispositivo,int registro)
{
  Wire.beginTransmission(dispositivo);
  Wire.write(registro);    
  Wire.endTransmission();
  Wire.requestFrom(dispositivo,1);
}

//Cálculo de los valores del acelerometro
float calculoAceleracion(int dispositivo,int registro0,int registro1){
  float aceleracion = 0;
  wireTransmission(dispositivo,registro0,registro1);
  if(Wire.available()<=2){
    int raw0 = Wire.read();
    int raw1 = Wire.read();
    raw1 = raw1<<8;
    int raw_out = raw0 + raw1;
    aceleracion = raw_out/AceleracionXBytes;
  }
  return aceleracion;
}

void valoresAcelerometro(){
    aceleracionX = calculoAceleracion(MPU6050_ADDRESS,ACCEL_XOUT_H,ACCEL_XOUT_L);
    aceleracionY = calculoAceleracion(MPU6050_ADDRESS,ACCEL_YOUT_H,ACCEL_YOUT_L);
    aceleracionZ = calculoAceleracion(MPU6050_ADDRESS,ACCEL_ZOUT_H,ACCEL_ZOUT_L);
}


float calculoGiroscopio(int dispositivo,int registro0,int registro1){
  float giroscopio = 0;
  int raw0,raw1;
  wireTransmissionSimple(dispositivo,registro0);
  if(Wire.available()<=1)   
  {
    raw0 = Wire.read();
  }
  wireTransmissionSimple(dispositivo,registro1);
  if(Wire.available()<=1)   
  {
    raw1 = Wire.read();
  }
  // Raw Data
  raw1 = raw1<<8;
  int raw_out = raw0 + raw1;
  giroscopio = raw_out*dpsXDigit;
}

void valoresGiroscopio(){
    giroscopioX = calculoGiroscopio(MPU6050_ADDRESS,GYRO_XOUT_H,GYRO_XOUT_L);
    giroscopioY = calculoGiroscopio(MPU6050_ADDRESS,GYRO_YOUT_H,GYRO_YOUT_L);
    giroscopioZ = calculoGiroscopio(MPU6050_ADDRESS,GYRO_ZOUT_H,GYRO_ZOUT_L);
}

void wireRead(int dispositivo, int direccion)
{
  Wire.beginTransmission(dispositivo);
  Wire.write(direccion);
  Wire.endTransmission();
  Wire.requestFrom(dispositivo,1);
  int DataFormat_data = Wire.read();            //Guardamos el valor por default
  Serial.println("El valor almacenado en: " + String(direccion) + " es: " + String(DataFormat_data));
}

void wireConfiguration(int dispositivo, int direccion, int valor)
{
  Wire.beginTransmission(dispositivo);
  Wire.write(direccion);              
  Wire.write(valor); 
  Wire.endTransmission();
}
