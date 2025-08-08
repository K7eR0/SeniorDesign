#include <ArduinoBLE.h>
#include <Arduino.h>

const int trigPin = 9;
const int echoPin = 10;
float duration, distance;
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(500000);
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

void monitorBLEperipheral(BLEDevice peripheral) {
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

    while (1)
      ;
    return;
  }

  // retrieve the IMUSensorData characteristic
  BLECharacteristic IMUSensorData = peripheral.characteristic("2A19");

  // subscribe to the simple key characteristic
  Serial.println("Subscribing to IMUSensorData characteristic ...");
  if (!IMUSensorData) {
    Serial.println("no IMUSensorData characteristic found!");
    peripheral.disconnect();
    return;
  } else if (!IMUSensorData.canSubscribe()) {
    Serial.println("IMUSensorData characteristic is not subscribable!");
    peripheral.disconnect();
    return;
  } else if (!IMUSensorData.subscribe()) {
    Serial.println("subscription failed!");
    peripheral.disconnect();
    return;
  } else {
    Serial.println("Subscribed to IMUSensorData characteristic");
  }

  while (peripheral.connected()) {

    // Look for an update on the IMUSensorData characteristic
    if (IMUSensorData.valueUpdated()) {
      // Get distance from ultrasonic sensor
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      duration = pulseIn(echoPin, HIGH);
      distance = duration * 0.034 / 2;

      // Convert char to string to send to python
      String str;
      int length = IMUSensorData.valueLength();
      const uint8_t *val = IMUSensorData.value();
      str.reserve(length);
      char buffer[10];
      String buffer2;
      for (int i = 0; i < 8; i++) {
        buffer2 += (char)val[i];
      }
      snprintf(buffer, sizeof(buffer), "%8.2f", buffer2);
      str += buffer2;
      buffer2 = "";

      for (int i = 8; i < 15; i++) {
        buffer2 += (char)val[i];
      }
      snprintf(buffer, sizeof(buffer), "%7.2f", buffer2);
      str += buffer2;

      buffer2 = "";
      for (int i = 14; i < 21; i++) {
        buffer2 += (char)val[i];
      }
      snprintf(buffer, sizeof(buffer), "%7.2f", buffer2);
      str += buffer2;
      str += " ";
      str += (char)val[23];

      snprintf(buffer, sizeof(buffer), "%6.2f", distance);
      str += " " + String(buffer);
      Serial.println(str);
      delay(10);
    }
  }

  Serial.println("BLE disconnected!");
}
