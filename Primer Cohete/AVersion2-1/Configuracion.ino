//FUNCIONES DE CONFIGURACION

void setup() 
{
  Serial.begin(9600);               //Comunicacion serial para debug
  Wire.begin();                     //Inicio de la comuniacion I2C       

  pinMode(Relay, OUTPUT);           //Relay es salida
  pinMode(Buzzer,OUTPUT);           //Buzzer es salida
  digitalWrite(Relay, LOW);         //Relay apagado
  digitalWrite(Buzzer, LOW);        //Buzzer apagado
  
  ////////////////////////////////// Comunicaciones //////////////////////////////////////
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
  //->Estableciendo DATA RATE a 800 [Hz]; Cut off = 35
  wireTransmission(Gyro_L3G4200D_ADDRESS,0x20,0xDF);
      //CTRL_REG1 - Power Mode
      //0xDF - 1101 1111b   
  //->Estableciendo Sensitivity
  wireTransmission(Gyro_L3G4200D_ADDRESS,0x23,48);
      //CTRL_REG4 - Sensitivity, Scale Selection
      //2000dps: 48d - 00110000b
        //--------MC5883L---------
  //->Estableciendo DATA RATE a 75 [Hz]
  wireTransmission(Magnet_MC5883L_ADDRESS,0x00,0xF8);
      //Configuration Register A
      //0xFF - 1111 1000b   
  //->Estableciendo Mode register
  wireTransmission(Magnet_MC5883L_ADDRESS,0x02,0x00);
      //Select mode register
      //Continuous measurement mode
  
    ///////////////////////// Archivo en la micro SD /////////////////////////
  //->Inicializacion del modulo micro SD
  if (!SD.begin(SSpin))                    //Si la tarjeta SD no inicia correctamente  
   {     
    while(1);                                //El ciclo setup termina
   }
  
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
    archivo.println("Dato,Millis,Temp,Presion,AejeX,AejeY,AejeZ,GejeX,GejeY,GejeZ,MejeX,MejeY,MejeZ,Altura,AltMediaMovil,Pendiente");
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
