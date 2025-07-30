#include <Arduino_BMI270_BMM150.h>
#include <ArduinoBLE.h>
#include <Arduino.h>
#include <MadgwickAHRS.h>
#include <stdio.h>

// Declare Bluetooth service
BLEService IMUService("180F");                                            //Set the service to IMU and entered UUID
BLEStringCharacteristic IMUSensorData("2A19", BLERead | BLENotify, 512);  //Set the data characteristics

// IMU filter variables:
// Setup filter
Madgwick filter;

// Sensor Measurements:
float xa, ya, za;  //Accelerometer in m/s^2
float xg, yg, zg;  //Gyro in rad/s
float xm, ym, zm;  //Mag in uT
const unsigned long updateInterval = 1000000;
unsigned long lastUpdateTime = 0;

// Ouputs
float roll = 0, pitch = 0, yaw = 0;  //Roll Pitch Yaw

// Button and Sensors
#define Calibration 4  //Setup for Calibration button

void setup() {
  pinMode(Calibration, INPUT);
  Serial.begin(9600);
  while (!Serial)
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
  BLE.setAdvertisedService(IMUService);         //add the service UUID
  IMUService.addCharacteristic(IMUSensorData);  //add the IMU characteristic
  BLE.addService(IMUService);                   //Add the IMU service
  IMUSensorData.writeValue("Init");             //set initial value for this characteristic

  BLE.advertise();  //Display Service

  String address = BLE.address();
  Serial.print("Waiting for connections, address is: ");
  Serial.println(address);

  filter.begin(60);  //Start filter with a sample rate of 60Hz

  delay(1000);
}

void loop() {
  // Create/clear a string to send data to central
  BLEDevice central = BLE.central();  //Wait for connection to central
  if (central) {
    Serial.println(central.address());
    while (central.connected()) {
      // Calibrate the system once
      // while(Calib = 0){
      //   if (digitalRead(Calibration) == HIGH){
      //     Calib = 1;
      //   }
      // }

      String package;   //Create a buffer to store the whole string
      char buffer[10];  //Create a buffer to store the individual strings
                        // Setup time int since last loop
      if ((micros() - lastUpdateTime) >= updateInterval/IMU.accelerationSampleRate()) {
        lastUpdateTime = micros();

        IMU.readAcceleration(xa, ya, za);
        IMU.readGyroscope(xg, yg, zg);
        IMU.readMagneticField(xm, ym, zm);

        // Perform sensor fusion using the filter
        filter.update(xg, yg, zg, xa, ya, za, xm, ym, zm);

        // Get orientation estimates in terms of pitch, yaw, and roll
        roll = filter.getRoll();
        pitch = filter.getPitch();
        yaw = filter.getYaw();
        snprintf(buffer, sizeof(buffer), "%8.2f", roll);
        package += buffer;
        snprintf(buffer, sizeof(buffer), "%7.2f", pitch);
        package += buffer;
        snprintf(buffer, sizeof(buffer), "%7.2f", yaw);
        package += buffer;
        IMUSensorData.writeValue(package);  //Send data to central
        Serial.println(package);
      }
    }
  }
}
