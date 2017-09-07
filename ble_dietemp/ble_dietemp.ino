// Copyright (c) Adafruit Industries. All rights reserved.

// Import libraries (BLEPeripheral depends on SPI)
#include <SPI.h>
#include <BLEPeripheral.h>
#include "BLESerial.h"

BLESerial bleSerial;  // Nordic's BLE UART service

void setup()
{
  Serial.begin(115200);
  
  Serial.println("nRF5x Die Temperature");

  bleSerial.setLocalName("micro:bit UART");
  bleSerial.begin();
}

// This function will return the temperature in
// degress Celsius * 100 (so 2900 = 29.00C)
int32_t temperature_data_get(void)
{
    int32_t temp;
    uint32_t err_code;
    
    err_code = sd_temp_get(&temp);
    if (err_code)
    {
      Serial.print("Temperature Error Code: 0x");
      Serial.print(err_code, HEX);
      Serial.println("");
    }
    
    return (temp / 4) * 100;
}

void loop() {
  bleSerial.poll();

  // Print temperature data
  int32_t temp = temperature_data_get();
  Serial.print("Temperature (C): ");
  Serial.println(temp/100.0F);  // Float value since temp is in 0.25°C steps

  // Send it over bluetooth
  bleSerial.println(temp/100.0F);

  delay(100);
}

