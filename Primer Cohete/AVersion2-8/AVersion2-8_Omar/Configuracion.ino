//FUNCIONES DE CONFIGURACION

void setup() 
{
  Serial.begin(9600);               //Comunicacion serial para debug
  Serial.println("inciando");
  Wire.begin();                     //Inicio de la comuniacion I2C       
  DEBUG_PORT.begin(9600);           //Comunicacion con el segundo puerto, el NeoSWSerial.h
  gpsPort.begin(9600);              
  
  pinMode(Relay, OUTPUT);           //Relay es salida
  pinMode(Buzzer,OUTPUT);           //Buzzer es salida
  digitalWrite(Relay, LOW);          //Relay apagado, DEBE DECIR DIGITALWRITE
  analogWrite(Buzzer, LOW);         //Buzzer apagado
 
  //->Inicializacion del modulo micro SD
  if (!SD.begin(SSpin))                    //Si la tarjeta SD no inicia correctamente  
   {     
    while(1){
      analogWrite(Buzzer, 1500);
      delay(100);
      Serial.println("No leyo la memria");
    }
   }
  //****************************CALIBRACION******************************
  //--------BMP085----------
  bmp085Calibration();
  //--------ADXL345---------
  //->Enable measurement
  wireTransmission(ADXL345_ADDRESS,Power_Register,8);
      //Power_CTL Register
      //Bit D3 High for measuring enable (0000 1000)
  //->Estableciendo DATA_FORMAT
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(0x31);
  Wire.endTransmission();
  Wire.requestFrom(ADXL345_ADDRESS,1);
  int DataFormat_data = Wire.read();            //Guardamos el valor por default
  DataFormat_data |= 0xb;                       //Activamos la aceleracion +-16g, manteniendo los demás valores iniciales
  //0xb = 0000 1011                             //FULL_RES en 1     Range en 11, +-16g
  wireTransmission(ADXL345_ADDRESS,0x31,DataFormat_data);
  //->Estableciendo BW_RATE = Date rate and power mode control
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(0x2c);
  Wire.endTransmission();
  Wire.requestFrom(ADXL345_ADDRESS,1);
  DataFormat_data = Wire.read();                 //Guardamos el valor por default
  DataFormat_data |= 0xf;                        //Output Dara rate = 3200 [Hz]
  //0xF = 0000 1111                              //Bandwidth = 1600 [Hz]
  wireTransmission(ADXL345_ADDRESS,0x2c,DataFormat_data);
        //--------L3G4200D---------
  //http://www.robotpark.com/image/data/PRO/91459/CD00265057.pdf
  //->Estableciendo DATA RATE a 100 [Hz]; Cut off = 25
  //Modo normal
  wireTransmission(Gyro_L3G4200D_ADDRESS,0x20,0x1F);
      //CTRL_REG1 - Power Mode
      //0x1F - 0001 1111
      //pagina 29   
  //->Estableciendo Sensitivity
  wireTransmission(Gyro_L3G4200D_ADDRESS,0x23,48);
      //CTRL_REG4 - Sensitivity, Scale Selection
      //2000dps: 48d - 00110000b
      //pagina 32
      
        //--------MC5883L---------
  //http://www.robotpark.com/image/data/PRO/91459/HMC5883L_3-Axis_Digital_Compass_IC.pdf
  //->Estableciendo DATA RATE a 15 [Hz]
  wireTransmission(Magnet_MC5883L_ADDRESS,0x00,0x10);
      //Configuration Register A 
      //0001 0000 = 0x10
      //pagina 12
  //->Estableciendo rango de +- 0.88 Ga
  wireTransmission(Magnet_MC5883L_ADDRESS,0x01,0x00);
      //Configuration Register B 
      //0000 0000 = 0x00
      //pagina 13
  //->Estableciendo Mode register
  wireTransmission(Magnet_MC5883L_ADDRESS,0x02,0x00);
      //Select mode register
      //Continuous measurement mode
      //pagina 14
      
  //->Leer y actualizar el numero de archivo
  //nFile = EEPROM.read(0);                      //Usamos la dirección cero de la EEPROM
  //Serial.println(nFile);
  //EEPROM.write(0,nFile+1);                     //Tenemos un máximo de 255 archivos
  //nFile = EEPROM.read(0);

  while(SD.exists("Datos"+String(nFile)+".txt")){
    nFile++;
  }
  Serial.println("Datos"+String(nFile)+".txt");

  //->Crear archivo de datos de vuelo
  archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
  //Si el archivo se crea correctamente
  if (archivo) {
    archivo.println("NLectura,Millis,Tiempo,Altura,AcelX,AcelY,AcelZ,MagX,MagY,MagZ,GyroX,GyroY,GyroZ,Estado,Longitud,Latitud,AlturaGPS");
    archivo.close();
      analogWrite(Buzzer, 1500);
      delay(500);
      analogWrite(Buzzer, LOW);
      delay(250);
      analogWrite(Buzzer, 1500);
      delay(500);
      analogWrite(Buzzer, LOW);
      delay(250);
      analogWrite(Buzzer, 1500);
      delay(1000);
      analogWrite(Buzzer, LOW);
  }
  //Si el archivo no se crea correctamente
  else {
    while(1){
      analogWrite(Buzzer, 1500);
      delay(100);
      analogWrite(Buzzer, LOW);
      delay(100);
      Serial.println("No se creo correctamente");
    }
  }
 t0 = millis();                       //Tiempo de salida de la configuración
}
