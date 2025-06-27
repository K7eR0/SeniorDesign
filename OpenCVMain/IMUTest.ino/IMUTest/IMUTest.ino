/*
  Arduino LSM6DS3 - Simple Accelerometer

  This example reads the acceleration values from the LSM6DS3
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  - Arduino Uno WiFi Rev 2 or Arduino Nano 33 IoT

  created 10 Jul 2019
  by Riccardo Rizzo

  This example code is in the public domain.
*/

#include <Arduino_LSM6DS3.h>
#include <ArduinoBLE.h>

BLEService IMUService("180F"); //Set the service to IMU and entered UUID
BLEStringCharacteristic IMUSensorData("2A19", BLERead | BLENotify, 512); //Set the data characteristics
void setup() {

  Serial.begin(9600);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!"); //Shows if IMU module failed to activate

    while (1);
  }
  if(!BLE.begin()){
    Serial.println("Failed to initialize BLE") //Shows if BLE module failed to activate
    while(1);  
  }
  BLE.setLocalName("IMUWearable");
  BLE.setAdvertisedService(IMUService); // add the service UUID
  IMUService.addCharacteristic(IMUSensorData); // add the IMU characteristic
  BLE.addService(IMUService); // Add the IMU service
  IMUSensorData.writeValue("Init"); // set initial value for this characteristic
  
  BLE.advertise(); // Display Service

  String address = BLE.address();
  Serial.print("Waiting for connections, address is: ");
  Serial.println(address);

  delay(1000);
}

void loop() {
  float x, y, z;
  String package; // Create a string to send data to central
  BLE central = BLE.central(); // Connect to central

  if (IMU.accelerationAvailable()) { // If statement that sends code whenever there is acceleration acting on the IMU
    IMU.readAcceleration(x, y, z); // Put the readings into floats for x y z vectors

    Serial.print(x);
    Serial.print('\t');
    Serial.print(y);
    Serial.print('\t');
    Serial.println(z);
  }
}
