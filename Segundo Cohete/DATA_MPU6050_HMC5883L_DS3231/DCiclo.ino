void loop() {
  //Obteniendo datos raw del magnetometro
  magnetometro.getHeading(&mx, &my, &mz);
  // Leer los datos del MPU
  MPUGetData();

  tiempo2 = millis();
  if(tiempo2 > (tiempo1+1000)){
    tiempo1 = millis();
    DateTime fecha = rtc.now();
    Serial.println("/////////////////////////////////////////////////////");
    Serial.print(fecha.day());
    Serial.print("/");
    Serial.print(fecha.month());
    Serial.print("/");
    Serial.print(fecha.year());
    Serial.print(" ");
    Serial.print(fecha.hour());
    Serial.print(":");
    Serial.print(fecha.minute());
    Serial.print(":");
    Serial.println(fecha.second());
    Serial.println("/////////////////////////////////////////////////////");
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
    Serial.println(gz,1);
  //}
  //archivo.close();
}
