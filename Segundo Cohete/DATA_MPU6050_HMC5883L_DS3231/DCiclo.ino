void loop() {
  int j = 0;
  archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
  if(archivo){
    while(j<20){
      //Obteniendo datos raw del magnetometro
      magnetometro.getHeading(&mx, &my, &mz);
      //Leer los datos del MPU
      MPUGetData();

      tiempo2 = millis();
      //Esta condición se cumple cada segundo que transcurre
      if(tiempo2 > (tiempo1+1000)){
        tiempo1 = millis();
        DateTime dia = rtc.now();
        fecha = dia.day() + diagonal + dia.month() + diagonal + dia.hour() + dosPuntos + dia.minute() + dosPuntos + dia.second();
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
