//GND - GND
//VCC - VCC
//SDA - Pin A4
//SCL - Pin A5

#include "Wire.h"
#include "I2Cdev.h"
#include "HMC5883L.h"

HMC5883L compass;

int16_t mx, my, mz;

void setup() 
{
    
    Serial.begin(9600);
    Wire.begin();
    compass.initialize();
}

void loop()
{
    //Obtener componentes del campo magnético
    compass.getHeading(&mx, &my, &mz);
    
    Serial.print("mx:");
    Serial.print(mx); 
    Serial.print("tmy:");
    Serial.print(my);
    Serial.print("tmz:");
    Serial.println(mz);
    delay(100);
}
