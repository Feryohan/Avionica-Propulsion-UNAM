//CICLO PRINCIPAL

void loop() {   
  int j=0;  
  archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
  if(archivo){
    while (j<20){
      //--> BMP085 
      temperature = bmp085GetTemperature(bmp085ReadUT()); 
      pressure = bmp085GetPressure(bmp085ReadUP());
      altitud(pressure, temperature);
      //--> ADXL345
      valoresAcelerometro();
      //--> L3G4200D
      valoresGiroscopio();
      //--> MC5883L
      valoresMagnetometro();
      //--> Media movil de la altura
      calculoMediaMovil();  
      //--> Checar nivel de la pila
      nivelPila();
   //--> Registro de los datos en la micro SD
      //registroDatos();
    archivo.print(numeroLectura);
    archivo.print(",");
    archivo.print(millis());
    archivo.print(",");
    archivo.print(alturaActual);
    archivo.print(",");
    archivo.print(aceleracionX);
    archivo.print(",");
    archivo.print(aceleracionY);
    archivo.print(",");
    archivo.print(aceleracionZ);
    archivo.print(",");
    archivo.print(magnetometroX);
    archivo.print(",");
    archivo.print(magnetometroY);
    archivo.print(",");
    archivo.print(magnetometroZ);
    archivo.print(",");
    archivo.print(giroscopioX);
    archivo.print(",");
    archivo.print(giroscopioY);
    archivo.print(",");
    archivo.print(giroscopioZ);
    archivo.print(",");
    archivo.println(Estado);
    numeroLectura = numeroLectura + 1;
    j=j+1;
    }
  }
  archivo.close();
}
