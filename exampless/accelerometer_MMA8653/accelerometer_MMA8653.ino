/*
 * MMA845XQ test code
 * (C) 2012 Akafugu Corporation
 *
 * This program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 */


#include "Wire.h"
#include "MMA8653.h"

MMA8653 accel;

void setup() {
  Serial.begin(9600);

  Serial.println("microbit accel test");
  
  accel.begin(false, 2); // 8-bit mode, 2g range
}


void loop() {
  accel.update();
  Serial.print((float)accel.getX() * 0.0156);    Serial.print(" , ");
  Serial.print((float)accel.getY() * 0.0156);    Serial.print(", ");
  Serial.println((float)accel.getZ() * 0.0156);

  delay(100);
}
