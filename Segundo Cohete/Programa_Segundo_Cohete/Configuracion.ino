void setup(){
  wdt_disable();
  delay(5000);
  wdt_enable(WDTO_2S);
  Wire.begin();
  // - - Configuración - -
  
  //-> MPU
  if(EEPROM.read(estadoMPU) == 1){
    estadoSensor(1, datosMPU);
    estadoSensor(0, estadoMPU);
    wdt_reset();
    MPUSelfTest();                             //Diagnostico y calibración del MPU
    estadoSensor(1, estadoMPU);
  }
  wdt_reset();

  //-> GPS
    DEBUG_PORT.begin(9600);
    gpsPort.begin(9600);

  //BMP180
  if(EEPROM.read(estadoBMP) == 1){
    estadoSensor(1,datosBMP);
    estadoSensor(0,estadoBMP);
    if(!bmp180.begin())
    {
      while(1);
    }
    estadoSensor(1,estadoBMP);
  }

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
      //->Leer y actualizar el numero de archivo
      numeroArchivo();
      //->Crear archivo de datos de vuelo
      iniciarArchivo();
    }
  }
  wdt_reset();
//  tiempo1 = millis();
}
