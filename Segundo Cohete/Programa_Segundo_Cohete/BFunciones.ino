void wireConfiguration(int dispositivo, int direccion, int valor)
{
  Wire.beginTransmission(dispositivo);
  Wire.write(direccion);
  Wire.endTransmission();
  Wire.requestFrom(dispositivo,1);
  int DataFormat_data = Wire.read();            //Guardamos el valor por default
  DataFormat_data |= valor;                     //Activamos el valor que ingresamos, manteniendo los demás valores iniciales
  wireTransmission(dispositivo,direccion,DataFormat_data); 
}

void wireTransmission(int dispositivo,int registro0,int registro1)
{
  Wire.beginTransmission(dispositivo);
  Wire.write(registro0);              
  Wire.write(registro1); 
  Wire.endTransmission();
  Wire.requestFrom(dispositivo,2);
}

void wireTransmissionSimple(int dispositivo,int registro)
{
  Wire.beginTransmission(dispositivo);
  Wire.write(registro);    
  Wire.endTransmission();
  Wire.requestFrom(dispositivo,1);
}

void estadoSensor(byte estado, byte address){
  EEPROM.write(address, estado);
}

void numeroArchivo(){
  nFile = EEPROM.read(500);          //Usamos la dirección 500 de la EEPROM
  EEPROM.write(500,nFile+1);         //Tenemos un máximo de 255 archivos
  nFile = EEPROM.read(500);
}

void iniciarArchivo(){
  estadoSensor(0, archivoMemoriaSD);
  archivo = SD.open("Datos"+String(nFile)+".txt",FILE_WRITE);
    if(archivo){
      //Si el archivo se crea correctamente
      archivo.println("AcelX,AcelY,AcelZ,GyroX,GyroY,GyroZ,Latitud,Longitud,Altitud,Hora");
      archivo.close();
      //wdt_reset();
      estadoSensor(1, archivoMemoriaSD);
      Serial.println("Archivo creado");
    }
    else{
      Serial.println("Archivo NO creado");  
    }
}

//- - - Configuración - - -

//Cálculo de los valores del acelerometro
float calculoAceleracion(int dispositivo,int registro0,int registro1){
  float aceleracion = 0;
  wireTransmission(dispositivo,registro0,registro1);
  if(Wire.available()<=2){
    int raw0 = Wire.read();
    int raw1 = Wire.read();
    raw1 = raw1<<8;
    int raw_out = raw0 + raw1;
    aceleracion = raw_out/AceleracionXBytes;
  }
  return aceleracion;
}

void valoresAcelerometro(){
  if(EEPROM.read(datosAcelerometro) == 1){
    estadoSensor(0, datosAcelerometro);
    aceleracionX = calculoAceleracion(MPU6050_ADDRESS,ACCEL_XOUT_H,ACCEL_XOUT_L);
    aceleracionY = calculoAceleracion(MPU6050_ADDRESS,ACCEL_YOUT_H,ACCEL_YOUT_L);
    aceleracionZ = calculoAceleracion(MPU6050_ADDRESS,ACCEL_ZOUT_H,ACCEL_ZOUT_L);
    estadoSensor(1, datosAcelerometro);
  }
}


float calculoGiroscopio(int dispositivo,int registro0,int registro1){
  float giroscopio = 0;
  int raw0,raw1;
  wireTransmissionSimple(dispositivo,registro0);
  if(Wire.available()<=1)   
  {
    raw0 = Wire.read();
  }
  wireTransmissionSimple(dispositivo,registro1);
  if(Wire.available()<=1)   
  {
    raw1 = Wire.read();
  }
  // Raw Data
  raw1 = raw1<<8;
  int raw_out = raw0 + raw1;
  giroscopio = raw_out*dpsXDigit;
}

void valoresGiroscopio(){
  if(EEPROM.read(datosGiroscopio) == 1){
    estadoSensor(0, datosGiroscopio);
    giroscopioX = calculoGiroscopio(MPU6050_ADDRESS,GYRO_XOUT_H,GYRO_XOUT_L);
    giroscopioY = calculoGiroscopio(MPU6050_ADDRESS,GYRO_YOUT_H,GYRO_YOUT_L);
    giroscopioZ = calculoGiroscopio(MPU6050_ADDRESS,GYRO_ZOUT_H,GYRO_ZOUT_L);
    estadoSensor(1, datosGiroscopio);
  }
}

// - - - Ciclo - - -
/*
void obtenerDatosGPS(){
long lon;     //Longitud deg e-7
long lat;     //latitude deg e-7
long hMSL;    // altura Nivel del Mar mm

if(serial.available()){
  //Revisar que 
  Array[0]=Array[1];
  Array[1]=Array[2];
  Array[2]=Array[3];
  Array[3]=serial.read();

  //Serial.print(Array[3], HEX);
  //Serial.print(" ");
  //Revisar header de la trama, para indicar que incio el mensaje
  if(Array[0]==UBX_HEADER[0] && Array[1]==UBX_HEADER[1] && Array[2]==UBX_HEADER[2] && Array[3]==UBX_HEADER[3]){
    pos=1;
    lon=0;lat=0;hMSL=0;
  }else{
    pos++;  
          if (pos<=8 && 4<pos){     //Bytes del 5 al 8
      if (pos-4<=1)lon=+Array[3]*16*16*16;
      if (pos-4<=2)lon=+Array[3]*16*16;
      if (pos-4<=3)lon=+Array[3]*16;
      if (pos-4<=4)lon=+Array[3];
    }else if (pos<=12 && 8<pos){  //Bytes del 9 al 12
      if (pos-8<=1)lat=+Array[3]*16*16*16;
      if (pos-8<=2)lat=+Array[3]*16*16;
      if (pos-8<=3)lat=+Array[3]*16;
      if (pos-8<=4)lat=+Array[3];
    }else if (pos<=20 && 16<pos){ //Bytes del 17 al 20
      if (pos-16<=1)hMSL=+Array[3]*16*16*16;
      if (pos-16<=2)hMSL=+Array[3]*16*16;
      if (pos-16<=3)hMSL=+Array[3]*16;
      if (pos-16<=4)hMSL=+Array[3];
    }else if(pos==33){
      serial.println("");
      serial.print("lon=");
      serial.print(lon);
      serial.print(" lat=");
      serial.print(lat);
      serial.print(" hMSL=");
      serial.println(hMSL);
          
    }
  }
 }
}
*/


void obtenerDatosRTC(){
  tiempo2 = millis();
  if(tiempo2 > (tiempo1+1000)){
    tiempo1 = millis();
    if(EEPROM.read(datosRTC) == 1){
      estadoSensor(0,datosRTC);
      DateTime dia = rtc.now();
      fecha = dia.day() + diagonal + dia.month() + diagonal + dia.hour() + dosPuntos + dia.minute() + dosPuntos + dia.second();
      estadoSensor(1,datosRTC);
    } 
    else{
      Serial.println("Sin RTC");
      fecha = "sin fecha";
    }
  }
}

void escribirDatos(){
  archivo.print(aceleracionX);
  archivo.print(",");
  archivo.print(aceleracionY);
  archivo.print(",");
  archivo.print(aceleracionZ);
  archivo.print(",");
  archivo.print(giroscopioX);
  archivo.print(",");
  archivo.print(giroscopioY);
  archivo.print(",");
  archivo.print(giroscopioZ);
  archivo.print(",");
  archivo.print("latitud");
  archivo.print(",");
  archivo.print("longitude");
  archivo.print(",");
  archivo.print("altitude");
  archivo.print(",");
  archivo.println(fecha);
}
