#include <EEPROM.h>

byte uno = 1;
byte cero = 0;

byte addressVuelo = 0, addressMPU = 1, addressBarometro = 2, addressRTC = 3, addressGPS = 4, addressMicroSD = 5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
/*  EEPROM.put(addressVuelo,uno);
  EEPROM.put(addressMPU,uno);
  EEPROM.put(addressBarometro,uno);
  EEPROM.put(addressRTC,uno);
  EEPROM.put(addressGPS,uno);
  EEPROM.put(addressMicroSD,uno);*/

  Serial.print("Estado Vuelo             ");
  Serial.println(EEPROM.read(addressVuelo));
  Serial.print("Estado MPU               ");
  Serial.println(EEPROM.read(addressMPU));
  Serial.print("Estado Barometro         ");
  Serial.println(EEPROM.read(addressBarometro));
  Serial.print("Estado RTC               ");
  Serial.println(EEPROM.read(addressRTC));
  Serial.print("Estado GPS               ");
  Serial.println(EEPROM.read(addressGPS));
  Serial.print("Estado MicroSD           ");
  Serial.println(EEPROM.read(addressMicroSD));
}

void loop() {
  // put your main code here, to run repeatedly:

}
