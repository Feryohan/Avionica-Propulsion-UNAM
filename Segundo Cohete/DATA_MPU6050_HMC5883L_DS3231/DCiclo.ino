void loop() {
  //Obteniendo datos raw del magnetometro
  magnetometro.getHeading(&mx, &my, &mz);
  // Leer los datos del MPU
  MPUGetData();

  tiempo2 = millis();
  if(tiempo2 > (tiempo1+1000)){
    tiempo1 = millis();
    DateTime dia = rtc.now();
    fecha = dia.day() + diagonal + dia.month() + diagonal + dia.hour() + dosPuntos + dia.minute() + dosPuntos + dia.second();
  }
  
    //archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
    //if(archivo){
    Serial.print(mx);
    Serial.print(",");
    Serial.print(my);
    Serial.print(",");
    Serial.print(mz);
    Serial.print(",");
    Serial.print(1000*ax);
    Serial.print(",");
    Serial.print(1000*ay);
    Serial.print(",");
    Serial.print(1000*az);
    Serial.print(",");
    Serial.print(gx,1);
    Serial.print(",");
    Serial.print(gy,1);
    Serial.print(",");
    Serial.print(gz,1);
    Serial.print(",");
    Serial.println(fecha);
  //}
  //archivo.close();
}
