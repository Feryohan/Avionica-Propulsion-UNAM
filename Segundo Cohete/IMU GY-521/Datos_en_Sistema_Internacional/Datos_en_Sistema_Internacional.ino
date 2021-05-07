//GND - GND
//VCC - VCC
//SDA - Pin A4
//SCL - Pin A5
 
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
 
const int mpuAddress = 0x68;  // Puede ser 0x68 o 0x69
MPU6050 mpu(mpuAddress);
 
int ax, ay, az;
int gx, gy, gz;
 
 
// Factores de conversion
const float accScale = 2.0 * 9.81 / 32768.0;
const float gyroScale = 250.0 / 32768.0;
 
void printTab()
{
   Serial.print(F("\t"));
}
 
// Mostrar medidas en Sistema Internacional
void printRAW()
{
   Serial.print(F("a[x y z](m/s2) g[x y z](deg/s):t"));
   Serial.print(ax * accScale); printTab();
   Serial.print(ay * accScale); printTab();
   Serial.print(az * accScale); printTab();
   Serial.print(gx * gyroScale);  printTab();
   Serial.print(gy * gyroScale);  printTab();
   Serial.println(gz * gyroScale);
}
 
void setup()
{
   Serial.begin(9600);
   Wire.begin();
   mpu.initialize();
   Serial.println(mpu.testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU"));
}
 
void loop()
{
   // Leer las aceleraciones y velocidades angulares
   mpu.getAcceleration(&ax, &ay, &az);
   mpu.getRotation(&gx, &gy, &gz);
 
   printRAW();
 
   delay(100);
}
