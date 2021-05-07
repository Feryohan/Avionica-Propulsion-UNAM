
void setup() {
  Serial.begin(9600);               //Comunicacion serial para debug
  Wire.begin();                     //Inicio de la comuniacion I2C       
  
  pinMode(Buzzer,OUTPUT);           //Buzzer es salida
  digitalWrite(Buzzer, LOW);        //Buzzer apagado

////////////////////////////////// Comunicaciones //////////////////////////////////////
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
   
  analogWrite(Buzzer, 200);
  delay(1000);
  analogWrite(Buzzer,LOW);
  delay(3000);
}
