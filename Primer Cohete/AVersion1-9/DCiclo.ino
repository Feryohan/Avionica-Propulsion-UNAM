//CICLO PRINCIPAL

void loop() {   
  //--> BMP085 
  temperature = bmp085GetTemperature(bmp085ReadUT()); 
  pressure = bmp085GetPressure(bmp085ReadUP());
  altitud(pressure, temperature);
  //--> ADXL345
  ejeXacelerometro();
  ejeYacelerometro();
  ejeZacelerometro();
  //--> L3G4200D
    start=millis();
  ejeXgiroscopio();
  ejeYgiroscopio();
  ejeZgiroscopio();
    // Calculating dt
    finished=millis();
    elapsed=finished-start;
    dt=elapsed/1000.0;
    start = elapsed = 0;
  //--> MC5883L
  ajusteMagnetometro();

  //--> Media movil de la altura
  calculoMediaMovil();  
  //Registro de los datos en la micro SD
  registroDatos();
  numeroDatos = numeroDatos + 1;
}
