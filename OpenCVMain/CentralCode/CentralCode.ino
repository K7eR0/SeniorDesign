#include <ArduinoBLE.h>
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1.5);

  if(!BLE.begin()){
    Serial.println("Failed to initialize BLE"); //Shows if BLE module failed to activate
    while(1);  
  }
  BLE.scan();
}

void loop() {
  BLEDevice peripheral = BLE.available(); // Check for available peripgerals
  if (peripheral.localName() == "IMUWearable"){
    BLE.stopScan();
  }else{
    BLE.scan();
  }
  
}