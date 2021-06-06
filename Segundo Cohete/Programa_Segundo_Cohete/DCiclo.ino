void loop() {
  int j = 0;
  if(EEPROM.read(addressMicroSD) == 1){
  archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
  if(archivo){
    while(j<20){
      //Obteniendo datos raw del magnetometro
      if(EEPROM.read(addressMagnetometro) == 1){
      magnetometro.getHeading(&mx, &my, &mz);
      }
      else{
        Serial.println("Sin datos del magnetometro");
      }
      //Leer los datos del MPU
      if(EEPROM.read(addressMPU) == 1){
      MPUGetData();
      }
      else{
        Serial.println("Sin datos del MPU");
      }
      tiempo2 = millis();
      //Esta condición se cumple cada segundo que transcurre
      if(EEPROM.read(addressRTC) == 1){
        if(tiempo2 > (tiempo1+1000)){
          tiempo1 = millis();
          DateTime dia = rtc.now();
          fecha = dia.day() + diagonal + dia.month() + diagonal + dia.hour() + dosPuntos + dia.minute() + dosPuntos + dia.second();
        }
      }
      else{
        Serial.println("Sin RTC");
      }
      archivo.print(mx);
      archivo.print(",");
      archivo.print(my);
      archivo.print(",");
      archivo.print(mz);
      archivo.print(",");
      archivo.print(1000*ax);
      archivo.print(",");
      archivo.print(1000*ay);
      archivo.print(",");
      archivo.print(1000*az);
      archivo.print(",");
      archivo.print(gx,1);
      archivo.print(",");
      archivo.print(gy,1);
      archivo.print(",");
      archivo.print(gz,1);
      archivo.print(",");
      archivo.println(fecha);  
      j = j+1;
    }
 }
 archivo.close();
  }
  else{
    Serial.println("No se pueden guardar los datos");
  }
}
