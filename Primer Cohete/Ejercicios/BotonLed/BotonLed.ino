
int PULSADOR = 2;
int LED = 3;
int estado = LOW;

void setup() {
  // put your setup code here, to run once:
pinMode(PULSADOR, INPUT);
pinMode(LED, OUTPUT);
digitalWrite(LED,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Se repite mientras que lo que entre por el pin 2 
  //sean 0 volts
  while(digitalRead(PULSADOR)==LOW);
  estado = digitalRead(LED);
  //Negamos el estado, mandando el valor
  //contrario del estado
  digitalWrite(LED, !estado);
  while(digitalRead(PULSADOR)==HIGH);
}
