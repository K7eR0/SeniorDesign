#include <Arduino_LSM6DS3.h>
#include <ArduinoBLE.h>
#include <Arduino.h>

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
    Serial.println("Failed to initialize BLE"); //Shows if BLE module failed to activate
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
  String package; // Create/clear a string to send data to central
  BLEDevice central = BLE.central(); // Connect to central

  if (IMU.accelerationAvailable()) { // If statement that sends code whenever there is acceleration acting on the IMU
    IMU.readAcceleration(x, y, z); // Put the readings into floats for x y z vectors

    // Build the string to send to central
    package += String("C "); // Have start letter for central to process as IMU data just in case other data needs to be sent
    package += String(x);
    package += String(" ");
    package += String(y);
    package += String(" ");
    package += String(z);
    package += String(" ");
  }
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z); // Put the readings into floats for x y z tilt

    // Keep adding to the string
    package += String(x);
    package += String(" ");
    package += String(y);
    package += String(" ");
    package += String(z);
  }

  IMUSensorData.writeValue(package); // Send data to central
}
