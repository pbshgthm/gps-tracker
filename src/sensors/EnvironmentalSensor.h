#pragma once
#include "SensorBase.h"
#include <Adafruit_BME280.h>

class EnvironmentalSensor : public Sensor
{
public:
  struct Data
  {
    bool valid = false;
    float temperature = 0;
    float pressure = 0;
    float humidity = 0;
    float altitude = 0;
  };

  EnvironmentalSensor();
  bool initialize() override;
  bool read() override;
  void print() override;
  Data getData();

private:
  Adafruit_BME280 bme;
  Data data;
  static const uint8_t BME280_ADDR = 0x76;
};