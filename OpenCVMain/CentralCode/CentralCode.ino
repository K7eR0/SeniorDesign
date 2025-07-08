#include <ArduinoBLE.h>
#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1)
      ;
  }

  Serial.println("Bluetooth® Low Energy Central scan");

  // start scanning for peripheral
  BLE.scan();
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();

  if (peripheral) {
    // discovered a peripheral
    Serial.println("Discovered a peripheral");
    Serial.println("-----------------------");

    // print address
    Serial.print("Address: ");
    Serial.println(peripheral.address());

    // print the local name, if present
    if (peripheral.hasLocalName()) {
      Serial.print("Local Name: ");
      Serial.println(peripheral.localName());
    }

    // print the advertised service UUIDs, if present
    if (peripheral.hasAdvertisedServiceUuid()) {
      Serial.print("Service UUIDs: ");
      for (int i = 0; i < peripheral.advertisedServiceUuidCount(); i++) {
        Serial.print(peripheral.advertisedServiceUuid(i));
        Serial.print(" ");
      }
      Serial.println();
    }

    // print the RSSI
    Serial.print("RSSI: ");
    Serial.println(peripheral.rssi());

    Serial.println();

    if (peripheral.localName() != "IMUWearable") {
      return;
    }
    BLE.stopScan();
    monitorBLEperipheral(peripheral);
    BLE.scanForUuid("180f");
  }
}

void monitorBLEperipheral(BLEDevice peripheral) { // THIS FUNCTION IS NOT OUR OWN AND IS PURELY FOR TESTING PURPOSES AND WILL BE REVERSE ENGINEERED AND REPLACED
  // connect to the peripheral
  Serial.println("Connecting ...");
  if (peripheral.connect()) {
    Serial.println("Connected");
  } else {
    Serial.println("Failed to connect!");
    return;
  }

  // discover peripheral attributes
  Serial.println("Discovering service 0xffe0 ...");
  if (peripheral.discoverService("180F")) {
    Serial.println("Service discovered");
  } else {
    Serial.println("Attribute discovery failed.");
    peripheral.disconnect();

    while (1);
    return;
  }

  // retrieve the IMUSensorData characteristic
  BLECharacteristic IMUSensorData = peripheral.characteristic("2A19");
//  BLEStringCharacteristic IMUSensorData = peripheral.characteristic("2A19", BLERead | BLEWrite, 512);
  
  // subscribe to the simple key characteristic
  Serial.println("Subscribing to IMUSensorData characteristic ...");
  if (!IMUSensorData) {
    Serial.println("no IMUSensorData characteristic found!");
    peripheral.disconnect();
    return;
  } 
    else if (!IMUSensorData.canSubscribe()) {
    Serial.println("IMUSensorData characteristic is not subscribable!");
    peripheral.disconnect();
    return;
  } 
    else if (!IMUSensorData.subscribe()) {
    Serial.println("subscription failed!");
    peripheral.disconnect();
    return;
  } 
    else {
    Serial.println("Subscribed to IMUSensorData characteristic");
//    Serial.println("Press the right and left buttons on your SensorTag.");
  }

  while (peripheral.connected()) {
    // while the peripheral is connected

    // check if the value of the simple key characteristic has been updated
    if (IMUSensorData.valueUpdated()) {
//    if (IMUSensorData.written()){
      // yes, get the value, characteristic is 1 byte so use byte value
//      long value = 0 ;
//      int ReceivingArray[2] = {0,0}; 

// THIS SECTION CONVERTS THE RECEIVED CHARACTERISTIC FROM UNSIGNED CHAR TO STRING // 
      String str;
      int length = IMUSensorData.valueLength();
      const uint8_t* val = IMUSensorData.value();
      str.reserve(length);

      for (int i = 0; i<length; i++){
        str += (char)val[i];
      }

      Serial.println(str);
        
//      IMUSensorData.readValue(value);

      
//      Serial.println(value);
//      IMUSensorData.readValue(ReceivingArray,2);
//      Serial.println(ReceivingArray[0]);
//      Serial.println(ReceivingArray[1]);
//      for(int i = 0; i < 1; i++)
//      {
//        Serial.println(ReceivingArray[i]);
//      }

//      if (value & 0x01) {
//        // first bit corresponds to the right button
//        Serial.println("Right button pressed");
//      }
//
//      if (value & 0x02) {
//        // second bit corresponds to the left button
//        Serial.println("Left button pressed");
      }
    }
  

  Serial.println("BLE disconnected!");
}

void readData(BLEDevice peripheral) { // THIS IS OUR FUNCTION BUT CURRENTLY DOESN'T WORK, IT WILL BE MODIFIED LATER
  Serial.println("Found");
  while (peripheral.connected()) {
    BLECharacteristic IMUSensorData = peripheral.characteristic("2A19");
    String receivedString = "";
    IMUSensorData.read();
    if (IMUSensorData.read()) {
    String receivedString = "";
    for (int i = 0; i < IMUSensorData.valueLength(); i++) {
      receivedString += (char)IMUSensorData.value()[i];
    }
    Serial.print("Received String: ");
    Serial.println(receivedString);

  }
}
}
