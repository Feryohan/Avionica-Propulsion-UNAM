void setup() {
  wdt_disable();
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Setup..");

  Serial.println("Wait 5 sec..");
  delay(5000); // espero 5 segundos, para dar tiempo a cargar un nuevo sketch.
  Serial.println("Arduino iniciado");
  wdt_enable(WDTO_4S);
  // - - Configuración - -
  //-> MPU
  if(EEPROM.read(addressMPU) == 1){
    estadoSensor(0, addressMPU);
    MPUSelfTest();                             //Diagnostico y calibración del MPU
    estadoSensor(1, addressMPU);
    Serial.println("Inicio MPU");
  }
  else{
    Serial.println("Fallo MPU");
  }

  //-> GPS
 /* for(int i = 0; i < sizeof(UBLOX_INIT); i++) {                        
  serial.write( pgm_read_byte(UBLOX_INIT+i) );
  delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
  }*/
  
  //-> Magnetometro
  if(EEPROM.read(addressMagnetometro) == 1){
    estadoSensor(0, addressMagnetometro);
    magnetometro.initialize();                 //Inicialización del magnetometro
    estadoSensor(1, addressMagnetometro);
    Serial.println("Inicio Magnetometro");
  }
  else{
    Serial.println("Fallo Magnetometro");
  }

  //-> RTC
 /* if(EEPROM.read(addressRTC) == 1){
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
      numeroArchivo();
      //->Crear archivo de datos de vuelo
      iniciarArchivo();
      Serial.println("Inicio ArchivoSD");
    }
    else{
      Serial.println("Fallo ArchivoSD");
    //wdt_reset();
    }
    Serial.println("Inicio microSD");
  }
  else{
    Serial.println("Fallo microSD");
  }
  wdt_reset();
//    tiempo1 = millis();
}
