#pragma once
#include "SensorBase.h"
#include <Adafruit_LIS3DH.h>

class AccelerometerSensor : public Sensor
{
public:
  struct Data
  {
    bool valid = false;
    float x = 0;
    float y = 0;
    float z = 0;
    float magnitude = 0;
  };

  AccelerometerSensor();
  bool initialize() override;
  bool read() override;
  void print() override;
  Data getData();

private:
  Adafruit_LIS3DH lis;
  Data data;
  float lastX = 0, lastY = 0, lastZ = 0;
  static const uint8_t ACCELEROMETER_ADDR = 0x19;
};