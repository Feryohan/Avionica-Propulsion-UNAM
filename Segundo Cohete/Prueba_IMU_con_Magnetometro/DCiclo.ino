void loop() {
  //Obteniendo datos raw del magnetometro
  magnetometro.getHeading(&mx, &my, &mz);
  // Leer las aceleraciones y velocidades angulares
  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);

  //tiempo2 = millis();
  //if(tiempo2 > (tiempo1+1000)){
   // tiempo1 = millis();
  /*  DateTime fecha = rtc.now();
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
    if(archivo){ */
    Serial.print(mx);
    Serial.print(",");
    Serial.print(my);
    Serial.print(",");
    Serial.print(my);
    Serial.print(",");
    Serial.print(ax);
    Serial.print(",");
    Serial.print(ay);
    Serial.print(",");
    Serial.print(az);
    Serial.print(",");
    Serial.print(gx);
    Serial.print(",");
    Serial.print(gy);
    Serial.print(",");
    Serial.println(gz);
 // }
//  archivo.close();
}
