void setup() {
 
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Arduino iniciado");
  
  
  // - - Configuración - -
  
  //-> MPU
  if(EEPROM.read(addressMPU) == 1){
    estadoSensor(0, addressMPU);
    MPUSelfTest();                             //Diagnostico y calibración del MPU
//  wdt_reset();
    Serial.println("MPU configurado");
    estadoSensor(1, addressMPU);
  }
  else{
    Serial.println("MPU ignorado");
//  wdt_reset();
  }
  
  //-> Magnetometro
  if(EEPROM.read(addressMagnetometro) == 1){
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
  
  //-> Módulo MicroSD
  if(EEPROM.read(addressMicroSD) == 1){
    estadoSensor(0, addressMicroSD);
    //->Leer y actualizar el numero de archivo
    nFile = EEPROM.read(500);          //Usamos la dirección 500 de la EEPROM
    EEPROM.write(500,nFile+1);         //Tenemos un máximo de 255 archivos
    nFile = EEPROM.read(500);
    //->Crear archivo de datos de vuelo
    archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
    //Si el archivo se crea correctamente
    archivo.println("MagX,MagY,MagZ,AcelX,AcelY,AcelZ,GyroX,GyroY,GyroZ,Fecha");
    archivo.close();
    //wdt_reset();
    Serial.println("El documento se creo correctamente");
    estadoSensor(1, addressMicroSD);
  }
  else{
    Serial.println("ModuloMicroSD ignorado");
    //wdt_reset();
  }
    tiempo1 = millis();
}
