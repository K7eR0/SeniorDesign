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

const float sensorRate = 100.0f;
const unsigned long updateInterval = 1000000 / sensorRate;
unsigned long lastUpdateTime = 0;

// Global variables to store the gyroscope bias
float gBiasX = 0, gBiasY = 0, gBiasZ = 0;
float roll = 0, pitch = 0, yaw = 0;

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
  // --- IMPORTANT: GYROSCOPE CALIBRATION ---
  Serial.println("Calibrating gyroscope... Keep board perfectly still!");
  const int numSamples = 500;
  float gx_sum = 0, gy_sum = 0, gz_sum = 0;

  for (int i = 0; i < numSamples; i++) {
    float gx, gy, gz;
    // Wait for sensor data to be available
    while (!IMU.gyroscopeAvailable()) {}
    IMU.readGyroscope(gx, gy, gz);
    gx_sum += gx;
    gy_sum += gy;
    gz_sum += gz;
    delay(3);  // A small delay between readings
  }

  // Calculate the average bias
  gBiasX = gx_sum / numSamples;
  gBiasY = gy_sum / numSamples;
  gBiasZ = gz_sum / numSamples;

  Serial.println("Calibration complete.");
  Serial.print("Gyro Bias -> X: ");
  Serial.print(gBiasX);
  Serial.print(", Y: ");
  Serial.print(gBiasY);
  Serial.print(", Z: ");
  Serial.println(gBiasZ);
  Serial.println();

  filter.begin(sensorRate);
  lastUpdateTime = micros();

  Serial.println("Pitch(deg), Roll(deg), Yaw(deg)");
  Serial.println("------------------------------------");

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
      if ((micros() - lastUpdateTime) >= updateInterval) {
        lastUpdateTime = micros();

        float ax, ay, az, gx, gy, gz, mx, my, mz;

        // Read all sensor data
        IMU.readAcceleration(ax, ay, az);
        IMU.readGyroscope(gx, gy, gz);
        IMU.readMagneticField(mx, my, mz);

        // --- Apply Gyroscope Bias Correction ---
        gx -= gBiasX;
        gy -= gBiasY;
        gz -= gBiasZ;

        // Apply your manual magnetometer offsets here if needed
        mx = mx - (-59.5);
        my = my - (-3.0);
        mz = mz - (26.0);

        // Update the filter with the corrected data
        filter.update(-gx, -gy, -gz, ax, ay, az, my, -mx, -mz);

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
        delay(25);
        Serial.println(package);
      }
    }
  }
}
