/*
  SerialPassthrough sketch

  Some boards, like the Arduino 101, the MKR1000, Zero, or the Micro, have one
  hardware serial port attached to Digital pins 0-1, and a separate USB serial
  port attached to the IDE Serial Monitor. This means that the "serial
  passthrough" which is possible with the Arduino UNO (commonly used to interact
  with devices/shields that require configuration via serial AT commands) will
  not work by default.

  This sketch allows you to emulate the serial passthrough behaviour. Any text
  you type in the IDE Serial monitor will be written out to the serial port on
  Digital pins 0 and 1, and vice-versa.

  On the 101, MKR1000, Zero, and Micro, "Serial" refers to the USB Serial port
  attached to the Serial Monitor, and "Serial1" refers to the hardware serial
  port attached to pins 0 and 1. This sketch will emulate Serial passthrough
  using those two Serial ports on the boards mentioned above, but you can change
  these names to connect any two serial ports on a board that has multiple ports.

  created 23 May 2016
  by Erik Nyquist

  https://docs.arduino.cc/built-in-examples/communication/SerialPassthrough/
*/
String x;
float step = 0;
#define LED 13
#define LEDSERIAL 2
void setup() {
  pinMode(LED, OUTPUT);
  pinMode(LEDSERIAL, OUTPUT);
  Serial.begin(115200);
  Serial.setTimeout(200);
}

void loop() {
  // put your main code here, to run repeatedly:

  while (!Serial.available()){
    digitalWrite(LEDSERIAL,HIGH);
  }
  digitalWrite(LEDSERIAL,LOW);
  x = Serial.readString();
  step = x.substring(2, 7).toFloat();
  Serial.print(step);
  if (step >= 0) {
    digitalWrite(LED,HIGH);
  }if(step < 0){
    digitalWrite(LED,LOW);
  }
}
