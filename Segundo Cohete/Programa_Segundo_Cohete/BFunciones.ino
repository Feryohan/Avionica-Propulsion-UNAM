void estadoSensor(byte estado, byte address){
  EEPROM.write(address, estado);
}

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
      archivo.close();
      //wdt_reset();
      estadoSensor(1, archivoMemoriaSD); 
    }
}

//- - - Configuración - - -

void MPUSelfTest(){
  mpu.MPU6050SelfTest(SelfTest); // Start by performing self test and reporting values
  if(SelfTest[0] < 1.0f && SelfTest[1] < 1.0f && SelfTest[2] < 1.0f && SelfTest[3] < 1.0f && SelfTest[4] < 1.0f && SelfTest[5] < 1.0f) {
    mpu.calibrateMPU6050(gyroBias, accelBias);   // Calibrate gyro and accelerometers, load biases in bias registers  
    mpu.initMPU6050();                           // Initialize device for active mode read of acclerometer and gyroscope
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

void altitud(double pressure, double temperature){
  TempK = temperature + 273.15;       //Calcular altitud en [m]
  altura = (log(pressure/Po))*((R*TempK)/(M*g))*-1;
}

void obtenerDatosBMP(){
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
        }      
      }      
    }   
  } 
    estadoSensor(1, datosBMP);
  }
}

void obtenerDatosGPS(){
if(serial.available()){
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
    }
  }
 }
}

 void obtenerDatosMPU(){
  if(EEPROM.read(datosMPU) == 1){
    estadoSensor(0, datosMPU);
    MPUGetData();
    estadoSensor(1, datosMPU);
  }
  else{
    ax = 0;
    ay = 0;
    az = 0;
    gx = 0;
    gy = 0;
    gz = 0;
  }
}

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
    else{
      Serial.println("Sin RTC");
      fecha = "sin fecha";
    }
  }
}
*/
void escribirDatos(){
  archivo.print(T);
  archivo.print(",");
  archivo.print(P);
  archivo.print(",");
  archivo.print(altura);
  archivo.print(",");
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
  archivo.print(lat);
  archivo.print(",");
  archivo.print(lon);
  archivo.print(",");
  archivo.println(hMSL);
  //archivo.println(fecha);
}
