void setup() {
  wdt_disable();
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Setup.. Esperemos 5 segunods para dar tiempo a cargar un nuevo sketch");
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

  //-> RTC
  if(EEPROM.read(addressRTC) == 1){
    wdt_reset();
    estadoSensor(0, addressRTC);
    rtc.adjust(DateTime(__DATE__, __TIME__));  //Obtener fecha actual
    Serial.println("RTC configurado");
    estadoSensor(1, addressRTC);
  }
  else{
    Serial.println("RTC ignorado");
    //wdt_reset();
  }
  
  //-> Módulo MicroSD
  if(EEPROM.read(addressMicroSD) == 1){
    estadoSensor(0, addressMicroSD);
    if(SD.begin(SSpin)){
      Serial.println("Inicio Modulo");
      //->Leer y actualizar el numero de archivo
      numeroArchivo();
      //->Crear archivo de datos de vuelo
      iniciarArchivo();
    }
    else{
      Serial.println("Fallo Modulo");
    //wdt_reset();
    }
  }
  else{
    Serial.println("Ignoro microSD");
  }
  wdt_reset();
  tiempo1 = millis();
}
