// Copyright (c) Adafruit Industries. All rights reserved.


// Slow die temperature reader for piping to Adafruit IO

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
    int32_t temp = 0;
    uint32_t err_code;
    
    err_code = sd_temp_get(&temp);
    if (err_code)
    {
      Serial.print("Temperature Error Code: 0x");
      Serial.print(err_code, HEX);
      Serial.println("");
      return 0;
    }
    
    return (temp / 4) * 100;
}

#define OVERSAMPLE 50

void loop() {
  bleSerial.poll();

  // Take 'OVERSAMPLES' measurements and average them!
  float avgtemp = 0;
  for (int i = 0; i < OVERSAMPLE; i++) {
    int32_t temp;
    do {
      temp = temperature_data_get();
    } while (temp == 0);  // re run until we get valid data
    avgtemp += temp;
    delay(1);
  }
  avgtemp /= OVERSAMPLE;
  
  Serial.print("Temperature (C): ");
  Serial.println(avgtemp/100.0F);  // Float value since temp is in 0.25°C steps

  // Send just the raw reading over bluetooth
  bleSerial.println(avgtemp/100.0F);

  delay(5000);
}

