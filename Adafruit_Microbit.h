#ifndef _ADAFRUIT_MICROBIT_H_
#define _ADAFRUIT_MICROBIT_H_

#include <Adafruit_GFX.h>

#define LED_ON   1
#define LED_OFF  0

void IRQ_MATRIX_HANDLER(void);

class Adafruit_Microbit : public Adafruit_GFX {
 public:
  Adafruit_Microbit();
  ~Adafruit_Microbit(void);
  boolean   begin(void);
  void      drawPixel(int16_t x, int16_t y, uint16_t color),
            fillScreen(uint16_t color),
            clear(void),
            rowHandler(void);

 private:
  void startTimer();

  uint8_t matrix_buffer[3][9];
};

#endif

