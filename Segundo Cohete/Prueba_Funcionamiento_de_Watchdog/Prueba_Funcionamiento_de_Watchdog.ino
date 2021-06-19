#include <avr/wdt.h>

void setup() {
wdt_disable(); // deshabilito el watchdog
Serial.begin(9600);
Serial.println("Setup..");

Serial.println("Wait 5 sec..");
delay(5000); // espero 5 segundos, para dar tiempo a cargar un nuevo sketch.
wdt_enable (WDTO_8S); // habilito el watchdog cada 8 segundos
Serial.println("Watchdog enabled.");
}

int timer = 0;

void loop(){
// Cada 1 segundo se incrementara un contador (que sera mostrado por el monitor serial) y parpadeara el led
if(!(millis()%1000)){
timer++;
Serial.println(timer);
digitalWrite(13, digitalRead(13)==1?0:1); delay(1);
}
//  wdt_reset(); // si se llamara a esta funcion todo andaria correctamente, ya que se resetearia el watchdog.
// Está remarcada para que se resetee el arduino a los 8 segundos y veamos que pasa.
}
