// Slow die temperature reader for piping to Adafruit IO

#include <Adafruit_Microbit.h>

Adafruit_Microbit microbit;

void setup()
{
  Serial.begin(115200);
  
  Serial.println("nRF5x Die Temperature Plotting");

  microbit.BTLESerial.begin();
  microbit.BTLESerial.setLocalName("microbit");

  // Start LED matrix driver after radio (required)
  microbit.begin();
}

#define OVERSAMPLE 50

void loop() {
  microbit.BTLESerial.poll();

  // Take 'OVERSAMPLES' measurements and average them!
  float avgtemp = 0;
  for (int i = 0; i < OVERSAMPLE; i++) {
    int32_t temp;
    do {
      temp = microbit.getDieTemp();
    } while (temp == 0);  // re run until we get valid data
    avgtemp += temp;
    delay(1);
  }
  avgtemp /= OVERSAMPLE;
  
  Serial.print("Temperature (C): ");
  Serial.println(avgtemp);  // Float value since temp is in 0.25°C steps

  // Send just the raw reading over bluetooth
  microbit.BTLESerial.println(avgtemp);

  delay(5000);
}

