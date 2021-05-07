/* MPU6050 Basic Example Code
 Demonstrate  MPU-6050 basic functionality including initialization, accelerometer trimming, sleep mode functionality as well as
 parameterizing the register addresses. Added display functions to allow display to on breadboard monitor. 
 No DMP use. We just want to get out the accelerations, temperature, and gyro readings.
 
 SDA and SCL should have external pull-up resistors (to 3.3V).
 10k resistors worked for me. They should be on the breakout
 board.
*/
 
#include <Wire.h>
#include "MPU6050.h"

// Pin definitions
int16_t accelCount[3];           // Stores the 16-bit signed accelerometer sensor output
float ax, ay, az;                // Stores the real accel value in g's
int16_t gyroCount[3];            // Stores the 16-bit signed gyro sensor output
float gx, gy, gz;                // Stores the real gyro value in degrees per seconds
float gyroBias[3], accelBias[3]; // Bias corrections for gyro and accelerometer
int16_t tempCount;               // Stores the internal chip temperature sensor output 
float temperature;               // Scaled temperature in degrees Celsius
float SelfTest[6];               // Gyro and accelerometer self-test sensor output
uint32_t count = 0;
float aRes, gRes; // scale resolutions per LSB for the sensors
MPU6050lib mpu;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  
  mpu.MPU6050SelfTest(SelfTest); // Start by performing self test and reporting values
  Serial.print("x-axis self test: acceleration trim within : "); Serial.print(SelfTest[0],1); Serial.println("% of factory value");
  Serial.print("y-axis self test: acceleration trim within : "); Serial.print(SelfTest[1],1); Serial.println("% of factory value");
  Serial.print("z-axis self test: acceleration trim within : "); Serial.print(SelfTest[2],1); Serial.println("% of factory value");
  Serial.print("x-axis self test: gyration trim within : "); Serial.print(SelfTest[3],1); Serial.println("% of factory value");
  Serial.print("y-axis self test: gyration trim within : "); Serial.print(SelfTest[4],1); Serial.println("% of factory value");
  Serial.print("z-axis self test: gyration trim within : "); Serial.print(SelfTest[5],1); Serial.println("% of factory value");

  if(SelfTest[0] < 1.0f && SelfTest[1] < 1.0f && SelfTest[2] < 1.0f && SelfTest[3] < 1.0f && SelfTest[4] < 1.0f && SelfTest[5] < 1.0f) {
    Serial.println("Pass Selftest!");    
    mpu.calibrateMPU6050(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers  
    mpu.initMPU6050(); Serial.println("MPU6050 initialized for active data mode...."); // Initialize device for active mode read of acclerometer, gyroscope, and temperature
    }
  else
    {
    Serial.print("Could not connect to MPU6050: 0x");
    while(1) ; // Loop forever if communication doesn't happen
    }
}

void loop()
{  
  // If data ready bit set, all data registers have new data
  if(mpu.readByte(MPU6050_ADDRESS, INT_STATUS) & 0x01) {  // check if data ready interrupt

    mpu.readAccelData(accelCount);  // Read the x/y/z adc values
    aRes=mpu.getAres();
    
    // Now we'll calculate the accleration value into actual g's
    ax = (float)accelCount[0]*aRes - accelBias[0];  // get actual g value, this depends on scale being set
    ay = (float)accelCount[1]*aRes - accelBias[1];   
    az = (float)accelCount[2]*aRes - accelBias[2];  
   
    mpu.readGyroData(gyroCount);  // Read the x/y/z adc values
    gRes=mpu.getGres();
 
    // Calculate the gyro value into actual degrees per second
    gx = (float)gyroCount[0]*gRes - gyroBias[0];  // get actual gyro value, this depends on scale being set
    gy = (float)gyroCount[1]*gRes - gyroBias[1];  
    gz = (float)gyroCount[2]*gRes - gyroBias[2];   

    tempCount = mpu.readTempData();  // Read the x/y/z adc values
    temperature = ((float) tempCount) / 340. + 36.53; // Temperature in degrees Centigrade
   }  
   
    uint32_t deltat = millis() - count;
    if(deltat > 500) {
 
    // Print acceleration values in milligs!
    Serial.print("X-acceleration: "); Serial.print(1000*ax); Serial.print(" mg "); 
    Serial.print("Y-acceleration: "); Serial.print(1000*ay); Serial.print(" mg "); 
    Serial.print("Z-acceleration: "); Serial.print(1000*az); Serial.println(" mg"); 
 
    // Print gyro values in degree/sec
    Serial.print("X-gyro rate: "); Serial.print(gx, 1); Serial.print(" degrees/sec "); 
    Serial.print("Y-gyro rate: "); Serial.print(gy, 1); Serial.print(" degrees/sec "); 
    Serial.print("Z-gyro rate: "); Serial.print(gz, 1); Serial.println(" degrees/sec"); 
    
   // Print temperature in degrees Centigrade      
    Serial.print("Temperature is ");  Serial.print(temperature, 2);  Serial.println(" degrees C"); // Print T values to tenths of s degree C
    Serial.println("");
        
    count = millis();
    }

}
