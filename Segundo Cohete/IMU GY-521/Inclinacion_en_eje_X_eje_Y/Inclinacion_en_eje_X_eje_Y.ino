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

void setup()
{
   Serial.begin(9600);
   Wire.begin();
   mpu.initialize();
   Serial.println(mpu.testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU"));
}

void loop() 
{
   // Leer las aceleraciones 
   mpu.getAcceleration(&ax, &ay, &az);

   //Calcular los angulos de inclinacion
   float accel_ang_x = atan(ax / sqrt(pow(ay, 2) + pow(az, 2)))*(180.0 / 3.14);
   float accel_ang_y = atan(ay / sqrt(pow(ax, 2) + pow(az, 2)))*(180.0 / 3.14);

   // Mostrar resultados
   Serial.print(F("Inclinacion en X: "));
   Serial.print(accel_ang_x);
   Serial.print(F("\tInclinacion en Y:"));
   Serial.println(accel_ang_y);
   delay(10);
}
