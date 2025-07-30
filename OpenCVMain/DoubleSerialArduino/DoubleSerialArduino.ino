String x;
float step = 0;
#define LED 13
#define LEDSERIAL 2
void setup() {
  pinMode(LED, OUTPUT);
  pinMode(LEDSERIAL, OUTPUT);
  Serial.begin(500000);
  Serial.setTimeout(20);
}

void loop() {
  // put your main code here, to run repeatedly:

  while (!Serial.available()){
    digitalWrite(LEDSERIAL,HIGH);
  }
  digitalWrite(LEDSERIAL,LOW);
  x = Serial.readString();
  step = x.substring(0, 6).toFloat();
  Serial.print(step);
  if (step >= .5) {
    digitalWrite(LED,HIGH);
  }if(step < .5){
    digitalWrite(LED,LOW);
  }
}
