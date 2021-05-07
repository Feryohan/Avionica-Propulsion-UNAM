//CICLO PRINCIPAL

void loop() {   
  //--> BMP085 
  temperature = bmp085GetTemperature(bmp085ReadUT()); 
  pressure = bmp085GetPressure(bmp085ReadUP());
  altitud(pressure, temperature);
  //--> ADXL345
  valoresAcelerometro();
  //--> L3G4200D
  start=millis();
  valoresGiroscopio();
  // Calculating dt
  finished=millis();
  elapsed=finished-start;
  dt=elapsed/1000.0;
  start = elapsed = 0;
  //--> MC5883L
  valoresMagnetometro();
  //--> GPS
  datosGPS();
  //--> Media movil de la altura
  calculoMediaMovil();  
  //--> Registro de los datos en la micro SD
  registroDatos();
  numeroLectura = numeroLectura + 1;
}
