void loop() {
  // put your main code here, to run repeatedly:
  valoresGiroscopio();
  valoresAcelerometro();
  Serial.print(aceleracionX);
  Serial.print(",");
  Serial.print(aceleracionY);
  Serial.print(",");
  Serial.print(aceleracionZ);
  Serial.print(",");
  Serial.print(giroscopioX);
  Serial.print(",");
  Serial.print(giroscopioY);
  Serial.print(",");
  Serial.println(giroscopioZ);
}
