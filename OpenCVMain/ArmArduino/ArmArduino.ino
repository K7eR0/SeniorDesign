//Senior Design Caprstone - Robotic Arm Manipulator Code
//Dylan Cadigan & Robert Nguyen

//Including Required Libraries
#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

//String from the serial communication
String x;

//--------------------------------------------------------------------
//Base Stepper Motor
// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1

// Create a new instance of the AccelStepper class:
AccelStepper stepper1 = AccelStepper(motorInterfaceType, stepPin, dirPin);
//--------------------------------------------------------------------

float base_value = 0;


void setup() {
  Serial.begin(115200);    //Start a serial connection with baud rate of 115200
  Serial.setTimeout(1.5);  //Timeout the conenction after 1.5 seconds

  //Setting Base Stepper Motor Parameters
  stepper1.setMaxSpeed(90);
  stepper1.setAcceleration(20);
}

void loop() {
  // put your main code here, to run repeatedly:
  //while (!Serial.available());
  x = Serial.readString();
  //x = String("  0.02   1.23   0.10   0.88   0.35   0.53 142.60  -4.62 167.85 412.30");
  base_value = x.substring(0, 6).toFloat();
  // stepper1.moveTo(step);
  // stepper1.runToPosition();
  //Serial.print(step);

  // Map the smoothed potentiometer value (0 to 1) to a target position (0 to 200):
  int position = map(base_value, 0, 1, 0, 100);
  //int position = base_value*200;
  // Set the target position:
  stepper1.moveTo(position);
  //Serial.print(position);
  //Serial.print(base_value);
  delay(100);
  //stepper1.moveTo(0.3);

  // Run to target position with set speed and acceleration/deceleration:
  stepper1.runToPosition();
}
