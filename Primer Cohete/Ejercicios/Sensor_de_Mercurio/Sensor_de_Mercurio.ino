int Sensor = 7;
int ESTADO;
int Buzzer = 6;

void setup() {
  // put your setup code here, to run once:
  pinMode(Sensor, INPUT);
  pinMode(Buzzer, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  ESTADO = digitalRead(Sensor);
  digitalWrite(Buzzer, ESTADO);
  Serial.print(ESTADO);
  delay(100);
}
