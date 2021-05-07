void setup() {
  Serial.begin(9600);
  Wire.begin();
  magnetometro.initialize();
  mpu.initialize();
  rtc.adjust(DateTime(__DATE__, __TIME__));  // funcion que permite establecer fecha y horario
  tiempo1 = millis();
  
  //->Inicializacion del modulo micro SD
/*  if (!SD.begin(SSpin))     //Si la tarjeta SD no inicia correctamente  
   {
    Serial.print("La tarjeta SD no se inicio correctamente");      
    while(1){
    }
   }
   
  //->Leer y actualizar el numero de archivo
  nFile = EEPROM.read(0);          //Usamos la dirección cero de la EEPROM
  EEPROM.write(0,nFile+1);         //Tenemos un máximo de 255 archivos
  nFile = EEPROM.read(0);
  //->Crear archivo de datos de vuelo
  archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
  //Si el archivo se crea correctamente
  if (archivo) {
    archivo.println("MagX,MagY,MagZ,AcelX,AcelY,AcelZ,GyroX,GyroY,GyroZ");
    archivo.close();
  }
  //Si el archivo no se crea correctamente
  else {
    Serial.print("El archivo no se creo correctamente");
    while(1){
    }
  }*/
  Serial.println("MagX,MagY,MagZ,AcelX,AcelY,AcelZ,GyroX,GyroY,GyroZ");
}
