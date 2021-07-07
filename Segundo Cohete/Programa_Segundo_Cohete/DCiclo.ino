void loop() {
  int j = 0;
  if(EEPROM.read(estadoModuloSD) == 1 && EEPROM.read(archivoMemoriaSD) == 1){
    if(EEPROM.read(registroDatosSD) == 1){
      estadoSensor(0, registroDatosSD);
      archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
        if(archivo){
          while(j<20){
            //Leer los datos del MPU
//             obtenerDatosMPU();
            //GPS
             //obtenerDatosGPS();      
            //RTC
              obtenerDatosRTC();
            //Escribir en la microSD
              escribirDatos();
              j = j+1;
              wdt_reset();
           }
          estadoSensor(1, registroDatosSD);
        }
      archivo.close();
     }
  }
}
