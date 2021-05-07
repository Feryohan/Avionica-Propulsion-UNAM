//FUNCIONES DE CONFIGURACION

void setup() 
{
  Serial.begin(9600);               //Comunicacion serial para debug
  Wire.begin();                     //Inicio de la comuniacion I2C       

  pinMode(Relay, OUTPUT);           //Relay es salida
  pinMode(Buzzer,OUTPUT);           //Buzzer es salida
  digitalWrite(Relay, LOW);         //Relay apagado
  digitalWrite(Buzzer, LOW);        //Buzzer apagado

  /////////////////////////// Modulos de Presion y SD /////////////////////////////
  //->Inicializacion del modulo micro SD
  if(!bmp180.begin())
  {
    Serial.println("Error en sensor de presion");
    while(1);                              //bucle infinito
  }
  //->Inicializacion del modulo micro SD
  if (!SD.begin(SSpin))                    //Si la tarjeta SD no inicia correctamente  
   {     
    Serial.println("Error en memoria SD");
    while(1);                              //El ciclo setup termina
   }
  Serial.println("Inicializacion correcta"); 
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
    Serial.println("Archivo listo");
  }
  //Si el archivo no se crea correctamente
  else {
    Serial.println("Error en apertura del archivo");
    while(1);  
  }
  t0 = millis();                       //Tiempo de salida de la configuración

  ////////////////////////////////// Comunicaciones //////////////////////////////////////
          //--------BMP085----------
  bmp085Calibration();
          //--------ADXL345---------
  //->Enable measurement
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(Power_Register);              //Power_CTL Register
  Wire.write(8);                           //Bit D3 High for measuring enable (0000 1000)
  Wire.endTransmission();
  //->Estableciendo DATA_FORMAT
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(0x31);
  Wire.endTransmission();
  Wire.requestFrom(ADXL345_ADDRESS,1);
  int DataFormat_data = Wire.read();            //Guardamos el valor por default
  DataFormat_data |= 0xb;                       //Activamos la aceleracion +-16g, manteniendo los demás valores iniciales
  //0xb = 0000 1011                             //FULL_RES en 1     Range en 11, +-16g
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(0x31);
  Wire.write(DataFormat_data);
  Wire.endTransmission();
  //->Estableciendo BW_RATE = Date rate and power mode control
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(0x2c);
  Wire.endTransmission();
  Wire.requestFrom(ADXL345_ADDRESS,1);
  DataFormat_data = Wire.read();                 //Guardamos el valor por default
  DataFormat_data |= 0xf;                        //Output Dara rate = 3200 [Hz]
  //0xF = 0000 1111                              //Bandwidth = 1600 [Hz]
  Wire.beginTransmission(ADXL345_ADDRESS);
  Wire.write(0x2c);
  Wire.write(DataFormat_data);
  Wire.endTransmission();
        //--------L3G4200D---------
  //->Estableciendo DATA RATE a 800 [Hz]; Cut off = 35
  Wire.beginTransmission(Gyro_L3G4200D_ADDRESS);
  Wire.write(0x20);                        //CTRL_REG1 - Power Mode
  Wire.write(0xDF);                        //0xDF - 1101 1111b   
  Wire.endTransmission();
  //->Estableciendo Sensitivity
  Wire.beginTransmission(Gyro_L3G4200D_ADDRESS);
  Wire.write(0x23);                        //CTRL_REG4 - Sensitivity, Scale Selection
  Wire.write(48);                          //2000dps: 48d - 00110000b
  Wire.endTransmission();   
        //--------MC5883L---------
   //->Estableciendo DATA RATE a 75 [Hz]
  Wire.beginTransmission(Magnet_MC5883L_ADDRESS);
  Wire.write(0x00);                        //Configuration Register A
  Wire.write(0xF8);                        //0xFF - 1111 1000b   
  Wire.endTransmission();
  //->Estableciendo Mode register
  Wire.beginTransmission(Magnet_MC5883L_ADDRESS); 
  Wire.write(0x02);                        //Select mode register
  Wire.write(0x00);                        //Continuous measurement mode
  Wire.endTransmission();
  for(int i = 0; i < 2; i++) {
      analogWrite(Buzzer, 150);
      delay(500);
      analogWrite(Buzzer, LOW);
      delay(500);
  }
}
