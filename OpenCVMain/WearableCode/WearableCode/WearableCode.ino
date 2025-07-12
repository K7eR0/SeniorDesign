#include <Arduino_LSM6DS3.h>
#include <ArduinoBLE.h>
#include <Arduino.h>

BLEService IMUService("180F");                                            //Set the service to IMU and entered UUID
BLEStringCharacteristic IMUSensorData("2A19", BLERead | BLENotify, 512);  //Set the data characteristics
void setup() {

  //Serial.begin(9600);
  //while (!Serial)
    ;

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");  //Shows if IMU module failed to activate

    while (1)
      ;
  }
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE");  //Shows if BLE module failed to activate
    while (1)
      ;
  }
  BLE.setLocalName("IMUWearable");
  BLE.setAdvertisedService(IMUService);         // add the service UUID
  IMUService.addCharacteristic(IMUSensorData);  // add the IMU characteristic
  BLE.addService(IMUService);                   // Add the IMU service
  IMUSensorData.writeValue("Init");             // set initial value for this characteristic

  BLE.advertise();  // Display Service

  String address = BLE.address();
  Serial.print("Waiting for connections, address is: ");
  Serial.println(address);

  delay(1000);
}

void loop() {
  float x1, y1, z1, x2, y2, z2;
  // Create/clear a string to send data to central
  BLEDevice central = BLE.central();  // Wait for connection to central
  if (central) {
    Serial.println(central.address());
    while (central.connected()) {
      String package;
      if (IMU.accelerationAvailable()) {  // If statement that sends code whenever there is acceleration acting on the IMU
        IMU.readAcceleration(x1, y1, z1);    // Put the readings into floats for x y z vectors
        IMU.readGyroscope(x2, y2, z2);    // Put the readings into floats for x y z tilt
        // Build the string to send to central
        package += String("C ");  // Have start letter for central to process as IMU data just in case other data needs to be sent
        package += String(x1,2);
        package += String(" ");
        package += String(y1,2);
        package += String(" ");
        package += String(z1,2);
        package += String(" ");
        // Keep adding to the string
        package += String(x2,2);
        package += String(" ");
        package += String(y2,2);
        package += String(" ");
        package += String(z2,2);
        package += String(" ");
        IMUSensorData.writeValue(package);  // Send data to central
        delay(205);
        Serial.println(package);
      }
    }
  }
}
