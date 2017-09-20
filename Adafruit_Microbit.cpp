#include <Adafruit_Microbit.h>
#include "nrf_soc.h"

#define MATRIX_ROWS 3
#define MATRIX_COLS 9
uint8_t rowpins[MATRIX_ROWS] = {26, 27, 28};
uint8_t colpins[MATRIX_COLS] = {3, 4, 10, 23, 24, 25, 9, 7, 6};

uint8_t pixel_to_row[25] = {1, 2, 1, 2, 1, 
			    3, 3, 3, 3, 3, 
			    2, 1, 2, 3, 2,
			    1, 1, 1, 1, 1,
			    3, 2, 3, 2, 3};

uint8_t pixel_to_col[25] = {1, 4, 2, 5, 3,
			    4, 5, 6, 7, 8,
			    2, 9, 3, 9, 1,
			    8, 7, 6, 5, 4,
			    3, 7, 1, 6, 2};

volatile uint8_t currentRow = 0;

Adafruit_Microbit_Matrix *handle = NULL;

/** TIMTER2 peripheral interrupt handler. This interrupt handler is called whenever there it a TIMER2 interrupt
 * Don't mess with this line. really.
 */
 extern "C"  { void TIMER2_IRQHandler(void) { IRQ_MATRIX_HANDLER();  } }



Adafruit_Microbit_Matrix::Adafruit_Microbit_Matrix() :  Adafruit_GFX(5,5) {
  memset(matrix_buffer, 0x0, MATRIX_COLS * MATRIX_ROWS);
}

Adafruit_Microbit_Matrix::~Adafruit_Microbit_Matrix(void) {}

boolean Adafruit_Microbit_Matrix::begin(void) {
  handle = this;

  for (uint8_t c=0; c<MATRIX_COLS ; c++) {
    pinMode(colpins[c], OUTPUT);
    digitalWrite(colpins[c], HIGH);
  }


  for (uint8_t r=0; r<MATRIX_ROWS ; r++) {
    pinMode(rowpins[r], OUTPUT);
    digitalWrite(rowpins[r], LOW);
  }

  for (uint8_t r=0; r<MATRIX_ROWS; r++) {
    for (uint8_t c=0; c<MATRIX_COLS; c++) {
      matrix_buffer[r][c] = 0;
    }
  }

  currentRow = 0;

  startTimer();

  return true;
}

// Matrix object function called by IRQ handler for each row
// This is not optimized at all but its not so bad either!
void Adafruit_Microbit_Matrix::rowHandler(void) {
  // disable current row
  digitalWrite(rowpins[currentRow], LOW);
  for (uint8_t c=0; c<MATRIX_COLS; c++) {
    digitalWrite(colpins[c], HIGH);
  }

  // go to next row
  if(++currentRow >= MATRIX_ROWS) {
    currentRow = 0;
  }

  //Serial.print("Row: "); Serial.println(currentRow);

  for (uint8_t c=0; c<MATRIX_COLS; c++) {
    if (matrix_buffer[currentRow][c]) {
      digitalWrite(colpins[c], LOW);    // Change column states for new row
    }
  }
  digitalWrite(rowpins[currentRow], HIGH);
}


// This sets up the IRQ for timer 2 to run the matrix refresh.
void Adafruit_Microbit_Matrix::startTimer(void)
{    
  NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;              // Set the timer in Counter Mode
  NRF_TIMER2->TASKS_CLEAR = 1;                           // clear the task first to be usable for later
  NRF_TIMER2->PRESCALER   = 4;  
  NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_16Bit;     //Set counter to 16 bit resolution
  NRF_TIMER2->CC[0] = 1000;                               //Set value for TIMER2 compare register 0
  NRF_TIMER2->CC[1] = 0;                                   //Set value for TIMER2 compare register 1
    
  // Enable interrupt on Timer 2, both for CC[0] and CC[1] compare match events
  NRF_TIMER2->INTENSET = (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos) ;
  NVIC_EnableIRQ(TIMER2_IRQn);
    
  NRF_TIMER2->TASKS_START = 1;               // Start TIMER2
}


void IRQ_MATRIX_HANDLER(void) {
  if ((NRF_TIMER2->EVENTS_COMPARE[0] != 0) && ((NRF_TIMER2->INTENSET & TIMER_INTENSET_COMPARE0_Msk) != 0))
  {
    NRF_TIMER2->EVENTS_COMPARE[0] = 0;         //Clear compare register 0 event 
    handle->rowHandler();
    NRF_TIMER2->CC[0] += 1000;
  }
}

void Adafruit_Microbit_Matrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) return;

  int16_t t;
  switch(rotation) {
   case 1:
    t = x;
    x = WIDTH  - 1 - y;
    y = t;
    break;
   case 2:
    x = WIDTH  - 1 - x;
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
  //Serial.print("Setting row "); Serial.print(row); Serial.print(" & col "); Serial.println(col);

  if(color) 
    matrix_buffer[row][col] = 1;
  else
    matrix_buffer[row][col] = 0;
}

void Adafruit_Microbit_Matrix::clear(void) {
  fillScreen(0);
}

void Adafruit_Microbit_Matrix::fillScreen(uint16_t color) {
  for (uint8_t r=0; r<MATRIX_ROWS; r++) {
    for (uint8_t c=0; c<MATRIX_COLS; c++) {
       matrix_buffer[r][c] = color;
    }
  }
}

/*************************************************************************************************/

void Adafruit_Microbit::begin(void) {
  matrix.begin();
}

uint8_t Adafruit_Microbit::getDieTemp(void) {
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
    
    return temp / 4;
}

/*************************************************************************************************/

Adafruit_Microbit_BLESerial* Adafruit_Microbit_BLESerial::_instance = NULL;

Adafruit_Microbit_BLESerial::Adafruit_Microbit_BLESerial(unsigned char req, unsigned char rdy, unsigned char rst) :
  BLEPeripheral(req, rdy, rst)
{
  this->_txCount = 0;
  this->_rxHead = this->_rxTail = 0;
  this->_flushed = 0;
  Adafruit_Microbit_BLESerial::_instance = this;

  addAttribute(this->_uartService);
  addAttribute(this->_uartNameDescriptor);
  setAdvertisedServiceUuid(this->_uartService.uuid());
  addAttribute(this->_rxCharacteristic);
  addAttribute(this->_rxNameDescriptor);
  this->_rxCharacteristic.setEventHandler(BLEWritten, Adafruit_Microbit_BLESerial::_received);
  addAttribute(this->_txCharacteristic);
  addAttribute(this->_txNameDescriptor);
}

void Adafruit_Microbit_BLESerial::begin(...) {
  BLEPeripheral::begin();
  #ifdef BLE_SERIAL_DEBUG
    Serial.println(F("Adafruit_Microbit_BLESerial::begin()"));
  #endif
}

void Adafruit_Microbit_BLESerial::poll() {
  if (millis() < this->_flushed + 100) {
    BLEPeripheral::poll();
  } else {
    flush();
  }
}

void Adafruit_Microbit_BLESerial::end() {
  this->_rxCharacteristic.setEventHandler(BLEWritten, NULL);
  this->_rxHead = this->_rxTail = 0;
  flush();
  BLEPeripheral::disconnect();
}

int Adafruit_Microbit_BLESerial::available(void) {
  BLEPeripheral::poll();
  int retval = (this->_rxHead - this->_rxTail + sizeof(this->_rxBuffer)) % sizeof(this->_rxBuffer);
  #ifdef BLE_SERIAL_DEBUG
    Serial.print(F("Adafruit_Microbit_BLESerial::available() = "));
    Serial.println(retval);
  #endif
  return retval;
}

int Adafruit_Microbit_BLESerial::peek(void) {
  BLEPeripheral::poll();
  if (this->_rxTail == this->_rxHead) return -1;
  uint8_t byte = this->_rxBuffer[ (this->_rxTail + 1) % sizeof(this->_rxBuffer)];
  #ifdef BLE_SERIAL_DEBUG
    Serial.print(F("Adafruit_Microbit_BLESerial::peek() = "));
    Serial.print((char) byte);
    Serial.print(F(" 0x"));
    Serial.println(byte, HEX);
  #endif
  return byte;
}

int Adafruit_Microbit_BLESerial::read(void) {
  BLEPeripheral::poll();
  if (this->_rxTail == this->_rxHead) return -1;
  this->_rxTail = (this->_rxTail + 1) % sizeof(this->_rxBuffer);
  uint8_t byte = this->_rxBuffer[this->_rxTail];
  #ifdef BLE_SERIAL_DEBUG
    Serial.print(F("Adafruit_Microbit_BLESerial::read() = "));
    Serial.print((char) byte);
    Serial.print(F(" 0x"));
    Serial.println(byte, HEX);
  #endif
  return byte;
}

void Adafruit_Microbit_BLESerial::flush(void) {
  if (this->_txCount == 0) return;
  this->_txCharacteristic.setValue(this->_txBuffer, this->_txCount);
  this->_flushed = millis();
  this->_txCount = 0;
  BLEPeripheral::poll();
  #ifdef BLE_SERIAL_DEBUG
    Serial.println(F("Adafruit_Microbit_BLESerial::flush()"));
  #endif
}

size_t Adafruit_Microbit_BLESerial::write(uint8_t byte) {
  BLEPeripheral::poll();
  if (this->_txCharacteristic.subscribed() == false) return 0;
  this->_txBuffer[this->_txCount++] = byte;
  if (this->_txCount == sizeof(this->_txBuffer)) flush();
  #ifdef BLE_SERIAL_DEBUG
    Serial.print(F("Adafruit_Microbit_BLESerial::write("));
    Serial.print((char) byte);
    Serial.print(F(" 0x"));
    Serial.print(byte, HEX);
    Serial.println(F(") = 1"));
  #endif
  return 1;
}

Adafruit_Microbit_BLESerial::operator bool() {
  bool retval = BLEPeripheral::connected();
  #ifdef BLE_SERIAL_DEBUG
    Serial.print(F("Adafruit_Microbit_BLESerial::operator bool() = "));
    Serial.println(retval);
  #endif
  return retval;
}

void Adafruit_Microbit_BLESerial::_received(const uint8_t* data, size_t size) {
  for (int i = 0; i < size; i++) {
    this->_rxHead = (this->_rxHead + 1) % sizeof(this->_rxBuffer);
    this->_rxBuffer[this->_rxHead] = data[i];
  }
  #ifdef BLE_SERIAL_DEBUG
    Serial.print(F("Adafruit_Microbit_BLESerial::received("));
    for (int i = 0; i < size; i++) Serial.print((char) data[i]);
    Serial.println(F(")"));
  #endif
}

void Adafruit_Microbit_BLESerial::_received(BLECentral& /*central*/, BLECharacteristic& rxCharacteristic) {
  Adafruit_Microbit_BLESerial::_instance->_received(rxCharacteristic.value(), rxCharacteristic.valueLength());
}
