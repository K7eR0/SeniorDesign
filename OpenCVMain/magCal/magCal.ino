#include <Arduino_BMI270_BMM150.h>

// Variables to store the min/max range of the magnetometer readings
float magX_min = 9999, magX_max = -9999;
float magY_min = 9999, magY_max = -9999;
float magZ_min = 9999, magZ_max = -9999;

unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  
  Serial.println("Magnetometer Calibration");
  Serial.println("Start moving the board in figure-8 patterns for 60 seconds...");
  Serial.println("Keep rotating it to cover all orientations.");
  Serial.println();
}

void loop() {
  float mx, my, mz;

  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(mx, my, mz);

    // Update the min and max values for each axis
    if (mx < magX_min) magX_min = mx;
    if (mx > magX_max) magX_max = mx;

    if (my < magY_min) magY_min = my;
    if (my > magY_max) magY_max = my;

    if (mz < magZ_min) magZ_min = mz;
    if (mz > magZ_max) magZ_max = mz;
  }
  
  // Print the calculated offsets every 2 seconds
  if (millis() - lastPrintTime > 2000) {
    // Calculate the offsets
    float offsetX = (magX_max + magX_min) / 2.0;
    float offsetY = (magY_max + magY_min) / 2.0;
    float offsetZ = (magZ_max + magZ_min) / 2.0;

    Serial.print("Offsets -> X: ");
    Serial.print(offsetX, 2);
    Serial.print(", Y: ");
    Serial.print(offsetY, 2);
    Serial.print(", Z: ");
    Serial.println(offsetZ, 2);
    
    lastPrintTime = millis();
  }
}