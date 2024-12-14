// EnvironmentalSensor.h
#pragma once

#include <Adafruit_BME280.h>

class EnvironmentalSensor
{
public:
  struct Data
  {
    float temperature = 0;
    float pressure = 0;
    float humidity = 0;
    float altitude = 0;
  };

  bool init();
  bool read();
  void print();
  Data getData();

private:
  Adafruit_BME280 envSensor;
  Data data;
  static const uint8_t BME280_ADDR = 0x76;
};