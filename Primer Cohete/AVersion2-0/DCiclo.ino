//CICLO PRINCIPAL

void loop() {
  medirPresion();
  altura = altitud();
  //--> ADXL345
  aceleracionX = ejeXacelerometro();
  aceleracionY = ejeYacelerometro();
  aceleracionZ = ejeZacelerometro();
  //--> L3G4200D
    start=millis();
  giroscopioX = ejeXgiroscopio();
  giroscopioY = ejeYgiroscopio();
  giroscopioZ = ejeZgiroscopio();
    // Calculating dt
    finished=millis();
    elapsed=finished-start;
    dt=elapsed/1000.0;
    start = elapsed = 0;
  //--> MC5883L
  magnetometroX = ejeXmagnetometro();
  magnetometroY = ejeYmagnetometro();
  magnetometroZ = ejeZmagnetometro();

  //--> Media movil de la altura
  calculoMediaMovil();  
  //Registro de los datos en la micro SD
  registroDatos();
  Serial.println(numeroDatos);
  numeroDatos = numeroDatos + 1;
}
