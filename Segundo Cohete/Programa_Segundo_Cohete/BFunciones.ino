void estadoSensor(byte estado, byte address){
    EEPROM.write(address, estado);
}

void numeroArchivo(){
  nFile = EEPROM.read(500);          //Usamos la dirección 500 de la EEPROM
  EEPROM.write(500,nFile+1);         //Tenemos un máximo de 255 archivos
  nFile = EEPROM.read(500);
}

void iniciarArchivo(){
  archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
    if(archivo){
      //Si el archivo se crea correctamente
      archivo.println("AcelX,AcelY,AcelZ,GyroX,GyroY,GyroZ,Latitud,Longitud,Altitud");
      archivo.close();
      //wdt_reset();
      estadoSensor(1, addressMicroSD);
    }
    else{
        
    }
}

//- - - Configuración - - -

void MPUSelfTest(){
  mpu.MPU6050SelfTest(SelfTest); // Start by performing self test and reporting values
  if(SelfTest[0] < 1.0f && SelfTest[1] < 1.0f && SelfTest[2] < 1.0f && SelfTest[3] < 1.0f && SelfTest[4] < 1.0f && SelfTest[5] < 1.0f) {
    mpu.calibrateMPU6050(gyroBias, accelBias);   // Calibrate gyro and accelerometers, load biases in bias registers  
    mpu.initMPU6050();                           // Initialize device for active mode read of acclerometer and gyroscope
    }
  else
    {
      //
    }
}

void MPUGetData(){
  // If data ready bit set, all data registers have new data
  if(mpu.readByte(MPU6050_ADDRESS, INT_STATUS) & 0x01) {  // check if data ready interrupt

    mpu.readAccelData(accelCount);  // Read the x/y/z adc values
    aRes=mpu.getAres();
    
    // Now we'll calculate the accleration value into actual g's
    ax = (float)accelCount[0]*aRes - accelBias[0];  // get actual g value, this depends on scale being set
    ay = (float)accelCount[1]*aRes - accelBias[1];   
    az = (float)accelCount[2]*aRes - accelBias[2];  
   
    mpu.readGyroData(gyroCount);  // Read the x/y/z adc values
    gRes=mpu.getGres();
 
    // Calculate the gyro value into actual degrees per second
    gx = (float)gyroCount[0]*gRes - gyroBias[0];  // get actual gyro value, this depends on scale being set
    gy = (float)gyroCount[1]*gRes - gyroBias[1];  
    gz = (float)gyroCount[2]*gRes - gyroBias[2];   
   }  
}

// - - - Ciclo - - -

void datosGPS(){
  while(gps.available( gpsPort )){
    fix = gps.read();
  }
}

/*void datosMagnetometro(){
  if(EEPROM.read(addressMagnetometro) == 1){
    estadoSensor(0, addressMagnetometro);
    magnetometro.getHeading(&mx, &my, &mz);
    estadoSensor(1, addressMagnetometro);
  }
  else{
  }
}*/

void datosMPU(){
  if(EEPROM.read(addressMPU) == 1){
    estadoSensor(0, addressMPU);
    MPUGetData();
    estadoSensor(1, addressMPU);
  }
  else{

  }
}

/*void datosRTC(){
  tiempo2 = millis();
  if(EEPROM.read(addressRTC) == 1){
    estadoSensor(0,addressRTC);
    if(tiempo2 > (tiempo1+1000)){
        tiempo1 = millis();
        DateTime dia = rtc.now();
        fecha = dia.day() + diagonal + dia.month() + diagonal + dia.hour() + dosPuntos + dia.minute() + dosPuntos + dia.second();
        estadoSensor(1,addressRTC);
      }
    }
    else{
      Serial.println("Sin RTC");
    }
}
*/
void escribirDatos(){
/*  archivo.print(mx);
  archivo.print(",");
  archivo.print(my);
  archivo.print(",");
  archivo.print(mz);
  archivo.print(",");*/
  archivo.print(1000*ax);
  archivo.print(",");
  archivo.print(1000*ay);
  archivo.print(",");
  archivo.print(1000*az);
  archivo.print(",");
  archivo.print(gx,1);
  archivo.print(",");
  archivo.print(gy,1);
  archivo.print(",");
  archivo.print(gz,1);
  archivo.print(",");
  archivo.print(fix.latitude(), 6);
  archivo.print(",");
  archivo.print(fix.longitude(), 6);
  archivo.print(",");
  archivo.println(fix.altitude());
/*archivo.print(",");
  archivo.println(fecha);*/
}
