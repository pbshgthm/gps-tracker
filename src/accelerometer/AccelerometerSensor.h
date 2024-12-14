#pragma once
#include <Adafruit_LIS3DH.h>

class AccelerometerSensor
{
public:
  struct Data
  {
    float x = 0;
    float y = 0;
    float z = 0;
    float magnitude = 0;
    float diffX = 0;
    float diffY = 0;
    float diffZ = 0;
    float diffMagnitude = 0;
  };

  bool init();
  bool read();
  void print();
  Data getData();

private:
  Adafruit_LIS3DH accSensor;
  Data data;
  static const uint8_t ACCELEROMETER_ADDR = 0x19;
};