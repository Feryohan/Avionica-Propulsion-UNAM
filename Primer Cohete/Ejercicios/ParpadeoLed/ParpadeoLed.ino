int LED = 2;

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}
  
void loop() {
  //Se envían 5 volts al pin 2
  digitalWrite(LED,HIGH);
  //Durante medio segundo espera
  delay(500);
  digitalWrite(LED,LOW);
  delay(500);
  }
