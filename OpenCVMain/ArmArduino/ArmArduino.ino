#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
String x;
int step = 0;
AccelStepper stepper1(1,3,2);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setTimeout(1.5);
  stepper1.setMaxSpeed(200);
  stepper1.setAcceleration(30);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (!Serial.available());
  x = Serial.readString();
  step = x.substring(0, 5).toInt();
  stepper1.moveTo(step);
  stepper1.runToPosition();
  Serial.print(step);
}
