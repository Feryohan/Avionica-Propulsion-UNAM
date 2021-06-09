void loop() {
  int j = 0;
  if(EEPROM.read(addressMicroSD) == 1){
    estadoSensor(0, addressMicroSD);
    archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
      if(archivo){
        while(j<20){
          //Obteniendo datos raw del magnetometro
            //datosMagnetometro();
          //Leer los datos del MPU
            datosMPU();
          //GPS
            datosGPS();      
          //RTC
            //datosRTC();
          //Escribir en la microSD
            escribirDatos();
            j = j+1;
         }
      }
    archivo.close();
    estadoSensor(1, addressMicroSD);
  }
  else{
    Serial.println("No se pueden guardar los datos");
  }
}
