//FUNCIONES DE CONFIGURACION

void setup() 
{
  Serial.begin(9600);               //Comunicacion serial para debug
  Wire.begin();                     //Inicio de la comuniacion I2C       
  DEBUG_PORT.begin(9600);           //Comunicacion con el segundo puerto, el NeoSWSerial.h
  gpsPort.begin(9600);              
  
  pinMode(Relay, OUTPUT);           //Relay es salida
  pinMode(Buzzer,OUTPUT);           //Buzzer es salida
  analogWrite(Relay, LOW);          //Relay apagado
  analogWrite(Buzzer, LOW);         //Buzzer apagado
 
  //->Inicializacion del modulo micro SD
  if (!SD.begin(SSpin))                    //Si la tarjeta SD no inicia correctamente  
   {     
    while(1){
      analogWrite(Buzzer, 250);
      delay(100);
    }
   }
  //--------BMP085----------
  bmp085Calibration();
  //--------ADXL345---------
  //->Enable measurement
  wireTransmission(ADXL345_ADDRESS,Power_Register,8);
      //Power_CTL Register
      //Bit D3 High for measuring enable (0000 1000)
      
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
      analogWrite(Buzzer, 200);
      delay(500);
      analogWrite(Buzzer, LOW);
      delay(250);
      analogWrite(Buzzer, 200);
      delay(500);
      analogWrite(Buzzer, LOW);
      delay(250);
      analogWrite(Buzzer, 200);
      delay(1000);
      analogWrite(Buzzer, LOW);
  }
  //Si el archivo no se crea correctamente
  else {
    while(1){
      analogWrite(Buzzer, 250);
      delay(100);
      analogWrite(Buzzer, LOW);
      delay(100);
    }
  }
 t0 = millis();                       //Tiempo de salida de la configuración
}
