#include <EEPROM.h>

byte uno = 1;
byte dos = 1;
byte tres = 1;
byte cuatro = 1;
byte cinco = 1;
byte seis = 1;
byte siete = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  EEPROM.put(1,uno);
  EEPROM.put(2,dos);
  EEPROM.put(3,tres);
  EEPROM.put(4,cuatro);
  EEPROM.put(5,cinco);
  EEPROM.put(6,seis);
  EEPROM.put(7,siete);

  Serial.println(EEPROM.read(0));
  Serial.println(EEPROM.read(1));
  Serial.println(EEPROM.read(2));
  Serial.println(EEPROM.read(3));
  Serial.println(EEPROM.read(4));
  Serial.println(EEPROM.read(5));
  Serial.println(EEPROM.read(6));
  Serial.println(EEPROM.read(7));
}

void loop() {
  // put your main code here, to run repeatedly:

}
