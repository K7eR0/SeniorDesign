//Senior Design Caprstone - Robotic Arm Manipulator Code
//Dylan Cadigan & Robert Nguyen

#include <Servo.h>
#include <Wire.h>
String x;
float step = 0;
#define LED 13
#define LEDSERIAL 2
float old_pos = 0;
float current_pos = 90;
int n = 0;
int claw = 0;

Servo myservo;  // create Servo object to control a servo
// twelve Servo objects can be created on most boards

Servo wrist;

int pos = 0;    // variable to store the servo position



void setup() {
  pinMode(LED, OUTPUT);
  pinMode(LEDSERIAL, OUTPUT);
  Serial.begin(500000);
  Serial.setTimeout(20);

  myservo.attach(9);  // attaches the servo on pin 9 to the Servo object
  wrist.attach(10);
}

void loop() {
  // put your main code here, to run repeatedly:

  while (!Serial.available()){
    digitalWrite(LEDSERIAL,HIGH);
  }
  digitalWrite(LEDSERIAL,LOW);
  x = Serial.readString();
  step = x.substring(41, 50).toFloat();
  
 // Map the smoothed potentiometer value (0 to 1023) to a target position (0 to 200):
  //int pos = map(step, -50, 120, 0, 180);
  
  if (n == 0){
    old_pos = step;

    n++; 
  }

  current_pos = current_pos-1.25*(step-old_pos);
  
  wrist.write(current_pos);              // tell servo to go to position in variable 'pos'
  //delay(10);        
  
  old_pos = step;
  n = n+1;

  Serial.print(step);
  Serial.print(x.substring(40, 48));


  claw = x.substring(65).toFloat();
 
 if (claw == 0){
  //for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(0);              // tell servo to go to position in variable 'pos'
    //delay(15);                       // waits 15 ms for the servo to reach the position
 // }
  }

  if (claw == 1){
     //for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(90);              // tell servo to go to position in variable 'pos'
    //delay(15);                       // waits 15 ms for the servo to reach the position
  //}
  }
 


 



  

}