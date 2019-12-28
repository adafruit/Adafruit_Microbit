#ifndef _ADAFRUIT_MICROBIT_H_
#define _ADAFRUIT_MICROBIT_H_

#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <BLEPeripheral.h>
#include <Fonts/TomThumb.h>

#define LED_ON 1
#define LED_OFF 0

void IRQ_MATRIX_HANDLER(void);

/** Class to create Adafruit_GFX interface for 5x5 matrix of micro:bit */
class Adafruit_Microbit_Matrix : public Adafruit_GFX {
public:
  Adafruit_Microbit_Matrix();
  ~Adafruit_Microbit_Matrix(void);
  boolean begin(void);
  void drawPixel(int16_t x, int16_t y, uint16_t color),
      fillScreen(uint16_t color), clear(void), rowHandler(void);
  void show(const uint8_t bitmap[]);
  void scrollText(char *string, uint8_t stepdelay = 150);
  void print(char *string);
  void print(int32_t i);
  void print(int i);
  void print(double f, int precision = 3);

  static const uint8_t EMPTYHEART[5], ///< an empty heart icon
      HEART[5],                       ///< full heart icon
      NO[5],                          ///< X icon
      YES[5];                         ///< Check icon

private:
  void startTimer();

  uint8_t matrix_buffer[3][9];
};

/** Class to use Nordic UART service as a Stream object on micro:bit */
class Adafruit_Microbit_BLESerial : public BLEPeripheral, public Stream {
public:
  Adafruit_Microbit_BLESerial(unsigned char req = BLE_DEFAULT_REQ,
                              unsigned char rdy = BLE_DEFAULT_RDY,
                              unsigned char rst = BLE_DEFAULT_RST);

  void begin(...);
  void poll();
  void end();

  virtual int available(void);
  virtual int peek(void);
  virtual int read(void);
  virtual void flush(void);
  virtual size_t write(uint8_t byte);
  using Print::write;
  virtual operator bool();

private:
  unsigned long _flushed;
  static Adafruit_Microbit_BLESerial *_instance;

  size_t _rxHead;
  size_t _rxTail;
  size_t _rxCount() const;
  uint8_t _rxBuffer[BLE_ATTRIBUTE_MAX_VALUE_LENGTH];
  size_t _txCount;
  uint8_t _txBuffer[BLE_ATTRIBUTE_MAX_VALUE_LENGTH];

  BLEService _uartService = BLEService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
  BLEDescriptor _uartNameDescriptor = BLEDescriptor("2901", "UART");
  BLECharacteristic _rxCharacteristic = BLECharacteristic(
      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWriteWithoutResponse,
      BLE_ATTRIBUTE_MAX_VALUE_LENGTH);
  BLEDescriptor _rxNameDescriptor =
      BLEDescriptor("2901", "RX - Receive Data (Write)");
  BLECharacteristic _txCharacteristic =
      BLECharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLENotify,
                        BLE_ATTRIBUTE_MAX_VALUE_LENGTH);
  BLEDescriptor _txNameDescriptor =
      BLEDescriptor("2901", "TX - Transfer Data (Notify)");

  void _received(const uint8_t *data, size_t size);
  static void _received(BLECentral & /*central*/,
                        BLECharacteristic &rxCharacteristic);
};

/** Class to create hardware interface to BLE/matrix of micro:bit */
class Adafruit_Microbit {
public:
  Adafruit_Microbit_Matrix matrix;        ///< 5x5 graphical matrix
  Adafruit_Microbit_BLESerial BTLESerial; ///< Nordic UART service connection

  void begin(void);

  uint8_t getDieTemp(void);
};
#endif
