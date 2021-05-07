//FUNCIONES DE CONFIGURACION

void setup() 
{
  Serial.begin(9600);               //Comunicacion serial para debug
  Wire.begin();                     //Inicio de la comuniacion I2C       

  pinMode(Relay, OUTPUT);           //Relay es salida
  pinMode(Buzzer,OUTPUT);           //Buzzer es salida
  digitalWrite(Relay, LOW);         //Relay apagado
  digitalWrite(Buzzer, LOW);        //Buzzer apagado
  
 
    ///////////////////////// Archivo en la micro SD /////////////////////////
  //->Inicializacion del modulo micro SD
  if (!SD.begin(SSpin))                    //Si la tarjeta SD no inicia correctamente  
   {     
    while(1);                                //El ciclo setup termina
   }
   //--------BMP085----------
  bmp085Calibration();
  //--------ADXL345---------
  //->Enable measurement
  wireTransmission(ADXL345_ADDRESS,Power_Register,8);
      //Power_CTL Register
      //Bit D3 High for measuring enable (0000 1000)
      
  analogWrite(Buzzer, 200);
  delay(1000);
  analogWrite(Buzzer,LOW);
  delay(3000);
  
  //->Leer y actualizar el numero de archivo
  nFile = EEPROM.read(0);                      //Usamos la dirección cero de la EEPROM
  EEPROM.write(0,nFile+1);                     //Tenemos un máximo de 255 archivos
  nFile = EEPROM.read(0);
  //->Crear archivo de datos de vuelo
  archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
  //Si el archivo se crea correctamente
  if (archivo) {
    archivo.println("#Lectura,Millis,Tiempo,Altura,AcelX,AcelY,AcelZ,MagX,MagY,MagZ,GyroX,GyroY,GyroZ,Estado,Longitud,Latitud,AlturaGPS");
    archivo.close();
    for(int i = 0; i < 2; i++) {
      analogWrite(Buzzer, 150);
      delay(500);
      analogWrite(Buzzer, LOW);
      delay(500);
    }
  }
  //Si el archivo no se crea correctamente
  else {
    while(1);  
  }
  t0 = millis();                       //Tiempo de salida de la configuración
}
