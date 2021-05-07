

// FUNCION DE CONFIGURACION

void setup()
{
  
  Serial.begin(9600);   //Comunicacion serial para debug

  Serial.println("HOLA");
  
  //  -----Iniciar modulo de presion y SD-----
  if (!bmp180.begin())
  {
      Serial.println("Error en sensor de presión");
    while(1); // bucle infinito
  }
  
  if (!SD.begin(chipSelect)) {
      Serial.println("Error en memoria SD");
    while(1);
  }
  //  -----Fin de inicializacion de modulos-----

  //  -----Leer y actualizar el numero de archivo-----
  nFile = EEPROM.read(0);   // Usamos la direccion cero de la EEPROM
  EEPROM.write(0,nFile+1);  // Tenemos un maximo de 255 archivos
  nFile = EEPROM.read(0);
  //  ----------Fin----------
  Serial.println("HOLA");
  calibracion();          // Funcion para calibrar la altura inicial

  //  -----Crear archivo de datos de vuelo-----
  Datos = SD.open("Datos" + String(nFile) + ".txt",FILE_WRITE);
  if(Datos) {
    n = 0;
    Datos.println("Numero,Tiempo_ms,Temperatura_C,Presion_Pa,Altura_m");
    Datos.close();
    digitalWrite(LEDR,LOW);
      Serial.println("Listo archivo!");
    digitalWrite(LEDR,HIGH);
    delay(500);
    digitalWrite(LEDR,LOW); 
  } 
  else 
  {
    Serial.println("Error al crear archivo de datos!");
    while(1);
  }//  ----------Fin----------
  Serial.println("HOLA");
  t0 = millis(); // Tiempo de salida de la configuracion
}
