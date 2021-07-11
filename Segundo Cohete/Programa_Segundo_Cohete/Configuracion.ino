void setup(){
  wdt_disable();
  Serial.begin(9600);
  delay(5000);
  wdt_enable(WDTO_2S);
  Wire.begin();
  Serial.println("Arduino iniciado");
  valoresDefaultMPU();
  // - - Configuración - -
  
  //-> MPU
  if(EEPROM.read(estadoMPU) == 1){
    Serial.println("datosMPU = 1");
    estadoSensor(1, datosMPU);
    Serial.println("estadoMPU = 0");
    estadoSensor(0, estadoMPU);
    wdt_reset();
    MPUSelfTest();                             //Diagnostico y calibración del MPU
    estadoSensor(1, estadoMPU);
    Serial.println("Inicio MPU");
  }
  else{
    Serial.println("Fallo MPU");
  }
  wdt_reset();

  //-> GPS
 /* serial.begin(9600);
  for(int i = 0; i < sizeof(UBLOX_INIT); i++) {                        
  serial.write( pgm_read_byte(UBLOX_INIT+i) );
  delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
  }*/

  //Serial.println("GPS configurado");

  //-> RTC
  /*if(EEPROM.read(estadoRTC) == 1){
    wdt_reset();
    estadoSensor(1, datosRTC);
    estadoSensor(0, estadoRTC);
    rtc.adjust(DateTime(__DATE__, __TIME__));  //Obtener fecha actual
    Serial.println("RTC configurado");
    estadoSensor(1, estadoRTC);
  }
  else{
    Serial.println("RTC ignorado");
    //wdt_reset();
  }*/
  
  //-> Módulo MicroSD
  if(EEPROM.read(estadoModuloSD) == 1){
    estadoSensor(1, registroDatosSD);
    estadoSensor(0, estadoModuloSD);
    if(SD.begin(SSpin)){
      estadoSensor(1, estadoModuloSD);
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
    Serial.println("Ignoro Modulo MicroSD");
  }
  wdt_reset();
//  tiempo1 = millis();
}
