void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // - - Configuración - -
  //-> MPU
  if(EEPROM.read(addressMPU) == 1){
    estadoSensor(0, addressMPU);
    MPUSelfTest();                             //Diagnostico y calibración del MPU
//  wdt_reset();
    estadoSensor(1, addressMPU);
  }
  else{
//  wdt_reset();
  }

  //GPS
  gpsPort.begin(9600);
  
  //-> Magnetometro
 /* if(EEPROM.read(addressMagnetometro) == 1){
    estadoSensor(0, addressMagnetometro);
    magnetometro.initialize();                 //Inicialización del magnetometro
    //wdt_reset();
    Serial.println("Magnetometro configurado");
    estadoSensor(1, addressMagnetometro);
  }
  else{
    Serial.println("Magnetometro ignorado");
    //wdt_reset();
  }

  //-> RTC
  if(EEPROM.read(addressRTC) == 1){
    estadoSensor(0, addressRTC);
    rtc.adjust(DateTime(__DATE__, __TIME__));  //Obtener fecha actual
    //wdt_reset();
    Serial.println("RTC configurado");
    estadoSensor(1, addressRTC);
  }
  else{
    Serial.println("Magnetometro ignorado");
    //wdt_reset();
  }
*/  
  //-> Módulo MicroSD
  if(EEPROM.read(addressMicroSD) == 1){
    estadoSensor(0, addressMicroSD);
    if(SD.begin(SSpin)){
      //->Leer y actualizar el numero de archivo
      nFile = EEPROM.read(500);          //Usamos la dirección 500 de la EEPROM
      EEPROM.write(500,nFile+1);         //Tenemos un máximo de 255 archivos
      nFile = EEPROM.read(500);
      //->Crear archivo de datos de vuelo
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
    else{
    
    //wdt_reset();
    }
  }
  else{
  }
//    tiempo1 = millis();
}
