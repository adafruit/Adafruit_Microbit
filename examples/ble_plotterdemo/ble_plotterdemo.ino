// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include "BLESerial.h"

// Add accelerometer support
#include "Wire.h"
#include "MMA8653.h"

MMA8653 accel;

// Our BLE connection
BLESerial bleSerial;


void setup() {
  Serial.begin(115200);

  Serial.println("Plotter demo ready!");

  accel.begin(false, 2); // 8-bit mode, 2g range
    
  // custom services and characteristics can be added as well
  bleSerial.setLocalName("microbit UART");
  bleSerial.begin();
}

void loop() {
  bleSerial.poll();

  accel.update();

  // print the data on serial port
  Serial.print(accel.getX());    Serial.print(", ");
  Serial.print(accel.getY());    Serial.print(", ");
  Serial.println(accel.getZ());

  // send it over bluetooth
  bleSerial.print(accel.getX());    bleSerial.print(",");
  bleSerial.print(accel.getY());    bleSerial.print(",");
  bleSerial.println(accel.getZ());

  delay(100);
}
