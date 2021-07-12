void loop() {
  //                                  --- Obtener Datos ---
  obtenerDatosMPU();
  obtenerDatosBMP();
  obtenerDatosGPS();
  //obtenerDatosRTC();
  //                                   --- Guardar Datos ---
  if(EEPROM.read(estadoModuloSD) == 1 && EEPROM.read(archivoMemoriaSD) == 1 && EEPROM.read(registroDatosSD) == 1)
  {
    estadoSensor(0, registroDatosSD);
    archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
    if(archivo){
      escribirDatos();
      estadoSensor(1, registroDatosSD);
    }
    archivo.close();
  }
  wdt_reset();
}
