void loop() {
  int j = 0;
  if(EEPROM.read(addressMicroSD) == 1){
    estadoSensor(0, addressMicroSD);
    archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
      if(archivo){
        while(j<20){
          //Leer los datos del MPU
           datosMPU();
          //GPS
         //  datosGPS();      
          //RTC
            datosRTC();
          //Escribir en la microSD
            escribirDatos();
            j = j+1;
            wdt_reset();
         }
      }
    archivo.close();
    estadoSensor(1, addressMicroSD);
    Serial.println("Paquete de datos");
  }
  else{
    Serial.println("No hay info para guardar");
  }
}
