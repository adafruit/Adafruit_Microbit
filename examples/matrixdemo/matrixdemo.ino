// This is a demo of the matrix driver code
// works with Adafruit GFX commands - its just
// a really small screen!
// https://learn.adafruit.com/adafruit-gfx-graphics-library

#include <Adafruit_Microbit.h>

Adafruit_Microbit_Matrix microbit;

const uint8_t
  smile_bmp[] =
  { B00000,
    B01010,
    B00000,
    B10001,
    B01110, };
    
void setup() {  
  Serial.begin(9600);
  
  Serial.println("microbit matrix demo is ready!");

  microbit.begin();
}
    
void loop(){
  // Fill screen
  microbit.fillScreen(LED_ON);
  delay(1000);

  // draw a heart
  microbit.show(microbit.HEART);
  delay(1000);

  // draw a no cross
  microbit.show(microbit.NO);
  delay(1000);

  // draw a yes check
  microbit.show(microbit.YES);
  delay(1000);

  // draw a custom made bitmap face
  microbit.show(smile_bmp);
  delay(1000);
  
  microbit.clear();
  // Draw a line 'by hand'
  microbit.drawPixel(0, 0, LED_ON);
  microbit.drawPixel(1, 1, LED_ON);
  microbit.drawPixel(2, 2, LED_ON);
  microbit.drawPixel(3, 3, LED_ON);
  microbit.drawPixel(4, 4, LED_ON);
  // draw the 'opposite' line with drawline (easier!)
  microbit.drawLine(0, 4, 4, 0, LED_ON);

  delay(1000);

  // erase screen, draw a square
  microbit.clear();
  microbit.drawRect(0,0, 5, 5, LED_ON); // top left corner @ (0,0), 5 by 5 pixels size

  delay(1000);

  // erase screen, draw a circle
  microbit.clear();
  microbit.drawCircle(2,2, 2, LED_ON); // center on 2, 2, radius 2

  delay(1000);

  // erase screen, draw a filled triangle
  microbit.clear();
  microbit.fillTriangle(0,4, 2,0, 4,4, LED_ON); 

  delay(1000);

  // scroll some text the 'easy' way
  microbit.print("HELLO WORLD");

  // count up!
  for (int i=0; i<10; i++) {
    microbit.print(i);
    delay(500);
  }

  microbit.print(3.1415, 4);  // pi time, 4 digits of precision!!

}
