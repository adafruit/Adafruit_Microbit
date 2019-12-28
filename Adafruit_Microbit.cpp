

/**
 *  Includes ftoa() code from stm32tpl --  STM32 C++ Template Peripheral Library
 *
 *  Copyright (c) 2009-2014 Anton B. Gusev aka AHTOXA
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 *
 */

#include "nrf_soc.h"
#include <Adafruit_Microbit.h>

#define MATRIX_ROWS 3
#define MATRIX_COLS 9
uint8_t rowpins[MATRIX_ROWS] = {26, 27, 28};
uint8_t colpins[MATRIX_COLS] = {3, 4, 10, 23, 24, 25, 9, 7, 6};

uint8_t pixel_to_row[25] = {1, 2, 1, 2, 1, 3, 3, 3, 3, 3, 2, 1, 2,
                            3, 2, 1, 1, 1, 1, 1, 3, 2, 3, 2, 3};

uint8_t pixel_to_col[25] = {1, 4, 2, 5, 3, 4, 5, 6, 7, 8, 2, 9, 3,
                            9, 1, 8, 7, 6, 5, 4, 3, 7, 1, 6, 2};

volatile uint8_t currentRow = 0;

Adafruit_Microbit_Matrix *handle = NULL;

/** TIMTER2 peripheral interrupt handler. This interrupt handler is called
 * whenever there it a TIMER2 interrupt Don't mess with this line. really.
 */
extern "C" {
void TIMER2_IRQHandler(void) { IRQ_MATRIX_HANDLER(); }
}

Adafruit_Microbit_Matrix::Adafruit_Microbit_Matrix() : Adafruit_GFX(5, 5) {
  memset(matrix_buffer, 0x0, MATRIX_COLS * MATRIX_ROWS);
}

Adafruit_Microbit_Matrix::~Adafruit_Microbit_Matrix(void) {}

/*!
 *    @brief Initialized the 5x5 matrix and scanning IRQ
 *    @returns True
 */
boolean Adafruit_Microbit_Matrix::begin(void) {
  handle = this;

  for (uint8_t c = 0; c < MATRIX_COLS; c++) {
    pinMode(colpins[c], OUTPUT);
    digitalWrite(colpins[c], HIGH);
  }

  for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
    pinMode(rowpins[r], OUTPUT);
    digitalWrite(rowpins[r], LOW);
  }

  for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
    for (uint8_t c = 0; c < MATRIX_COLS; c++) {
      matrix_buffer[r][c] = 0;
    }
  }

  currentRow = 0;

  startTimer();

  return true;
}

/*!
 *    @brief Matrix object function called by IRQ handler for each row
 *    This is not optimized at all but its not so bad either!
 */
void Adafruit_Microbit_Matrix::rowHandler(void) {
  // disable current row
  digitalWrite(rowpins[currentRow], LOW);
  for (uint8_t c = 0; c < MATRIX_COLS; c++) {
    digitalWrite(colpins[c], HIGH);
  }

  // go to next row
  if (++currentRow >= MATRIX_ROWS) {
    currentRow = 0;
  }

  // Serial.print("Row: "); Serial.println(currentRow);

  for (uint8_t c = 0; c < MATRIX_COLS; c++) {
    if (matrix_buffer[currentRow][c]) {
      digitalWrite(colpins[c], LOW); // Change column states for new row
    }
  }
  digitalWrite(rowpins[currentRow], HIGH);
}

/*!
 *    @brief Sets up the IRQ for timer 2 to run the matrix refresh.
 */
void Adafruit_Microbit_Matrix::startTimer(void) {
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer; // Set the timer in Counter Mode
  NRF_TIMER2->TASKS_CLEAR = 1; // clear the task first to be usable for later
  NRF_TIMER2->PRESCALER = 4;
  NRF_TIMER2->BITMODE =
      TIMER_BITMODE_BITMODE_16Bit; // Set counter to 16 bit resolution
  NRF_TIMER2->CC[0] = 1000;        // Set value for TIMER2 compare register 0
  NRF_TIMER2->CC[1] = 0;           // Set value for TIMER2 compare register 1

  // Enable interrupt on Timer 2, both for CC[0] and CC[1] compare match events
  NRF_TIMER2->INTENSET =
      (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos);
  NVIC_EnableIRQ(TIMER2_IRQn);

  NRF_TIMER2->TASKS_START = 1; // Start TIMER2
}

void IRQ_MATRIX_HANDLER(void) {
  if ((NRF_TIMER2->EVENTS_COMPARE[0] != 0) &&
      ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0)) {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0; // Clear compare register 0 event
    handle->rowHandler();
    NRF_TIMER2->CC[0] += 1000;
  }
}

/*!
 *    @brief  Draw a single pixel/LED on the 5x5 matrix
 *    @param  x 0 to 4 column
 *    @param  y 0 to 4 row
 *    @param  color 1 for LEDs on, 0 for off
 */
void Adafruit_Microbit_Matrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
    return;

  int16_t t;
  switch (rotation) {
  case 1:
    t = x;
    x = WIDTH - 1 - y;
    y = t;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = y;
    y = HEIGHT - 1 - t;
    break;
  }

  // get row and col
  uint8_t pixelnum = y * 5 + x;
  uint8_t col = pixel_to_col[pixelnum] - 1;
  uint8_t row = pixel_to_row[pixelnum] - 1;
  // Serial.print("Setting row "); Serial.print(row); Serial.print(" & col ");
  // Serial.println(col);

  if (color)
    matrix_buffer[row][col] = 1;
  else
    matrix_buffer[row][col] = 0;
}

/*!
 *    @brief  Clear the 5x5 matrix
 */
void Adafruit_Microbit_Matrix::clear(void) { fillScreen(0); }

/*!
 *    @brief  Fill the 5x5 matrix with an LED 'color'
 *    @param  color 1 for LEDs on, 0 for off
 */
void Adafruit_Microbit_Matrix::fillScreen(uint16_t color) {
  for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
    for (uint8_t c = 0; c < MATRIX_COLS; c++) {
      matrix_buffer[r][c] = color;
    }
  }
}

/*!
 *    @brief  Display a 5-byte bitmap on the 5x5 LED matrix
 *    @param  bitmap 5 byte bitmap
 */
void Adafruit_Microbit_Matrix::show(const uint8_t bitmap[]) {
  clear();
  drawBitmap(-3, 0, bitmap, 8, 5, LED_ON);
}

/*!
 *    @brief  Display a string on the 5x5 LED matrix
 *    @param  string Null-terminated ascii string
 */
void Adafruit_Microbit_Matrix::print(char *string) {
  setFont(&TomThumb);
  setTextWrap(false);
  setTextColor(LED_ON);

  if (strlen(string) > 1) {
    scrollText(string);
  } else {
    clear();
    setCursor(0, 5);
    Adafruit_GFX::print(string);
  }
}

/*!
 *    @brief  Display a signed number on the 5x5 LED matrix
 *    @param  i The value
 */
void Adafruit_Microbit_Matrix::print(int32_t i) {
  char buffer[34];
  memset(buffer, 0, 34);

  itoa(i, buffer, 10);
  print(buffer);
}

/*!
 *    @brief  Display a signed number on the 5x5 LED matrix
 *    @param  i The value
 */
void Adafruit_Microbit_Matrix::print(int i) { print((int32_t)i); }

#define MAX_PRECISION (10)
static const double rounders[MAX_PRECISION + 1] = {
    0.5,          // 0
    0.05,         // 1
    0.005,        // 2
    0.0005,       // 3
    0.00005,      // 4
    0.000005,     // 5
    0.0000005,    // 6
    0.00000005,   // 7
    0.000000005,  // 8
    0.0000000005, // 9
    0.00000000005 // 10
};

/*!
 *    @brief  Display a floating point number on the 5x5 LED matrix
 *    @param  f The floating point value
 *    @param  precision Digits after decimal
 */
void Adafruit_Microbit_Matrix::print(double f, int precision) {
  char buf[80];

  char *ptr = buf;
  char *p = ptr;
  char *p1;
  char c;
  long intPart;

  // check precision bounds
  if (precision > MAX_PRECISION)
    precision = MAX_PRECISION;

  // sign stuff
  if (f < 0) {
    f = -f;
    *ptr++ = '-';
  }

  if (precision < 0) { // negative precision == automatic precision guess
    if (f < 1.0)
      precision = 6;
    else if (f < 10.0)
      precision = 5;
    else if (f < 100.0)
      precision = 4;
    else if (f < 1000.0)
      precision = 3;
    else if (f < 10000.0)
      precision = 2;
    else if (f < 100000.0)
      precision = 1;
    else
      precision = 0;
  }

  // round value according the precision
  if (precision)
    f += rounders[precision];

  // integer part...
  intPart = f;
  f -= intPart;

  if (!intPart)
    *ptr++ = '0';
  else {
    // save start pointer
    p = ptr;

    // convert (reverse order)
    while (intPart) {
      *p++ = '0' + intPart % 10;
      intPart /= 10;
    }

    // save end pos
    p1 = p;

    // reverse result
    while (p > ptr) {
      c = *--p;
      *p = *ptr;
      *ptr++ = c;
    }
    // restore end pos
    ptr = p1;
  }

  // decimal part
  if (precision) {
    // place decimal point
    *ptr++ = '.';

    // convert
    while (precision--) {
      f *= 10.0;
      c = f;
      *ptr++ = '0' + c;
      f -= c;
    }
  }

  // terminating zero
  *ptr = 0;

  Serial.println(buf);
  print(buf);
}

/*!
 *    @brief  Scroll display a string on the 5x5 LED matrix
 *    @param  string Null-terminated ascii string
 *    @param  stepdelay Milliseconds per scroll step
 */
void Adafruit_Microbit_Matrix::scrollText(char *string, uint8_t stepdelay) {
  setFont(&TomThumb);
  setTextWrap(false);
  setTextColor(LED_ON);

  for (int i = 5; i > (((int16_t)strlen(string) - 1) * -5); i--) {
    setCursor(i, 5);
    clear();
    Adafruit_GFX::print(string);
    delay(stepdelay);
  }
}

/*****************************************************************/

/*!
 *    @brief  Initializes the LED matrix
 */
void Adafruit_Microbit::begin(void) { matrix.begin(); }

/*!
 *    @brief  Request the temperature from the Soft Device
 *    @returns Temperature in Celsius
 */
uint8_t Adafruit_Microbit::getDieTemp(void) {
  int32_t temp = 0;
  uint32_t err_code;

  err_code = sd_temp_get(&temp);
  if (err_code) {
    Serial.print("Temperature Error Code: 0x");
    Serial.print(err_code, HEX);
    Serial.println("");
    return 0;
  }

  return temp / 4;
}

/*********************************************************************/

Adafruit_Microbit_BLESerial *Adafruit_Microbit_BLESerial::_instance = NULL;

/*!
 *    @brief  Create a Nordic UART service interface
 *    @param  req Unused
 *    @param  rdy Unused
 *    @param  rst Unused
 */
Adafruit_Microbit_BLESerial::Adafruit_Microbit_BLESerial(unsigned char req,
                                                         unsigned char rdy,
                                                         unsigned char rst)
    : BLEPeripheral(req, rdy, rst) {
  this->_txCount = 0;
  this->_rxHead = this->_rxTail = 0;
  this->_flushed = 0;
  Adafruit_Microbit_BLESerial::_instance = this;

  addAttribute(this->_uartService);
  addAttribute(this->_uartNameDescriptor);
  setAdvertisedServiceUuid(this->_uartService.uuid());
  addAttribute(this->_rxCharacteristic);
  addAttribute(this->_rxNameDescriptor);
  this->_rxCharacteristic.setEventHandler(
      BLEWritten, Adafruit_Microbit_BLESerial::_received);
  addAttribute(this->_txCharacteristic);
  addAttribute(this->_txNameDescriptor);
}

/*!
 *    @brief  Initialize Nordic UART service interface
 */
void Adafruit_Microbit_BLESerial::begin(...) {
  BLEPeripheral::begin();
#ifdef BLE_SERIAL_DEBUG
  Serial.println(F("Adafruit_Microbit_BLESerial::begin()"));
#endif
}

/*!
 *    @brief Check/flush the UART service pipe
 */
void Adafruit_Microbit_BLESerial::poll() {
  if (millis() < this->_flushed + 100) {
    BLEPeripheral::poll();
  } else {
    flush();
  }
}

/*!
 *    @brief Send any pending data and close BLE connection
 */
void Adafruit_Microbit_BLESerial::end() {
  this->_rxCharacteristic.setEventHandler(BLEWritten, NULL);
  this->_rxHead = this->_rxTail = 0;
  flush();
  BLEPeripheral::disconnect();
}

/*!
 *  @brief Check how many bytes are available to read over Nordic UART
 *  @returns Bytes available to read
 */
int Adafruit_Microbit_BLESerial::available(void) {
  BLEPeripheral::poll();
  int retval = (this->_rxHead - this->_rxTail + sizeof(this->_rxBuffer)) %
               sizeof(this->_rxBuffer);
#ifdef BLE_SERIAL_DEBUG
  Serial.print(F("Adafruit_Microbit_BLESerial::available() = "));
  Serial.println(retval);
#endif
  return retval;
}

/*!
 *    @brief Peek at next byte of UART buffer, without removing it
 *    @returns Byte read, -1 if no data to read (use available() first!)
 */
int Adafruit_Microbit_BLESerial::peek(void) {
  BLEPeripheral::poll();
  if (this->_rxTail == this->_rxHead)
    return -1;
  uint8_t byte = this->_rxBuffer[(this->_rxTail + 1) % sizeof(this->_rxBuffer)];
#ifdef BLE_SERIAL_DEBUG
  Serial.print(F("Adafruit_Microbit_BLESerial::peek() = "));
  Serial.print((char)byte);
  Serial.print(F(" 0x"));
  Serial.println(byte, HEX);
#endif
  return byte;
}

/*!
 *    @brief Read one byte out of UART buffer
 *    @returns Byte read, -1 if no data to read (use available() first!)
 */
int Adafruit_Microbit_BLESerial::read(void) {
  BLEPeripheral::poll();
  if (this->_rxTail == this->_rxHead)
    return -1;
  this->_rxTail = (this->_rxTail + 1) % sizeof(this->_rxBuffer);
  uint8_t byte = this->_rxBuffer[this->_rxTail];
#ifdef BLE_SERIAL_DEBUG
  Serial.print(F("Adafruit_Microbit_BLESerial::read() = "));
  Serial.print((char)byte);
  Serial.print(F(" 0x"));
  Serial.println(byte, HEX);
#endif
  return byte;
}

/*!
 *    @brief Send any pending data in UART buffer
 */
void Adafruit_Microbit_BLESerial::flush(void) {
  if (this->_txCount == 0)
    return;
  this->_txCharacteristic.setValue(this->_txBuffer, this->_txCount);
  this->_flushed = millis();
  this->_txCount = 0;
  BLEPeripheral::poll();
#ifdef BLE_SERIAL_DEBUG
  Serial.println(F("Adafruit_Microbit_BLESerial::flush()"));
#endif
}

/*!
 *    @brief Write one byte out to UART service
 *    @param byte Since data byte to write
 *    @returns 1 on success, 0 on failure
 */
size_t Adafruit_Microbit_BLESerial::write(uint8_t byte) {
  BLEPeripheral::poll();
  if (this->_txCharacteristic.subscribed() == false)
    return 0;
  this->_txBuffer[this->_txCount++] = byte;
  if (this->_txCount == sizeof(this->_txBuffer))
    flush();
#ifdef BLE_SERIAL_DEBUG
  Serial.print(F("Adafruit_Microbit_BLESerial::write("));
  Serial.print((char)byte);
  Serial.print(F(" 0x"));
  Serial.print(byte, HEX);
  Serial.println(F(") = 1"));
#endif
  return 1;
}

/*!
 *    @brief Test if UART service is connected over BLE
 *    @returns True if connected
 */
Adafruit_Microbit_BLESerial::operator bool() {
  bool retval = BLEPeripheral::connected();
#ifdef BLE_SERIAL_DEBUG
  Serial.print(F("Adafruit_Microbit_BLESerial::operator bool() = "));
  Serial.println(retval);
#endif
  return retval;
}

void Adafruit_Microbit_BLESerial::_received(const uint8_t *data, size_t size) {
  for (int i = 0; i < size; i++) {
    this->_rxHead = (this->_rxHead + 1) % sizeof(this->_rxBuffer);
    this->_rxBuffer[this->_rxHead] = data[i];
  }
#ifdef BLE_SERIAL_DEBUG
  Serial.print(F("Adafruit_Microbit_BLESerial::received("));
  for (int i = 0; i < size; i++)
    Serial.print((char)data[i]);
  Serial.println(F(")"));
#endif
}

void Adafruit_Microbit_BLESerial::_received(
    BLECentral & /*central*/, BLECharacteristic &rxCharacteristic) {
  Adafruit_Microbit_BLESerial::_instance->_received(
      rxCharacteristic.value(), rxCharacteristic.valueLength());
}

/*************************************************************************************************/

const uint8_t MICROBIT_SMILE[5] = {B00000000, B01010000, B00000000, B10001000,
                                   B01110000};

const uint8_t Adafruit_Microbit_Matrix::EMPTYHEART[5] = {
    B01010, B10101, B10001, B01010, B00100,
};

const uint8_t Adafruit_Microbit_Matrix::HEART[5] = {
    B01010, B11111, B11111, B01110, B00100,
};

const uint8_t Adafruit_Microbit_Matrix::NO[5] = {
    B10001, B01010, B00100, B01010, B10001,
};

const uint8_t Adafruit_Microbit_Matrix::YES[5] = {
    B00000, B00001, B00010, B10100, B01000,
};
