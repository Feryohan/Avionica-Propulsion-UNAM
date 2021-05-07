const int pBuzzer = 6;
const int contador = 0;

void setup() {
  // put your setup code here, to run once:
   pinMode(pBuzzer, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(pBuzzer, 150);
  delay(1000);
  analogWrite(pBuzzer, LOW);
  delay(1000);  
  if(contador == 10){
    break;  
  }
}
