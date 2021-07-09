#include <Wire.h>        //Libreria para la comunicacion I2C

//--- VARIABLES ---
//-> MPU
#define MPU6050_ADDRESS 0x69        //Direccion del MPU cuando A0 recibe voltaje
//Si no recibe voltaje, la dirección seria 0x68 pero esto entra en conflicto con el RTC

#define SMPRT_DIV       0x19
//Acelerometro
#define CONFIG          0x1A       //Gyroscope Output Rate = 8kHz when the DLPF is disabled (DLPF_CFG = 0 or 7), and 1kHz
#define DLPF_CFG        0x3        //when the DLPF is enabled (see Register 26).
                                   //Pagina 12: Note: The accelerometer output rate is 1kHz
//Accelerometro -> Bandwidth(44 Hz), Delay(4.90 ms), Frecuencia de muestreo (1kHz)
//Giroscopio -> Bandwidth(42 Hz), Delay(4.8 ms), Frecuencia de muestreo (1kHz)

#define GYRO_CONFIG   0x1B        //Pagina 14
#define FS_SEL        0x18          //Se escoge la escala completa del giroscopio
                                  //la cual es de +-2000 º/s, mandando bit4: 1, bit3: 1
#define ACCEL_CONFIG  0x1C        //Pagina 15
#define AFS_SEL       0x18        //Se escoge la escala completa del acelerometro
                                  //la cual es de +-16 g, mandando bit4: 1, bit3: 1


#define ACCEL_XOUT_H  0x3B
#define ACCEL_XOUT_L  0x3C
#define ACCEL_YOUT_H  0x3D
#define ACCEL_YOUT_L  0x3E
#define ACCEL_ZOUT_H  0x3F
#define ACCEL_ZOUT_L  0x40
//Giroscopio
#define GYRO_XOUT_H   0x43
#define GYRO_XOUT_L   0x44
#define GYRO_YOUT_H   0x45
#define GYRO_YOUT_L   0x46
#define GYRO_ZOUT_H   0x47
#define GYRO_ZOUT_L   0x48

float aceleracionX,aceleracionY,aceleracionZ;
//output value from -1 to +1, Gravity acceleration acting on the X-Axis
//2^8 bytes
float AceleracionXBytes = 256;
        //--------L3G4200D---------
float giroscopioX,giroscopioY,giroscopioZ;
// From the datasheet: 70 mdps/digit
float dpsXDigit = 0.07;      //Angular rate [dps]
