int wireRead(int dispositivo, int direccion)
{
  Wire.beginTransmission(dispositivo);
  Wire.write(direccion);
  Wire.endTransmission();
  Wire.requestFrom(dispositivo,1);
  int DataFormat_data = Wire.read();            //Guardamos el valor por default
  return DataFormat_data;
}

void wireConfiguration(int dispositivo, int direccion, int valor)
{
  Wire.beginTransmission(dispositivo);
  Wire.write(direccion);              
  Wire.write(valor); 
  Wire.endTransmission();
}

//                                  --- Calibración ---
void estadoSensor(byte estado, byte address){
  EEPROM.write(address, estado);   //Sirve para escribir en la direccion el estado (0 o 1) del sensor
}
//--> MPU <--
void MPUinit(){
  wireConfiguration(MPU6050_ADDRESS,PWR_MGMT_1, 0x01); // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
  wireConfiguration(MPU6050_ADDRESS,CONFIG, 0x06);  //Bandwidth = 5Hz, Frecuencia de 1 kHz, delay de 19 ms
  wireConfiguration(MPU6050_ADDRESS, SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; the same rate set in CONFIG above
  uint8_t c =  wireRead(MPU6050_ADDRESS, GYRO_CONFIG);
  wireConfiguration(MPU6050_ADDRESS, GYRO_CONFIG, c & ~0xE0); // Clear self-test bits [7:5]
  wireConfiguration(MPU6050_ADDRESS, GYRO_CONFIG, c & ~0x18); // Clear AFS bits [4:3]
  wireConfiguration(MPU6050_ADDRESS, GYRO_CONFIG, c | Gscale << 3); // Set full scale range for the gyro
  c =  wireRead(MPU6050_ADDRESS, ACCEL_CONFIG);
  wireConfiguration(MPU6050_ADDRESS, ACCEL_CONFIG, c & ~0xE0); // Clear self-test bits [7:5]
  wireConfiguration(MPU6050_ADDRESS, ACCEL_CONFIG, c & ~0x18); // Clear AFS bits [4:3]
  wireConfiguration(MPU6050_ADDRESS, ACCEL_CONFIG, c | Ascale << 3); // Set full scale range for the accelerometer
  wireConfiguration(MPU6050_ADDRESS, INT_PIN_CFG, 0x22);
  wireConfiguration(MPU6050_ADDRESS, INT_ENABLE, 0x01);  // Enable data ready (bit 0) interrupt
}
//--> Módulo MicroSD <--
void numeroArchivo(){
  nFile = EEPROM.read(500);          //Usamos la dirección 500 de la EEPROM
  EEPROM.write(500,nFile+1);         //Tenemos un máximo de 255 archivos
  nFile = EEPROM.read(500);
}
void iniciarArchivo(){
  estadoSensor(0, archivoMemoriaSD);
  archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
    if(archivo){
      //Si el archivo se crea correctamente
      archivo.println("Temp,Pres,Alt,AcelX,AcelY,AcelZ,GyroX,GyroY,GyroZ,Latitud,Longitud,Altitud,Hora");
      estadoSensor(1, archivoMemoriaSD); 
    }
  archivo.close();
}

//                                  --- Obtener Datos ---
//--> MPU <--
void MPUGetData(){
  // If data ready bit set, all data registers have new data
  if(wireRead(MPU6050_ADDRESS, INT_STATUS) & 0x01) {  // check if data ready interrupt

//    mpu.readAccelData(accelCount);  // Read the x/y/z adc values

    uint8_t rawData[6];  // x/y/z accel register data stored here
    rawData[0] = wireRead(MPU6050_ADDRESS,ACCEL_XOUT_H);
    rawData[1] = wireRead(MPU6050_ADDRESS,ACCEL_XOUT_L);
    rawData[2] = wireRead(MPU6050_ADDRESS,ACCEL_YOUT_H);
    rawData[3] = wireRead(MPU6050_ADDRESS,ACCEL_YOUT_L);
    rawData[4] = wireRead(MPU6050_ADDRESS,ACCEL_ZOUT_H);
    rawData[5] = wireRead(MPU6050_ADDRESS,ACCEL_ZOUT_L);
    accelCount[0] = (int16_t)((rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
    accelCount[1] = (int16_t)((rawData[2] << 8) | rawData[3]) ;
    accelCount[2] = (int16_t)((rawData[4] << 8) | rawData[5]) ;
    
    
    // Now we'll calculate the accleration value into actual g's
    ax = (float)accelCount[0]*Aescala;
    ay = (float)accelCount[1]*Aescala;
    az = (float)accelCount[2]*Aescala;
   
    rawData[0] = wireRead(MPU6050_ADDRESS,GYRO_XOUT_H);
    rawData[1] = wireRead(MPU6050_ADDRESS,GYRO_XOUT_L);
    rawData[2] = wireRead(MPU6050_ADDRESS,GYRO_YOUT_H);
    rawData[3] = wireRead(MPU6050_ADDRESS,GYRO_YOUT_L);
    rawData[4] = wireRead(MPU6050_ADDRESS,GYRO_ZOUT_H);
    rawData[5] = wireRead(MPU6050_ADDRESS,GYRO_ZOUT_L);
    gyroCount[0] = (int16_t)((rawData[0] << 8) | rawData[1]) ;  // Turn the MSB and LSB into a signed 16-bit value
    gyroCount[1] = (int16_t)((rawData[2] << 8) | rawData[3]) ;
    gyroCount[2] = (int16_t)((rawData[4] << 8) | rawData[5]) ;
 
    // Calculate the gyro value into actual degrees per second
    gx = (float)gyroCount[0]*Gescala;
    gy = (float)gyroCount[1]*Gescala;
    gz = (float)gyroCount[2]*Gescala;
   } 
}
void obtenerDatosMPU(){
  if(EEPROM.read(datosMPU) == 1){
    estadoSensor(0, datosMPU);
    MPUGetData();
    estadoSensor(1, datosMPU);
  }
}

//--> BMP180 <--
void altitud(double pressure, double temperature){ 
  Altura_Actual_RAM = (log(pressure/Po))*((R*(temperature + 273.15))/(M*g))*-1;  //Calcular altitud en [m]
}
void obtenerDatosBMP()
{
  if(EEPROM.read(datosBMP) == 1){
    estadoSensor(0, datosBMP);
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
            altitud(P,T);
            estadoSensor(1, datosBMP);  
          }      
        }      
      }   
      } 
   }
}

//--> GPS <--
/*void obtenerDatosGPS()
{
    while (gps.available( gpsPort )) {
    fix = gps.read();

    if (fix.valid.location) {
      latitudDato = fix.latitude();
      longitudDato = fix.longitude();
    }
    if (fix.valid.altitude){
      altitudDato = fix.altitude(); 
    }
  }
}
*/
//--> RTC <--
/*void obtenerDatosRTC(){
  tiempo2 = millis();
  if(tiempo2 > (tiempo1+1000)){
    tiempo1 = millis();
    if(EEPROM.read(datosRTC) == 1){
      estadoSensor(0,datosRTC);
      DateTime dia = rtc.now();
      fecha = dia.day() + diagonal + dia.month() + diagonal + dia.hour() + dosPuntos + dia.minute() + dosPuntos + dia.second();
      estadoSensor(1,datosRTC);
    } 
  }
}
*/

//                                  --- Guardar Datos ---
void escribirDatos(){
  archivo.print(T);
  archivo.print(",");
  archivo.print(P);
  archivo.print(",");
  archivo.print(Altura_Actual_RAM);
  archivo.print(",");
  archivo.print(ax);
  archivo.print(",");
  archivo.print(ay);
  archivo.print(",");
  archivo.print(az);
  archivo.print(",");
  archivo.print(gx,1);
  archivo.print(",");
  archivo.print(gy,1);
  archivo.print(",");
  archivo.println(gz,1);
 /* archivo.print(",");
  archivo.print((latitudDato),6);
  archivo.print(",");
  archivo.print((longitudDato),6);
  archivo.print(",");
  archivo.println(altitudDato);*/
  //archivo.println(fecha);
}
