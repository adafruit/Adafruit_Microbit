// This is a demo of the matrix driver code
// works with Adafruit GFX commands - its just
// a really small screen!
// https://learn.adafruit.com/adafruit-gfx-graphics-library

#include <Adafruit_Microbit.h>
#include <Fonts/TomThumb.h>

Adafruit_Microbit_Matrix microbit;

const uint8_t PROGMEM
  smile_bmp[] =
  { B00000000,
    B01010000,
    B00000000,
    B10001000,
    B01110000, };
    
void setup() {  
  Serial.begin(9600);
  
  Serial.println("microbit matrix demo is ready!");

  microbit.begin();

  // setup font for later!
  microbit.setFont(&TomThumb);
  microbit.setTextWrap(false);
  microbit.setTextColor(LED_ON);
}
    
void loop(){
  // Fill screen
  microbit.fillScreen(LED_ON);
  delay(1000);

  // draw a face
  microbit.clear();
  microbit.drawBitmap(0, 0, smile_bmp, 8, 5, LED_ON);
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

  // scroll some text
  String myMessage = "HELLO WORLD";
  for (int i = 5; i > (((int)myMessage.length()-1) * -5) ; i--) {
    microbit.setCursor(i, 5);
    microbit.clear();
    microbit.print(myMessage);
    delay(150);
  }
}
