void loop() {
  //                                  --- Obtener Datos ---
  obtenerDatosMPU();
  obtenerDatosBMP();
  //obtenerDatosGPS();
  obtenerDatosRTC();
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
//                              --- ¿En qué etapa nos encontramos? --
  switch (EEPROM.read(estadoVuelo)) {
  case 0:   //Espera
    //evaluarSeguro();
    if(((Altura_Actual_RAM > EEPROM.get(direcc_AltTerr_EEPROM,AltTerr_EEPROM) + 15 /*metros*/)&&(estadoSeguro = 1)) || ((estadoSeguro = 1)&&(AccXYZMax > 2)) || ((AccXYZMax > 2)&&(Altura_Actual_RAM > EEPROM.get(direcc_AltTerr_EEPROM,AltTerr_EEPROM) + 15 /*metros*/))) 
    {
      estadoSensor(1, estadoVuelo);
    }
    break;
  case 1:   //Ascenso
    if(((EEPROM.get(direcc_Alt_Max_EEPROM,Altura_Maxima_EEPROM) > Altura_Actual_RAM + 10 /*metros*/)&&(Altura_Actual_RAM > EEPROM.get(direcc_AltTerr_EEPROM,AltTerr_EEPROM) + 200/*metros*/)) || (contadorTiempo > EEPROM.read(direcc_Tiempo_Ascenso_EEPROM)) || ((AccXYZMin < 0.5)&&(EEPROM.get(direcc_Alt_Max_EEPROM,Altura_Maxima_EEPROM) > Altura_Actual_RAM + 10 /*metros*/)))
    {
      //liberarParacaidas();
      estadoSensor(2, estadoVuelo);
    }
    break;
  case 2:   //Descenso
    
    break;
  }
  numeroDeLectura += 1;
  wdt_reset();
}
