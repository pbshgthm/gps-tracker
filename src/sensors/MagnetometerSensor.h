#pragma once
#include "SensorBase.h"
#include <QMC5883LCompass.h>

class MagnetometerSensor : public Sensor
{
public:
  struct Data
  {
    bool valid = false;
    int x = 0;
    int y = 0;
    int z = 0;
    float heading = 0;
    String direction = "";
  };

  MagnetometerSensor();
  bool initialize() override;
  bool read() override;
  void print() override;
  Data getData();

private:
  QMC5883LCompass compass;
  Data data;
  static const uint8_t QMC5883L_ADDR = 0x0D;
  String getCompassDirection(float heading);
};