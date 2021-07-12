//                                  --- Calibración ---
void estadoSensor(byte estado, byte address){
  EEPROM.write(address, estado);   //Sirve para escribir en la direccion el estado (0 o 1) del sensor
}
//--> MPU <--
void MPUConfiguracion(){
  //mpu.calibrateMPU6050(gyroBias, accelBias);   // Calibrate gyro and accelerometers, load biases in bias registers  
  mpu.initMPU6050();
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
  if(mpu.readByte(MPU6050_ADDRESS, INT_STATUS) & 0x01) {  // check if data ready interrupt
    mpu.readAccelData(accelCount);  // Read the x/y/z adc values
    // Now we'll calculate the accleration value into actual g's
    ax = (float)accelCount[0]*aRes; //- accelBias[0];  // get actual g value, this depends on scale being set
    ay = (float)accelCount[1]*aRes; //- accelBias[1];   
    az = (float)accelCount[2]*aRes; //- accelBias[2]; 
  
    mpu.readGyroData(gyroCount);  // Read the x/y/z adc values
    // Calculate the gyro value into actual degrees per second
    gx = (float)gyroCount[0]*gRes; //- gyroBias[0];  // get actual gyro value, this depends on scale being set
    gy = (float)gyroCount[1]*gRes;// - gyroBias[1];
    gz = (float)gyroCount[2]*gRes; //- gyroBias[2];
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
  altura = (log(pressure/Po))*((R*(temperature + 273.15))/(M*g))*-1;  //Calcular altitud en [m]
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
void obtenerDatosGPS()
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
  archivo.print(altura);
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
  archivo.print(gz,1);
  archivo.print(",");
  archivo.print((latitudDato),6);
  archivo.print(",");
  archivo.print((longitudDato),6);
  archivo.print(",");
  archivo.println(altitudDato);
  //archivo.println(fecha);
}
