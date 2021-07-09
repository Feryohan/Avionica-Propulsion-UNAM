void setup(){
  wdt_disable();
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Setup.. Esperemos 5 segundos para dar tiempo a cargar un nuevo sketch");
  delay(5000); // espero 5 segundos, para dar tiempo a cargar un nuevo sketch.
  Serial.println("Arduino iniciado");
  wdt_enable(WDTO_2S);
  
  // - - Configuración - -
  
  //-> MPU
  if(EEPROM.read(estadoMPU) == 1){
    estadoSensor(1, datosAcelerometro);
    estadoSensor(1, datosGiroscopio);
    estadoSensor(0, estadoMPU);
    Wire.beginTransmission(MPU6050_ADDRESS);
    Wire.write(0x6B); //PWR_MGMT_1
    Wire.write(0); //Despierta al MPU6050
    Wire.endTransmission();
    //Estableciendo Bandwidth y Frecuencia de muestreo
    //wireConfiguration(MPU6050_ADDRESS,CONFIG,DLPF_CFG);
    //Estableciendo la escala completa del giroscopio
    //wireConfiguration(MPU6050_ADDRESS,GYRO_CONFIG,FS_SEL);
    //Estableciendo la escala completa del acelerometro
    //wireConfiguration(MPU6050_ADDRESS,ACCEL_CONFIG,AFS_SEL);
    Serial.println("MPU configurada");
    estadoSensor(1, estadoMPU);
  }

  //-> GPS
 /* for(int i = 0; i < sizeof(UBLOX_INIT); i++) {                        
  serial.write( pgm_read_byte(UBLOX_INIT+i) );
  delay(5); // simulating a 38400baud pace (or less), otherwise commands are not accepted by the device.
  }*/

  Serial.println("GPS configurado");

  //-> RTC
  if(EEPROM.read(estadoRTC) == 1){
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
  }
  wdt_reset();
  
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
    Serial.println("Ignoro microSD");
  }
  wdt_reset();
  tiempo1 = millis();
}
