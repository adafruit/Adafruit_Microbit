
/*
 * Serial Port over BLE
 * Create UART service compatible with Nordic's *nRF Toolbox* and Adafruit's *Bluefruit LE* iOS/Android apps.
 *
 * Copyright (c) Sandeep Mistry. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 * BLESerial class implements same protocols as Arduino's built-in Serial class and can be used as it's wireless
 * replacement. Data transfers are routed through a BLE service with TX and RX characteristics. To make the
 * service discoverable all UUIDs are NUS (Nordic UART Service) compatible.
 *
 * Please note that TX and RX characteristics use Notify and WriteWithoutResponse, so there's no guarantee
 * that the data will make it to the other end. However, under normal circumstances and reasonable signal
 * strengths everything works well.
 */

#include <Adafruit_Microbit.h>

Adafruit_Microbit microbit;

void setup() {
  Serial.begin(115200);

  Serial.println("Microbit ready!");
  
  // custom services and characteristics can be added as well
  microbit.BTLESerial.begin();
  microbit.BTLESerial.setLocalName("microbit");

  // Start LED matrix driver after radio (required)
  microbit.begin();
}

void loop() {
  microbit.BTLESerial.poll();

  forward();
  //loopback();
  spam();
}


// forward received from Serial to microbit.BTLESerial and vice versa
void forward() {
  if (microbit.BTLESerial && Serial) {
    int byte;
    if (microbit.BTLESerial.available()) {
      Serial.write(microbit.BTLESerial.read());
    }
    char buffer[10];
    memset(buffer, 0x0, 10);
    int idx = 0;
    
    while (Serial.available() && idx != 10) {
       buffer[idx] = Serial.read();
       idx++;
    }
    if (idx) {
      microbit.BTLESerial.write(buffer, idx);
    }
  }
  delay(1);
}

// echo all received data back
void loopback() {
  if (microbit.BTLESerial) {
    int byte;
    while ((byte = microbit.BTLESerial.read()) > 0) {
        microbit.BTLESerial.write(byte);
    }
  }
}

// periodically sent time stamps
void spam() {
  if (microbit.BTLESerial) {
    microbit.BTLESerial.print(millis());
    microbit.BTLESerial.println(" tick-tacks!");
    delay(1000);
  }
}
