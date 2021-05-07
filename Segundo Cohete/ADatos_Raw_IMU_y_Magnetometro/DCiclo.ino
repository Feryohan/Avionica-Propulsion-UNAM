void loop() {
  //Obteniendo datos raw del magnetometro
  magnetometro.getHeading(&mx, &my, &mz);
  // Leer las aceleraciones y velocidades angulares
  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);

  tiempo2 = millis();
  if(tiempo2 > (tiempo1+1000)){
    tiempo1 = millis();
    DateTime fecha = rtc.now();
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
    delay(1000);
  }
  
    archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
    if(archivo){
    archivo.print(mx);
    archivo.print(",");
    archivo.print(my);
    archivo.print(",");
    archivo.print(mz);
    archivo.print(",");
    archivo.print(ax);
    archivo.print(",");
    archivo.print(ay);
    archivo.print(",");
    archivo.print(az);
    archivo.print(",");
    archivo.print(gx);
    archivo.print(",");
    archivo.print(gy);
    archivo.print(",");
    archivo.println(gz);
  }
  archivo.close();
}
