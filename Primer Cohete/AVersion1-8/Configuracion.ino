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
  
    ///////////////////////// Archivo en la micro SD /////////////////////////
  //->Inicializacion del modulo micro SD
  if (!SD.begin(SSpin))                    //Si la tarjeta SD no inicia correctamente  
   {     
    Serial.println("Fallo en inicializacion !");
    while(1);                                //El ciclo setup termina
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
    numeroDatos = 0;
    archivo.println("Dato,Millis,Temp,Presion,AejeX,AejeY,AejeZ,GejeX,GejeY,GejeZ,MejeX,MejeY,MejeZ,Altura,AltMediaMovil,Pendiente");
    archivo.close();
    Serial.println("Archivo listo");
    for(int i = 0; i < 2; i++) {
      analogWrite(Buzzer, 150);
      delay(500);
      analogWrite(Buzzer, LOW);
      delay(500);
    }
  }
  //Si el archivo no se crea correctamente
  else {
    Serial.println("Error en apertura del archivo");
    while(1);  
  }
  t0 = millis();                       //Tiempo de salida de la configuración
  
    for (int i = 1; i < 141; i++){
      //--> BMP085 
      temperature = bmp085GetTemperature(bmp085ReadUT()); 
      pressure = bmp085GetPressure(bmp085ReadUP());
      altura = altitud(pressure, temperature);
      //--> ADXL345
      aceleracionX = ejeXacelerometro();
      aceleracionY = ejeYacelerometro();
      aceleracionZ = ejeZacelerometro();
      //--> L3G4200D
        start=millis();
      giroscopioX = ejeXgiroscopio();
      giroscopioY = ejeYgiroscopio();
      giroscopioZ = ejeZgiroscopio();
        // Calculating dt
        finished=millis();
        elapsed=finished-start;
        dt=elapsed/1000.0;
        start = elapsed = 0;
      //--> MC5883L
      magnetometroX = ejeXmagnetometro();
      magnetometroY = ejeYmagnetometro();
      magnetometroZ = ejeZmagnetometro();

      //--> Escritura de las ultimas 8 alturas
      if(numeroDatos == 1){
          altura1 = altura;
      }
      if(numeroDatos == 2){
          altura2 = altura;
      }
      if(numeroDatos == 3){
          altura3 = altura;
      }
      if(numeroDatos == 4){
          altura4 = altura;
      }
      if(numeroDatos == 5){
          altura5 = altura;
      }
      if(numeroDatos == 6){
          altura6 = altura;
      }
      if(numeroDatos == 7){
          altura7 = altura;
      }
      if(numeroDatos == 8){
          altura8 = altura;
      }
      //--> Ultimos 2 valores de la altura media movil
      float AMM1 = alturaMediaMovil;
      float AMM2;
      
      if(numeroDatos > 8){
        //Promedio de las ultimas 8 alturas
        alturaMediaMovil = round((altura1+altura2+altura3+altura4+altura5+altura6+altura7+altura8)/(8));
        AMM2 = alturaMediaMovil;
        //Se van registrando las ultimas 8 alturas que sean calculadas
        altura1 = altura2;
        altura2 = altura3;
        altura3 = altura4;
        altura4 = altura5;
        altura5 = altura6;
        altura6 = altura7;
        altura7 = altura8;
        altura8 = altura;
      }
      //Calculo de la pendiente de las alturas medias moviles
      if(numeroDatos > 9){
        alturaPendiente = (AMM2 - AMM1);
      }
      numeroDatos = numeroDatos + 1;
      //Registro de los datos en la micro SD
      Serial.println(i);
      registroDatos();
    }
    
    //El relevador se activa por 2 segundos       
    digitalWrite(Relay, HIGH);
    Serial.println("---> Escritura correcta"); 
    delay(2000);
    digitalWrite(Relay, LOW);
}
